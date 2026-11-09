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
The Crescendo was built for the Hack Club YSWS Programme [Halflife](https://halflife.hackclub.com/), drawing inspiration from the project [MIDI-Keypad](https://github.com/KaiPereira/MIDI-Keypad/tree/master). 

# Components
This project was built using the following components:
* WaveESP32-S3 Mini Development Board
* EC11 Rotary Encoder Module
* Kailh switches & DSA Keycaps
* A custom PCB!

# Building it!
Drawing inspiration from the [MIDI-Keypad](https://github.com/KaiPereira/MIDI-Keypad/tree/master), I decided to make a minimal, budget-friendly version of this keypad - featuring 7 capacitive touch keys for each of the natural notes, a rotary encoder for volume adjustment, another for transpose, and two Kailh switches - to play sounds of different instruments, and to record your music. Another switch is used to switch on and off the metronome, and if you press the switch on the first rotary encoder, you can use it to change the tempo. I have been playing the piano for six years now, so even if I don't know enough about PCB design, I do know a bit about music. 

Initially, I searched up part prices and decided which ones I could use, due to my costly microcontroller I had to cut down on other costs.

Then, I figured out how to install all the libraries, and then connected the components on the schematic. Initially, I didn't understand how the capacitive touch keys worked, so I added actual touch sensors to my BOM and my schematic - I was so happy when I could remove them! (- $3)

![Schematic_Initial](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/9767a8e7234c842a3e5da6db817b0a9a40159527fe4bf587d5a4d28173d70d5f.png)
^ My initial schematic with 7 touch sensors

I used all the pins on my esp32-s3, and my final schematic looks like this-
![Schematic_Final](https://halflife.hackclub-assets.com/sessions/GCeaAMEwysPJCYY5Y3iNPYstz2oSVP5D/087bde03c69eaf87923fbb53dd76950eef567bf3a00eb3a1771360b98014f9d9.png)

After assigning footprints, I updated the PCB from the schematic and began to make my rectangular capacitive touch keys. I've decided to make them 1.36x4 cm - big enough to play them, but small enough that the PCB size remains under 100x100mm, to cut costs. I made the traces as direct as possible, and routed them. I used the front copper layer for all the traces except the ground ones, as  I found that I had enough space for all of them. 

I then added doodles to the front and the back, and finally, I added the 3D models from the MIDI repo and the Week 1 Starbie care package.

# Firmware (EDIT)

# What I learnt (EDIT)
This was one of the first hardware projects I've done, and the first music-realted project I've done. I learnt a lot, including how to - 
- add footprint & symbol libraries in KiCAD, and add 3D models
- connect rotary encoders in the schematic
- add capacitive touch keys in KiCAD, with the Waveshare ESP32-S3
- add images to PCB silkscreens
- work with MIDI keyboards
- ensure PCBs remain under 100x100mm >_<

As soon as I finish the firmware - I will update this column >_<.

# BOM (EDIT)



# Images and Renders (EDIT)
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
This project would not have been possible without the constant support of the Halflife community, and the detailed guidance given in the MIDI-Keypad guide. I loved designing this project and I can't wait to build the PCB and create my own music!


