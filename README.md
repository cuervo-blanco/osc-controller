# OSC Controller (QLab Edition)

This is a little project that came out of a real-world problem during a show:  
we were working with a performer who didn’t have a stage manager to run their sound cues. 
They were on their own. That got us thinking — what if they could run their own cues from the stage? 
Not with a laptop, not with a phone, but with a dedicated piece of hardware that’s simple, and designed just for this.

So, this is that idea — or at least, the first version of it.

It’s a standalone OSC controller that talks to QLab over Wi-Fi. 
It has a rotary knob, an LCD screen, and a few buttons. You scroll through cues, 
trigger them, or run the whole show without needing a screen or mouse or anything else. 

At the moment, this is **just a prototype**. It works — you can set it up, connect it to Wi-Fi, 
enter your QLab IP, and start sending OSC messages. But I’m still developing the code, 
refining the interface, and planning a physical enclosure (probably starting with a cardboard mock-up, 
then moving to a 3D-printed or fabricated case).

I’d love feedback! If this seems useful to you, or you’d like to try one in a show, reach out. 
I might build a few units to share with folks.

---

## Features

- Rotary encoder with push-button input  
- 16x2 I2C LCD screen for menus and text entry  
- Menu-driven UI with support for:
  - Connecting to Wi-Fi
  - Setting the QLab IP and port
  - Creating and editing cues
  - Custom third-button actions (panic, go, stop all, etc.)
  - Switching between English and Spanish
- OTA firmware updates via GitHub
- Preferences and cues saved to flash (so you don’t lose them when powered off)

---

## What You’ll Need

- ESP32 board  
- Rotary encoder (with push button)  
- 3 momentary buttons (Fire, Back, Third Button)  
- 16x2 LCD screen with I2C backpack (address `0x27`)  
- Breadboard or perfboard to prototype  
- Jumper wires, maybe some resistors  
- A QLab machine on the same network

---

## How to Build It

I recommend using [PlatformIO](https://platformio.org/) for building and uploading. 
It handles dependencies automatically and works great with ESP32.

1. Clone this repo:
   ```bash
   git clone https://github.com/yourname/osc-controller.git
   cd osc-controller

2. Build and upload the firmware:

   ```bash
   pio run --target upload
   ```

3. Power it up. You’ll be guided through connecting to Wi-Fi and entering your QLab IP and port.

4. Add your cues using the knob. From there, you can run your show directly from the device.

---

## License

This software is free for personal and non-commercial use.

If you’re thinking of using it commercially or want to integrate it into something you're selling, reach out — I’m open to collaboration or licensing discussions.

