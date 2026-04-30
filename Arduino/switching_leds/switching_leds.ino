#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define EXT_LED_PIN   6
#define INT_LED_PIN   5
#define NUM_LEDS      7
#define BUTTON_PIN    7
#define UV_RELAY_PIN  8
#define MP3_RX        3
#define MP3_TX        2

#define PHASE1_END  30000    // 0:30
#define PHASE2_END  90000    // 1:30
#define PHASE3_END  120000   // 2:00

Adafruit_NeoPixel exterior(NUM_LEDS, EXT_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel interior(NUM_LEDS, INT_LED_PIN, NEO_RGB + NEO_KHZ800);

SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFRobotDFPlayerMini player;

enum State {
  DEFAULT_STATE,
  PRESENTATION_PHASE1,
  PRESENTATION_PHASE2,
  PRESENTATION_PHASE3
};

State currentState = DEFAULT_STATE;

unsigned long presentationStartTime = 0;
bool lastButtonState = HIGH;

void setColor(Adafruit_NeoPixel &ring, int r, int g, int b) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, ring.Color(r, g, b));
  }
  ring.show();
}

void uvOn()  { digitalWrite(UV_RELAY_PIN, HIGH); }
void uvOff() { digitalWrite(UV_RELAY_PIN, LOW);  }

void enterDefaultState() {
  currentState = DEFAULT_STATE;
  presentationStartTime = 0;
  uvOff();
  setColor(exterior, 255, 0, 0); // Exterior green
  setColor(interior, 0, 255, 0); // Interior red
  player.stop();
  lastButtonState = HIGH;
}

void enterPhase1() {
  currentState = PRESENTATION_PHASE1;
  presentationStartTime = millis();
  uvOff();
  setColor(exterior, 0, 255, 0); // Exterior red (room occupied)
  setColor(interior, 0, 255, 0); // Interior red stays ON
  delay(500);
  player.play(1);
}

void enterPhase2() {
  currentState = PRESENTATION_PHASE2;
  uvOn();
  setColor(interior, 0, 0, 0);
}

void enterPhase3() {
  currentState = PRESENTATION_PHASE3;
  uvOff();
  setColor(interior, 0, 0, 0);
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(UV_RELAY_PIN, OUTPUT);  // was missing!
  uvOff();                         // was missing!

  exterior.begin();
  exterior.show();
  interior.begin();
  interior.show();

  mp3Serial.begin(9600);
  delay(1000);
  if (player.begin(mp3Serial)) {
    player.volume(20);
  }

  enterDefaultState();
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  switch (currentState) {

    case DEFAULT_STATE:
      if (currentButtonState == LOW && lastButtonState == HIGH) {
        enterPhase1();
      }
      break;

    case PRESENTATION_PHASE1:
    case PRESENTATION_PHASE2:
    case PRESENTATION_PHASE3: {
      // Only calculate elapsed when inside a presentation
      unsigned long elapsed = millis() - presentationStartTime;

      if (currentState == PRESENTATION_PHASE1 && elapsed >= PHASE1_END) {
        enterPhase2();
      }
      else if (currentState == PRESENTATION_PHASE2 && elapsed >= PHASE2_END) {
        enterPhase3();
      }
      else if (currentState == PRESENTATION_PHASE3 && elapsed >= PHASE3_END) {
        enterDefaultState();
      }
      break;
    }
  }

  lastButtonState = currentButtonState;
}