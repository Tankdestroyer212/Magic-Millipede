#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define EXT_LED_PIN   6
#define INT_LED_PIN   5
#define NUM_LEDS      7
#define BUTTON_PIN    7
#define UV_RELAY_PIN  4
#define MP3_RX        3
#define MP3_TX        2

#define PHASE1 = 30000 // intro narration
#define PHASE2 = 60000 // UV light and bio narration
#define PHASE3 = 30000 // light music + dark room

Adafruit_NeoPixel exterior(NUM_LEDS, EXT_LED_PIN, NEO_RGB + NEO_KHZ800);
Adafruit_NeoPixel interior(NUM_LEDS, INT_LED_PIN, NEO_RGB + NEO_KHZ800);

SoftwareSerial mp3Serial(MP3_RX, MP3_TX);
DFRobotDFPlayerMini player;

enum State {
  DEFAULT_STATE,
  PRESENTATION_PHASE1
  PRESENTATION_PHASE2
  PRESENTATION_PHASE3
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

void uvOn()  { digitalWrite(UV_RELAY_PIN, HIGH); }
void uvOff() { digitalWrite(UV_RELAY_PIN, LOW);  }

void enterDefaultState() {
  currentState = DEFAULT_STATE;
  ufOff();
  player.top();
  setColor(exterior, 255, 0, 0); // Exterior green
  setColor(interior, 0, 255, 0); // Interior red
}

void enterPhase1() {
  currentState = PRESENTATION_PHASE1;
  stateStartTime = millis();
  uvOff();
  setColor(exterior, 0, 255, 0); // Exterior red (room occupied)
  setColor(interior, 0, 255, 0); // Interior red stays ON
  delay(1000);
  player.play(1);                // Track 1: welcome narration + countdown
}

void enterPhase2() {
currentState = PRESENTATION_PHASE2;
  uvOn();                        // UV ON at 0:30 — F2.3, F6.3
  setColor(interior, 0, 0, 0);  // Interior red OFF — F2.3
}

void enterPhase3() {
  currentState = PRESENTATION_PHASE3;
  uvOff();                       // UV OFF at 1:30 — F2.3, F6.3
  setColor(interior, 0, 0, 0);  // Room stays dark — F2.3
}

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  exterior.begin();
  exterior.show();
  interior.begin();
  interior.show();

  mp3Serial.begin(9600);
  delay(1000);
  if (player.begin(mp3Serial)) {
    player.volume(20);
  }

  enterDefaultState(); // Start in default state
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);
  unsigned long elapsed = millis() - presentationStartTime;

  switch (currentState) {

    case DEFAULT_STATE:
      // F2.1 — wait for button press
      // F6.4 — only triggers on clean rising edge, ignores rapid presses
      if (currentButtonState == LOW && lastButtonState == HIGH) {
        enterPhase1();
      }
      break;

    case PRESENTATION_PHASE1:
      // F2.4 — button completely ignored during presentation
      // F2.3 — transition to phase 2 at 0:30
      if (elapsed >= PHASE1_END) {
        enterPhase2();
      }
      break;

    case PRESENTATION_PHASE2:
      // F2.4 — button completely ignored during presentation
      // F2.3, F6.3 — UV turns off at 1:30
      if (elapsed >= PHASE2_END) {
        enterPhase3();
      }
      break;

    case PRESENTATION_PHASE3:
      // F2.4 — button completely ignored during presentation
      // F2.5 — reset to default at 2:00
      if (elapsed >= PHASE3_END) {
        enterDefaultState();
      }
      break;
  }

  lastButtonState = currentButtonState;
}