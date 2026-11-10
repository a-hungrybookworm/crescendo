/*

CRESCENDO - FIRMWARE

Pins:
- 7 capacitive touch keys (GPIO7 to GPIO13) -> 7 natural notes (C4 = MIDI 60), channel 1
- SW1 (GPIO6)                               -> metronome on/ off (clicks on MIDI channel 10)
- SW2 (RX / GPIO44)                         -> instrument change, channel 1
- SW3 (TX / GPIO43)                         -> recording start/ stop
- Encoder SW4 (GPIO1 / GPIO2, switch GPIO3) -> volume, if switch pressed then tempo
- Encoder SW5 (GPIO4 / GPIO5)               -> transpose (-12 to 12 semitones)
- On-board RGB LED (GPIO 21)                -> red = recording, blue = metronome on, yellow blinks at power-up while touch keys are calibrated

Arduino IDE Settings:
Board           -> "ESP32S3 Dev Module" (or "Waveshare ESP32-S3-Zero")
USB Mode        -> "USB-OTG (TinyUSB)"
USB CDC On Boot -> "Disabled" (set "Enabled" only if DEBUG_TOUCH is 1)
This sketch requires the esp32 Arduino core v3.x.

While flashing, plug in while holding BOOT, and then upload code. After uploading, press RESET so it registers as a MIDI device.

MIDI messages-
- Channel 1: Note On/Off, Program Change (instrument), CC7 (volume)
- Channel 10: metronome clicks (note 76 = downbeat, 77 = other beats)
- Channel 16: status CCs
    CC20/21 tempo MSB/LSB   CC22 transpose (+64)   CC23 metronome (0/127)
    CC24 recording (0/127)  CC25 knob mode (0 vol / 127 tempo)
    CC26 instrument index   CC27 volume mirror
    CC127 (sent by the computer) -> asks the keypad for a full status update

*/

#include <Arduino.h>                                            // core arduino functions

#if ARDUINO_USB_MODE                                            // ensure USB mode is set to tiny usb - needed for MIDI
#error "Check whether USB Mode is set to USB-OTG (TinyUSB)"
#endif

#include "USB.h"                                               // controls esp32s native usb port
#include "USBMIDI.h"                                           // board appears as MIDI device
#include "soc/gpio_reg.h"                                      // low-level registers names for speed

#if ESP_ARDUINO_VERSION_MAJOR < 3                              // ensures either name for LED function works
#define rgbLedWrite neopixelWrite                              // incase of old esp cores
#endif

USBMIDI MIDI;                                                  // MIDI object - all MIDI messages are sent through this

// Pins
const uint8_t KEY_PINS[7]   = {7, 8, 9, 10, 11, 12, 13};   // KEY1..KEY7 (capacitive touch keys)
const uint8_t PIN_BTN_METRO = 6;    // SW1
const uint8_t PIN_BTN_INSTR = 44;   // SW2 (RX pin)
const uint8_t PIN_BTN_REC   = 43;   // SW3 (TX pin)
const uint8_t PIN_VOL_A     = 1;    // SW4 encoder A  (volume / tempo)
const uint8_t PIN_VOL_B     = 2;    // SW4 encoder B
const uint8_t PIN_MODE_SW   = 3;    // SW4 push switch (volume <-> tempo)
const uint8_t PIN_TRN_A     = 4;    // SW5 encoder A  (transpose)
const uint8_t PIN_TRN_B     = 5;    // SW5 encoder B
const uint8_t PIN_LED       = 21;   // on-board WS2812

// Notes (MIDI numbers)
const uint8_t BASE_NOTES[7] = {60, 62, 64, 65, 67, 69, 71};  // C4 D4 E4 F4 G4 A4 B4
const uint8_t VELOCITY      = 100;  // fixed loudness of hit (touch keys have no velocity)

