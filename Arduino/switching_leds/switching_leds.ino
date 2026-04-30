#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define EXT_LED_PIN  6
#define INT_LED_PIN  5
#define NUM_LEDS     7
#define BUTTON_PIN   7
#define UV_RELAY_PIN 8
#define MP3_RX       3
#define MP3_TX       2

Adafruit_NeoPixel exterior(NUM_LEDS, EXT_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel interior(NUM_LEDS, INT_LED_PIN, NEO_RGB + NEO_KHZ800);

SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFRobotDFPlayerMini player;

#define DEFAULT    0
#define PHASE1  1
#define PHASE2  2
#define PHASE3  3

int currentState = DEFAULT;
unsigned long phaseStart = 0;

void setColor(Adafruit_NeoPixel &ring, int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

void uvOn()  { digitalWrite(UV_RELAY_PIN, LOW);  }
void uvOff() { digitalWrite(UV_RELAY_PIN, HIGH); }

void triggerAudio(int track) {
  delay(100);
  player.play(track);
  delay(100);
}

void resetAudio() {
  player.stop();
  delay(500);
}

void setup() {
  Serial.begin(9600);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(UV_RELAY_PIN, OUTPUT);
  uvOff();

  exterior.begin();
  exterior.show();
  setColor(exterior, 255, 0, 0); // exterior green

  interior.begin();
  interior.show();
  setColor(interior, 0, 255, 0); // interior red

  delay(1000);
  mp3Serial.begin(9600);
  delay(1000);
  if (player.begin(mp3Serial)) {
    Serial.println("DFPlayer connected!");
    player.volume(20);
  } else {
    Serial.println("DFPlayer failed");
  }
}

void loop() {
  switch (currentState) {

    case DEFAULT:
      if (digitalRead(BUTTON_PIN) == LOW) {
        currentState = PHASE1;
        phaseStart = millis();
        setColor(exterior, 0, 255, 0); // exterior red
        setColor(interior, 0, 255, 0); // interior red stays on
        triggerAudio(1);               // play track 1
      }
      break;

    case PHASE1:
      if (millis() - phaseStart >= 30000) {
        currentState = PHASE2;
        phaseStart = millis();
        uvOn();                        // UV on
        setColor(interior, 0, 0, 0);  // interior off
      }
      break;

    case PHASE2:
      if (millis() - phaseStart >= 60000) {
        currentState = PHASE3;
        phaseStart = millis();
        uvOff();                       // UV off
        setColor(interior, 0, 0, 0);  // room stays dark
      }
      break;

    case PHASE3:
      if (millis() - phaseStart >= 30000) {
        currentState = DEFAULT;
        uvOff();
        resetAudio();                  // reset audio for next cycle
        setColor(exterior, 255, 0, 0); // exterior green
        setColor(interior, 0, 255, 0); // interior red
      }
      break;
  }
}