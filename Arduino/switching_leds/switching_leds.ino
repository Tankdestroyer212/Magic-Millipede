#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define EXT_LED_PIN   6
#define INT_LED_PIN   5
#define NUM_LEDS      7
#define BUTTON_PIN    7
#define MP3_RX        3
#define MP3_TX        2

Adafruit_NeoPixel exterior(NUM_LEDS, EXT_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel interior(NUM_LEDS, INT_LED_PIN, NEO_RGB + NEO_KHZ800);

SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFRobotDFPlayerMini player;

bool redActive = false;
unsigned long redStartTime = 0;
bool lastButtonState = HIGH;

void setColor(Adafruit_NeoPixel &ring, int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  exterior.begin();
  exterior.show();
  setColor(exterior, 255, 0, 0); // Starts green

  interior.begin();
  interior.show();
  setColor(interior, 0, 255, 0); // Starts red

  mp3Serial.begin(9600);
  if (player.begin(mp3Serial)) {
    player.volume(25);
  }
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  if (redActive) {
    if (now - redStartTime >= 60000) {
      redActive = false;
      setColor(exterior, 255, 0, 0);
      setColor(interior, 0, 255, 0);
    }
  }
  else {
    if (currentButtonState == LOW && lastButtonState == HIGH) {
      redActive = true;
      redStartTime = now;
      setColor(exterior, 0, 255, 0);
      setColor(interior, 0, 0, 0);
      player.play(1);
    }
  }

  lastButtonState = currentButtonState;
}