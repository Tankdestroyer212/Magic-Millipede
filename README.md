# Magic-Millipede
- Dhruv, Addison, and Parker made their own version of a Magic Millipede Exhibit. This project uses an Arduino MEGA wired to a couple of lights, an audio module, and a speaker.
# Project Overview
- Since the exhibit has a live audio narration in a dark room, our goal is to make this experience functional by having the lights and audio turn on and off at the correct times.
- The key features of this project are the "magic" that is shown once the button is pressed. An exterior light will change from green to red, a red light in the interior will turn off, and a narration will begin to run.
# System Architecture
- The whole system is organized around an Arduino, which is the core logic unit.
- We are using a singular breadboard that holds the 2 lights, speaker, and audio module, which is the DFPlayer. 
# Key Components
- I have designed the code in a way that it changes based on the state it is in. There are 2 states, DEFAULT_STATE and PRESENTATION_STATE. Both are dedicated functions that handle everything that happens when you enter the state. This means the changing of the lights, sound, and runtime. switch(currentState) in loop() is the heart of the state machine. It checks which state you are in and runs the logic specific to the state. Within PRESENTATION_STATE, the button simply does not exist, so the presentation will have to run through the full narration before resetting.
# Getting Started
- This design is not very complicated to run. The Arduino must be plugged into the breadboard with cables going to 5V (+) and GND (-). Here are a few things to keep in mind as well:
  - Everything shares the breadboard.
  - The DFPlayer does have very specific wiring, so please refer to an online guide for wiring.
  - The PIN number CAN be changed, but you would have to change the PIN numbers in the code as well. If you want to follow the code's PINs, here they are:
```
#define EXT_LED_PIN   6
#define INT_LED_PIN   5
#define NUM_LEDS      7
#define BUTTON_PIN    7
#define MP3_RX        3
#define MP3_TX        2
```
Here are the dependencies/libraries needed:
```
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
```
# Design Notes
- This code used AI in the process while cleaning up the code. Since C++ is new to me, I ran into many issues while coding and was slowly losing time. AI helped clean up the code and assist in building the state machine, which is also a totally new concept I learned last minute. I do not normally use AI, as I want to challenge myself and learn new things. If there is anything that should/needs cleaning up, please let me know so I can use it to grow and improve!