// MIDI channels (1-based)
const uint8_t CH_NOTES  = 1;        // notes on channel 1
const uint8_t CH_METRO  = 10;       // metronome on channel 10 - general MIDI percussion channel
const uint8_t CH_STATUS = 16;       // status messages on channel 16 - separate so doesn't interfere with music

// Status CC numbers (must match the host page)
enum : uint8_t {
  CC_TEMPO_MSB = 20, CC_TEMPO_LSB = 21, CC_TRANSPOSE = 22, CC_METRO = 23,      // tempo (2 parts), transpose, metronome
  CC_REC = 24, CC_MODE = 25, CC_INSTR = 26, CC_VOLUME = 27, CC_REQUEST = 127   // recording, knob mode, instrument, volume, status request
};

// Ranges
const int TEMPO_MIN = 40, TEMPO_MAX = 240, TEMPO_DEFAULT = 120;   // tempo range
const int TRANSPOSE_MIN = -12, TRANSPOSE_MAX = 12;                // transpose range
const int VOLUME_STEP = 4;                                        // volume change per encoder click
const uint8_t BEATS_PER_BAR = 4;                                  // time signature

// Encoders
#define ENC_STEPS_PER_DETENT 4      // most EC11 encoders: 4. If one click = 2 steps, set 2.
#define ENC_VOL_REVERSE 0           // set 1 to reverse direction
#define ENC_TRN_REVERSE 0

// Touch tuning (ESP32-S3: reading RISES when touched)
const float TOUCH_ON_RATIO   = 1.15f;   // "pressed" when reading > baseline * this
const float TOUCH_OFF_RATIO  = 1.08f;   // "released" when reading < baseline * this
const float BASELINE_DRIFT   = 0.01f;   // slow re-calibration while untouched
const uint8_t TOUCH_CONFIRM  = 2;       // consecutive samples to confirm a change
const uint32_t TOUCH_INTERVAL_MS = 4;   // between readings

#define DEBUG_TOUCH 0   // 1 = print raw touch values (needs "USB CDC On Boot: Enabled")

// Instruments (General MIDI program numbers, 0-based)
const uint8_t INSTRUMENT_PROGRAMS[] = {
  0,   // Acoustic Grand Piano
  4,   // Electric Piano
  11,  // Vibraphone
  19,  // Church Organ
  24,  // Nylon Guitar
  40,  // Violin
  48,  // String Ensemble
  56,  // Trumpet
  73,  // Flute
  80,  // Square Lead
};
const uint8_t NUM_INSTRUMENTS = sizeof(INSTRUMENT_PROGRAMS); // number of instruments in list

// States
int  volume    = 100;        // volume (0-127)
int  tempo     = TEMPO_DEFAULT;   // tempo (BPM)
int  transpose = 0;          // transpose (semitones)
uint8_t instrIdx = 0;        // instrument
bool metroOn   = false;      // metronome on/ off
bool recording = false;      // recording on/ off
enum KnobMode : uint8_t { MODE_VOLUME = 0, MODE_TEMPO = 1 };   // what SW4 encoder controls
KnobMode knobMode = MODE_VOLUME;                               // starts in volume mode

// Encoders
struct Encoder {
  uint8_t pinA, pinB;
  volatile uint8_t state;          // last-seen pin pattern
  volatile int8_t  acc;            // accumulated partial movement
  volatile int16_t steps;          // clicks not yet processed
};

Encoder encVol = {PIN_VOL_A, PIN_VOL_B, 0, 0, 0};        // volume/ tempo encoder
Encoder encTrn = {PIN_TRN_A, PIN_TRN_B, 0, 0, 0};        // transpose encoder
portMUX_TYPE encMux = portMUX_INITIALIZER_UNLOCKED;      // stops interrupt and main loops from touching steps at the same moment

