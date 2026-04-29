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

enum State {
  DEFAULT_STATE,
  PRESENTATION_STATE
};

State currentState = DEFAULT_STATE;

unsigned long stateStartTime = 0;
bool lastButtonState = HIGH;

void setColor(Adafruit_NeoPixel &ring, int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

void enterDefaultState() {
  currentState = DEFAULT_STATE;
  setColor(exterior, 255, 0, 0); // Exterior green
  setColor(interior, 0, 255, 0); // Interior red
}

void enterPresentationState() {
  currentState = PRESENTATION_STATE;
  stateStartTime = millis();
  setColor(exterior, 0, 255, 0); // Exterior red
  setColor(interior, 0, 0, 0);   // Interior off
  player.play(1);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  exterior.begin();
  exterior.show();
  interior.begin();
  interior.show();

  mp3Serial.begin(9600);
  if (player.begin(mp3Serial)) {
    player.volume(25);
  }

  enterDefaultState(); // Start in default state
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long now = millis();

  switch (currentState) {

      case DEFAULT_STATE:
        // Wait for button press to transition
        if (currentButtonState == LOW && lastButtonState == HIGH) {
          enterPresentationState();
        }
        break;

      case PRESENTATION_STATE:
        // Button is locked — just wait for timer
        if (now - stateStartTime >= 10000) {
          enterDefaultState();
        }
        break;
  }

  lastButtonState = currentButtonState;
}