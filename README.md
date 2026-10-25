# Crescendo

<img width="797" height="441" alt="3D_Front" src="https://github.com/user-attachments/assets/613ef26c-153e-4bfb-98ba-9ae9e0fd3a4c" />

A crescendo is a custom MIDI keyboard capable of producing music from several different instruments and recording it as a MIDI file.

# Features
- Sounds of different instruments
- Can record music (MIDI) to be edited on a DAW
- 7 keys - 7 natural notes
- Capacitive touch keys
- Transpose functions
- Metronome and tempo additions
- Volume changing
- Compact PCB design

# Why it was built
The Crescendo was built for the Hack Club YSWS Programme [Halflife](https://halflife.hackclub.com/), drawing inspiration from the starter project [MIDI-Keypad](https://github.com/SharKingStudios/Starbie/blob/main/README.md](https://github.com/KaiPereira/MIDI-Keypad/tree/master). 

# Components
This project was built using the following components:
* WaveESP32-S3 Mini Development Board
* EC11 Rotary Encoder Module
* Kailh switches
* DSA keycaps

# Building it!
Drawing inspiration from the [MIDI-Keypad](https://github.com/SharKingStudios/Starbie/blob/main/README.md](https://github.com/KaiPereira/MIDI-Keypad/tree/master), I decided to make a minimal, budget-friendly version of this keypad - featuring 7 capacitive touch keys for each of the natural notes, volume, transpose, metronome & tempo, sounds of different instruments, and a recording feature. I have been playing the piano for six years now, so even if I don't know enough about PCB design, I do know a bit about music. 

Initially, I searched up part prices and decided which ones I could use, due to my costly microcontroller I had to cut down on other costs.

Then, I figured out how to install all the libraries, and then connected the components on the schematic. Initially, I didn't understand how the capacitive touch keys worked, so I added actual touch sensors to my BOM and my schematic - I was so happy when I could remove them! (- $3)

![Schematic_Initial](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/9767a8e7234c842a3e5da6db817b0a9a40159527fe4bf587d5a4d28173d70d5f.png)
^ My initial schematic with 7 touch sensors

I used all the pins on my esp32-s3, and my final schematic looks like this-
![Schematic_Final](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/087bde03c69eaf87923fbb53dd76950eef567bf3a00eb3a1771360b98014f9d9.png)

After assigning footprints, I updated the PCB from the schematic and began to make my rectangular capacitive touch keys. I've decided to make them 2x5 cm - big enough to play them, but small enough that the PCB size doesn't increase drastically. I made the traces as direct as possible, and routed them. I used the front copper layer for all the traces except the ground ones, as  I found that I had enough space for all of them. 

I then added doodles to the front and the back, and finally, I added the 3D models from the MIDI repo and the Week 1 Starbie care package. 


# What I learnt
This was one of the first hardware projects I've done, and the first music-realted project I've done. I learnt a lot, including how to - 
- add footprint & symbol libraries in KiCAD, and add 3D models
- connect rotary encoders in the schematic
- add capacitive touch keys in KiCAD, with the Waveshare ESP32-S3
- add images to PCB silkscreens
- work with MIDI keyboards

As soon as I finish the firmware - I will update this column >_<.

# BOM

| Part | What it's for | Qty | Unit | Total | Vendor |
| --- | --- | --- | --- | --- | --- |
| [XIAO-ESP32-C3](https://robu.in/product/seeed-studio-xiao-esp32c3-tiny-mcu-board-with-wi-fi-and-ble-battery-charge-supported-power-efficiency-and-rich-interface/) | Microcontroller | 1 | $7.74 | $7.74 | [Robu.in](https://robu.in/product/seeed-studio-xiao-esp32c3-tiny-mcu-board-with-wi-fi-and-ble-battery-charge-supported-power-efficiency-and-rich-interface/) |
| [MAX98357A Amplifier](https://robu.in/product/smartelex-i2s-audio-breakout-max98357a/) | Amp for the speaker | 1 | $2.00 | $2.00 | [Robu.in](https://robu.in/product/smartelex-i2s-audio-breakout-max98357a/) |
| [DHT11](https://robocraze.com/products/dht11-humidity-temperature-sensor-module-1) | Environment sensor | 1 | $0.53 | $0.53 | [Robocraze.com](https://robocraze.com/products/dht11-humidity-temperature-sensor-module-1) |
| [MPU6050](https://robocraze.com/products/mpu-6050-triple-axis-accelerometer-gyroscope-module) | Accelerometer & gyroscope | 1 | $1.56 | $1.56 | [Robocraze.com](https://robocraze.com/products/mpu-6050-triple-axis-accelerometer-gyroscope-module) |
| [0.96 OLED](https://robocraze.com/products/0-96in-oled-display-module-4pin) | Display | 1 | $1.75 | $1.75 | [Robocraze.com](https://robocraze.com/products/0-96in-oled-display-module-4pin) |
| [Kailh Switches (pack of 10)](https://meckeys.com/shop/accessories/keyboard-accessories/key-switches/kailh-switch-pack/) | Controls (buttons) | 1 | $1.45 | $1.45 | [Meckeys.com](https://meckeys.com/shop/accessories/keyboard-accessories/key-switches/kailh-switch-pack/) |
| [Blank DSA Keycaps (Set of 5)](https://meckeys.com/shop/accessories/keyboard-accessories/keycaps/blank/blank-dsa-keycaps-1u/) | Keycaps for the buttons | 1 | $1.03 | $1.03 | [Meckeys.com](https://meckeys.com/shop/accessories/keyboard-accessories/keycaps/blank/blank-dsa-keycaps-1u/) |
| [Speaker (1 Watt 8 Ohm oval mini)](https://probots.co.in/8-ohm-1w-1510-mm-oval-mini-audio-speaker.html) | Talking! | 1 | $0.51 | $0.51 | [Probots](https://probots.co.in/8-ohm-1w-1510-mm-oval-mini-audio-speaker.html) |
| [PCB](https://robu.in/product/online-pcb-manufacturing-service/) | Circuitry & connections | 1 | $9.30 | $9.30 | [Robu.in](https://robu.in/product/online-pcb-manufacturing-service/) |
| **Parts subtotal** | — | — | — | **$25.87** | — |
| **Tax & shipping** | — | — | — | **$3.06** | — |
| **Total** | — | — | — | **$28.93** | — |

# Images and Renders
Final schematic- 
![Schematic_Final](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/087bde03c69eaf87923fbb53dd76950eef567bf3a00eb3a1771360b98014f9d9.png)

This is my current PCB-
![PCB_Front](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/6aa5e69052f96f651646956931a36a81257f29c9112d212d4c94896cd8b7f05d.png)

And its backside-
![PCB_Back](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/7c2ff2df67a20922fdda3acb00f76b9dbfa15024675bff892ffd602f72989c03.png)


![3D_Front](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/35777f014485e26a9da6ed3f78f788d47be212345f80b6fc96b7feced681db6a.png)
^ The front side

![3D_Back](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/90955b29ef3c499768dcb03f5aeab2ee410ba3ba219df09f4a99489dba2d49aa.png)
^ And the back

# Credits
This project would not have been possible without the constant support of the Halflife community, and the detailed guidance given in the MIDI-Keypad guide. I loved designing this project and I can't wait to build the PCB, and create my own music!


