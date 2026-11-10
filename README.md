# Crescendo
<img width="800" height="600" alt="front_3d" src="https://github.com/user-attachments/assets/71c44107-5d24-4251-a24d-dee8d8b28d0c" />


The crescendo is a custom MIDI keyboard capable of producing music from several different instruments and recording it as a MIDI file.

# Features
- Music from different instruments
- Can record music (MIDI) to be edited on a DAW
- 7 keys for 7 natural notes
- Capacitive touch keys
- Transpose functions
- Metronome and tempo additions
- Volume adjustment
- Compact PCB design

# Why it was built
The Crescendo was built for the Hack Club YSWS Programme [Halflife](https://halflife.hackclub.com/), drawing inspiration from the project [MIDI-Keypad](https://github.com/KaiPereira/MIDI-Keypad/tree/master). 

# Components
This project was built using the following components:
* WaveESP32-S3 Mini Development Board
* EC11 Rotary Encoder Modules
* Kailh switches & DSA Keycaps
* A custom PCB!

# Building it!
Drawing inspiration from the [MIDI-Keypad](https://github.com/KaiPereira/MIDI-Keypad/tree/master), I decided to make a minimal, budget-friendly version of this project - featuring 7 capacitive touch keys for each of the natural notes, volume, transpose, metronome & tempo, sounds of different instruments, and a recording feature. I have been playing the piano for six years now, so even if I don't know enough about PCB design, I do know a bit about music. 

Initially, I searched up part prices and decided which ones I could use - I had to cut down on costs as I had a costly microcontroller.

Then I figured out how to install all the footprint libraries, and connected the components on a KiCAD schematic. Initially, I didn't understand how the capacitive touch keys worked, so I added actual touch sensors to my BOM and my schematic - I was so happy when I could remove them! (- $3)

![Schematic_Initial](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/9767a8e7234c842a3e5da6db817b0a9a40159527fe4bf587d5a4d28173d70d5f.png)
^ My initial schematic with 7 touch sensors

I used all the pins on my esp32-s3, and my final schematic looks like this-
![Schematic_Final](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/087bde03c69eaf87923fbb53dd76950eef567bf3a00eb3a1771360b98014f9d9.png)

After assigning footprints, I updated the PCB from the schematic and began to make a footprint for my rectangular capacitive touch keys. I've decided to make them 1.36 x 4cm - big enough to play them, but small enough that the PCB size remains under 100 x 100mm. I made the traces as direct as possible, and routed them. I used the front copper layer for all the traces except the ground ones, as  I found that I had enough space for all of them. 

I then added doodles to the front and the back, and finally, I added the 3D models from the MIDI repo and the Week 1 Starbie care package - finishing my PCB design!

# What I learnt 
This was one of the first hardware projects I've done, and the first music-related project I've done. I learnt a lot, including how to - 
- add footprint & symbol libraries in KiCAD, and add 3D models
- connect rotary encoders in the schematic
- add capacitive touch keys in KiCAD, with the Waveshare ESP32-S3
- add images to PCB silkscreens
- work with MIDI keyboards
- ensure PCBs remain under 100x100mm >_<

# BOM 

| Item | Qty | Supplier | Cost/item | Total Cost | Link |
|------|-----|----------|-----------|------------|------|
| WaveESP32-S3 Mini Development Board | 1 | Waveshare | $5.49 | $5.49 | [ESP32-S3](https://www.waveshare.com/esp32-s3-zero.htm?sku=25081) |
| EC11 Rotary Encoder Module | 2 | Robocraze | $0.74 | $1.48 | [EC11](https://robocraze.com/products/ec11-15mm-rotary-encoder-module?_pos=1&_sid=6ba8a4a86&_ss=r) |
| Kailh Switches (pack of 10) | 1 | Meckeys | $1.45 | $1.45 | [Kailh Switches](https://meckeys.com/shop/accessories/keyboard-accessories/key-switches/kailh-switch-pack/) |
| Blank DSA Keycaps (Set of 5) | 1 | Meckeys | $1.03 | $1.03 | [DSA Keycaps](https://meckeys.com/shop/accessories/keyboard-accessories/keycaps/blank/blank-dsa-keycaps-1u/) |
| PCB | 1 | Robu | $9.59 | $9.59 | [PCB](https://robu.in/product/online-pcb-manufacturing-service/) |
| **Parts subtotal** | | | | **$19.04** | |
| **Estimated total shipping** | | | | **$14.04** | |
| **Grand Total** | | | | **$33.08** | |

# Images and Renders 

The final schematic - 
![Schematic_Final](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/087bde03c69eaf87923fbb53dd76950eef567bf3a00eb3a1771360b98014f9d9.png)

The Crescendo PCB - 
<img width="1104" height="887" alt="front_pcb" src="https://github.com/user-attachments/assets/ab190b91-cabc-49e5-8012-5b1bc44455ab" />

And its backside -
<img width="1049" height="999" alt="back_pcb" src="https://github.com/user-attachments/assets/17b648f3-0ba1-492e-9dbb-99e6f4d5632a" />

<img width="1414" height="1062" alt="front_3d" src="https://github.com/user-attachments/assets/531e332d-3e7e-4db6-933a-3a1c7057c22e" />
^ The front (3D viewer)

<img width="1355" height="1053" alt="back_3d" src="https://github.com/user-attachments/assets/b6551385-dc42-4bb6-a268-660d346a0995" />
^ And the back

# Credits
This project would not have been possible without the constant support of the Halflife community, and the detailed guidance given in the MIDI-Keypad guide. I loved designing this project and can't wait to build the PCB!