// Quadrature transition table: index = (prev << 2) | curr -> +1 forward, -1 back, 0 no change/ bounce
static const int8_t QDEC[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

static inline uint8_t IRAM_ATTR fastRead(uint8_t pin) {   // reads pin directly from chips input register
  return (REG_READ(GPIO_IN_REG) >> pin) & 1;              // pins 0..31 only
}

static inline void IRAM_ATTR encUpdate(Encoder &e) {             // runs when encoder pin changes
  uint8_t s = (fastRead(e.pinA) << 1) | fastRead(e.pinB);        // reads A and B into a 2-bit number
  portENTER_CRITICAL_ISR(&encMux);                               // take lock
  e.acc += QDEC[(e.state << 2) | s];                             // looks up movement in table and adds to acc
  e.state = s;                                                   // remembers state for next run
  if (e.acc >= ENC_STEPS_PER_DETENT)       { e.steps++; e.acc -= ENC_STEPS_PER_DETENT; } // once four steps accumulate in one direction,
  else if (e.acc <= -ENC_STEPS_PER_DETENT) { e.steps--; e.acc += ENC_STEPS_PER_DETENT; } // records one full click in steps
#if ENC_STEPS_PER_DETENT == 4
  if (s == 3) e.acc = 0;                    // resync at the rest position
#endif
  portEXIT_CRITICAL_ISR(&encMux);         // release lock
}

void IRAM_ATTR isrVol() { encUpdate(encVol); }  // interrupt handlers placed in internal RAM
void IRAM_ATTR isrTrn() { encUpdate(encTrn); }

int16_t takeSteps(Encoder &e) {  // called in main loop
  portENTER_CRITICAL(&encMux);   // grabs completed clicks and count resets to 0
  int16_t s = e.steps;           // copies clicks
  e.steps = 0;                   // resets count
  portEXIT_CRITICAL(&encMux);    // release lock
  return s;
}

// Buttons
struct Button {
  uint8_t pin;
  bool stable;        // confirmed state - debounced level (HIGH = released, pull-up)
  bool lastRead;      // most recent raw reading
  uint32_t changedAt; // time raw reading last changed
};

Button btnMetro = {PIN_BTN_METRO, HIGH, HIGH, 0}; // metronome
Button btnInstr = {PIN_BTN_INSTR, HIGH, HIGH, 0}; // instrument change
Button btnRec   = {PIN_BTN_REC,   HIGH, HIGH, 0}; // recording
Button btnMode  = {PIN_MODE_SW,   HIGH, HIGH, 0}; // encoder push

// Returns true once per press (falling edge after debounce)
bool pressed(Button &b) {
  bool r = digitalRead(b.pin);                                    // raw reading
  uint32_t now = millis();
  if (r != b.lastRead) { b.lastRead = r; b.changedAt = now; }     // reading changed - note the time
  if (r != b.stable && (now - b.changedAt) > 25) {                // accept only if steady for 25 ms (debounce)
    b.stable = r;
    return b.stable == LOW;                                       // true only on press (LOW), not release
  }
  return false;
}

// Touch
float   baseline[7];                // resting reading of each key
bool    keyDown[7]    = {false};    // whether currently held
uint8_t confirmCnt[7] = {0};        // confirmation counter
uint8_t activeNote[7] = {0};        // actual note sent (for transpose)

// MIDI Output

void sendStatus(uint8_t cc, uint8_t value) { MIDI.controlChange(cc, value, CH_STATUS); } // status control change - channel 16

void sendTempo() {                                 // splits tempo into two parts (MIDI values max 127) - host page re-assembles them
  sendStatus(CC_TEMPO_MSB, (tempo >> 7) & 0x7F);   // high part
  sendStatus(CC_TEMPO_LSB, tempo & 0x7F);          // low 7 bits
}

void sendVolume() {
  MIDI.controlChange(7, volume, CH_NOTES);   // real MIDI volume (works in any DAW/synth) - channel 1
  sendStatus(CC_VOLUME, volume);             // sends copy on status channel
}

void sendInstrument() {
  MIDI.programChange(INSTRUMENT_PROGRAMS[instrIdx], CH_NOTES);  // sends program change to switch instrument
  sendStatus(CC_INSTR, instrIdx);                               // position in list - display
}

// Status only on channel 16
void sendStatusSnapshot() {                                 // sends every status value at once
  sendTempo();                                              // tempo
  sendStatus(CC_TRANSPOSE, transpose + 64);                 // transpose (+64 so negative values fit in 0-127)
  sendStatus(CC_METRO, metroOn ? 127 : 0);                  // metronome
  sendStatus(CC_REC, recording ? 127 : 0);                  // recording
  sendStatus(CC_MODE, knobMode == MODE_TEMPO ? 127 : 0);    // knob mode
  sendStatus(CC_INSTR, instrIdx);                           // instrument
  sendStatus(CC_VOLUME, volume);                            // volume
}

void sendFullState() {        // instrument, volume & status messages - startup & when computer asks
  MIDI.programChange(INSTRUMENT_PROGRAMS[instrIdx], CH_NOTES);    // instrument
  MIDI.controlChange(7, volume, CH_NOTES);                        // volume
  sendStatusSnapshot();                                           // status
}

// LED
void updateLed() {
  if (recording)    rgbLedWrite(PIN_LED, 40, 0, 0);   // red - when recording
  else if (metroOn) rgbLedWrite(PIN_LED, 0, 0, 25);   // blue - when metronome is on
  else              rgbLedWrite(PIN_LED, 0, 0, 0);    // off
}

// Metronome
uint32_t nextBeatUs = 0;          // when next beat is due
uint8_t  beatIdx    = 0;          // which beat is playing (of bar)
bool     clickOn    = false;      // whether click note is currently sounding
uint32_t clickOffUs = 0;          // when to stop click note
uint8_t  clickNote  = 0;          // which click note is sounding

void restartBar() {              // starts bar - jumps to beat 1
  beatIdx = 0;
  nextBeatUs = micros();
}

void serviceMetronome() {                               // called from loop
  uint32_t now = micros();
  if (clickOn && (int32_t)(now - clickOffUs) >= 0) {    // if click sounds for 40 ms, sends its note-off
    MIDI.noteOff(clickNote, 0, CH_METRO);
    clickOn = false;
  }
  if (!metroOn) return;                                 // stop here if metronome is off
  if ((int32_t)(now - nextBeatUs) >= 0) {               // checks whether time for next beat (safe when micros() wraps)
    if (clickOn) MIDI.noteOff(clickNote, 0, CH_METRO);  // stops previous click if still sounding
    bool down = (beatIdx == 0);                         // first beat of bar = downbeat
    clickNote = down ? 76 : 77;                         // Hi / Low Wood Block (GM drums)
    MIDI.noteOn(clickNote, down ? 127 : 90, CH_METRO);  // downbeat louder
    clickOn = true;
    clickOffUs = now + 40000;                           // click lasts 40 ms
    beatIdx = (beatIdx + 1) % BEATS_PER_BAR;            // next beat, wraps after 4
    nextBeatUs += 60000000UL / (uint32_t)tempo;         // schedules next beat (microseconds per beat) - no drift
    if ((int32_t)(now - nextBeatUs) > 0) nextBeatUs = now + 60000000UL / (uint32_t)tempo;  // resync if loop fell behind
  }
}

// Keys
void keyPress(uint8_t i) {
  int n = constrain((int)BASE_NOTES[i] + transpose, 0, 127);  // adds transpose to key's base note, clamps to MIDI range
  activeNote[i] = (uint8_t)n;                                 // remember, so note-off matches
  MIDI.noteOn(activeNote[i], VELOCITY, CH_NOTES);             // sends note on
}

void keyRelease(uint8_t i) {
  MIDI.noteOff(activeNote[i], 0, CH_NOTES);                   // note off for the note that was started
}

void calibrateTouch() {
  for (uint8_t i = 0; i < 7; i++) touchRead(KEY_PINS[i]);   // warm-up
  for (uint8_t i = 0; i < 7; i++) baseline[i] = 0;          // clear totals
  const int N = 24;                                         // number of readings per key
  for (int s = 0; s < N; s++) {
    rgbLedWrite(PIN_LED, (s & 2) ? 30 : 0, (s & 2) ? 20 : 0, 0);   // yellow blink
    for (uint8_t i = 0; i < 7; i++) baseline[i] += touchRead(KEY_PINS[i]);   // adds up readings
    delay(20);                                              // 20 ms apart
  }
  for (uint8_t i = 0; i < 7; i++) baseline[i] /= N;         // average resting reading
  rgbLedWrite(PIN_LED, 0, 0, 0);                            // LED off
}

void serviceTouch() {
  static uint32_t last = 0;                         // time of last scan
  uint32_t now = millis();
  if (now - last < TOUCH_INTERVAL_MS) return;       // scan only every 4 ms
  last = now;

  for (uint8_t i = 0; i < 7; i++) {
    float raw = (float)touchRead(KEY_PINS[i]);      // reads key
    if (!keyDown[i]) {                              // key currently up
      if (raw > baseline[i] * TOUCH_ON_RATIO) {     // above "on" threshold
        if (++confirmCnt[i] >= TOUCH_CONFIRM) { keyDown[i] = true; confirmCnt[i] = 0; keyPress(i); }   // confirmed - play note
      } else {
        confirmCnt[i] = 0;
        baseline[i] += (raw - baseline[i]) * BASELINE_DRIFT;   // slowly tracks drift (temperature, humidity)
      }
    } else {                                        // key currently held
      if (raw < baseline[i] * TOUCH_OFF_RATIO) {    // below "off" threshold
        if (++confirmCnt[i] >= TOUCH_CONFIRM) { keyDown[i] = false; confirmCnt[i] = 0; keyRelease(i); }   // confirmed - stop note
      } else {
        confirmCnt[i] = 0;
      }
    }
  }

#if DEBUG_TOUCH && ARDUINO_USB_CDC_ON_BOOT          // only when debugging
  static uint32_t lastPrint = 0;
  if (now - lastPrint > 200) {                      // prints 5 times a second
    lastPrint = now;
    for (uint8_t i = 0; i < 7; i++) {               // raw reading / baseline, * = pressed
      Serial.printf("K%d %6lu/%6lu%s  ", i + 1, (unsigned long)touchRead(KEY_PINS[i]),
                    (unsigned long)baseline[i], keyDown[i] ? "*" : " ");
    }
    Serial.println();
  }
#endif
}

// Controls

void serviceButtons() {
  if (pressed(btnMetro)) {                       // SW1 - metronome
    metroOn = !metroOn;                          // toggle on/ off
    if (metroOn) restartBar();                   // start on beat 1
    sendStatus(CC_METRO, metroOn ? 127 : 0);     // report to computer
    updateLed();
  }
  if (pressed(btnInstr)) {                       // SW2 - instrument
    instrIdx = (instrIdx + 1) % NUM_INSTRUMENTS; // next instrument, wraps to first
    sendInstrument();
  }
  if (pressed(btnRec)) {                         // SW3 - recording
    recording = !recording;                      // toggle on/ off
    sendStatus(CC_REC, recording ? 127 : 0);     // report to computer
    if (recording && metroOn) restartBar();      // recording starts on a downbeat
    updateLed();
  }
  if (pressed(btnMode)) {                        // SW4 push - knob mode
    knobMode = (knobMode == MODE_VOLUME) ? MODE_TEMPO : MODE_VOLUME;   // flip volume <-> tempo
    sendStatus(CC_MODE, knobMode == MODE_TEMPO ? 127 : 0);             // report to computer
  }
}

void serviceEncoders() {
  int16_t v = takeSteps(encVol);                 // clicks from volume/ tempo encoder
#if ENC_VOL_REVERSE
  v = -v;                                        // reverse direction if set
#endif
  if (v) {
    if (knobMode == MODE_VOLUME) {                                  // volume mode
      int nv = constrain(volume + v * VOLUME_STEP, 0, 127);         // 4 per click, clamped
      if (nv != volume) { volume = nv; sendVolume(); }              // send only if changed
    } else {                                                        // tempo mode
      int step = (abs(v) > 1) ? v * 4 : v;       // spin fast = bigger jumps
      int nt = constrain(tempo + step, TEMPO_MIN, TEMPO_MAX);       // clamped to tempo range
      if (nt != tempo) { tempo = nt; sendTempo(); }                 // send only if changed
    }
  }

  int16_t t = takeSteps(encTrn);                 // clicks from transpose encoder
#if ENC_TRN_REVERSE
  t = -t;                                        // reverse direction if set
#endif
  if (t) {
    int nt = constrain(transpose + t, TRANSPOSE_MIN, TRANSPOSE_MAX);                 // 1 semitone per click, clamped
    if (nt != transpose) { transpose = nt; sendStatus(CC_TRANSPOSE, transpose + 64); }   // send only if changed
  }
}

void handleIncomingMidi() {                      // reads messages from the computer
  midiEventPacket_t p;                           // one USB-MIDI packet (header + 3 bytes)
  while (MIDI.readPacket(&p)) {
    // Host asks for a status refresh: CC127 on channel 16
    if ((p.header & 0x0F) == 0x0B && p.byte1 == (0xB0 | (CH_STATUS - 1)) && p.byte2 == CC_REQUEST) {
      sendFullState();                           // reply with current state
    }
  }
}

void serviceHeartbeat() {                        // re-sends status every 3 s so display always catches up
  static uint32_t last = 0;
  if (millis() - last > 3000) { last = millis(); sendStatusSnapshot(); }
}

// Setup/ loop

void setup() {                                   // runs once at power-up
  pinMode(PIN_BTN_METRO, INPUT_PULLUP);          // inputs with internal pull-up resistors
  pinMode(PIN_BTN_INSTR, INPUT_PULLUP);          // (switches wired straight to GND)
  pinMode(PIN_BTN_REC,   INPUT_PULLUP);
  pinMode(PIN_MODE_SW,   INPUT_PULLUP);
  pinMode(PIN_VOL_A, INPUT_PULLUP);
  pinMode(PIN_VOL_B, INPUT_PULLUP);
  pinMode(PIN_TRN_A, INPUT_PULLUP);
  pinMode(PIN_TRN_B, INPUT_PULLUP);

  USB.productName("Touch MIDI Keypad");          // device name shown on computer
  USB.manufacturerName("DIY");
  MIDI.begin();                                  // register MIDI first,
  USB.begin();                                   // then start USB

#if DEBUG_TOUCH && ARDUINO_USB_CDC_ON_BOOT
  Serial.begin(115200);                          // serial output - debug only
#endif

  encVol.state = (fastRead(PIN_VOL_A) << 1) | fastRead(PIN_VOL_B);   // starting position of encoders
  encTrn.state = (fastRead(PIN_TRN_A) << 1) | fastRead(PIN_TRN_B);
  attachInterrupt(PIN_VOL_A, isrVol, CHANGE);    // interrupt on any change of either encoder pin
  attachInterrupt(PIN_VOL_B, isrVol, CHANGE);
  attachInterrupt(PIN_TRN_A, isrTrn, CHANGE);
  attachInterrupt(PIN_TRN_B, isrTrn, CHANGE);

  delay(300);                // lets USB and power settle
  calibrateTouch();          // keep fingers off the keys for ~0.5 s after plug-in
  sendFullState();           // initial state to computer
  updateLed();
}

void loop() {                // runs forever - each function does a quick check and returns
  handleIncomingMidi();      // messages from computer
  serviceTouch();            // touch keys
  serviceButtons();          // buttons
  serviceEncoders();         // encoder turns
  serviceMetronome();        // metronome
  serviceHeartbeat();        // status every 3 s
}
