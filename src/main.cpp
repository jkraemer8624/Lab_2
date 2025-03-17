#include <Arduino.h>

// Define pins
#define GREEN_LED 17
#define YELLOW_LED 26
#define RED_LED 12
#define BUZZER 27
#define BUTTON 13

// Time constants for each state
const unsigned long RED_TIME = 10000;
const unsigned long YELLOW_TIME = 2000;
const unsigned long RED_YELLOW_TIME = 2000;
const unsigned long GREEN_TIME = 5000;
const unsigned long BUZZER_GREEN_ON = 500;
const unsigned long BUZZER_GREEN_OFF = 1500;
const unsigned long BUZZER_RED_ON = 250;
const unsigned long BUZZER_RED_OFF = 250;

enum TrafficState {
  STATE_RED,
  STATE_RED_YELLOW,
  STATE_GREEN,  
  STATE_YELLOW
};

TrafficState stateSequence[] = {
  STATE_RED, 
  STATE_RED_YELLOW,
  STATE_GREEN,
  STATE_YELLOW
};

int currentStateIndex = 0;
unsigned long stateStartTime = 0;
unsigned long buzzerStateStart = 0;
bool buttonOn = false;
unsigned long buttonPressTime = 0;

void buzzerFunction(bool red_or_green);
void switchState();

void setup() {
  Serial.begin(115200);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);

  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);

  currentStateIndex = 0;
  stateStartTime = millis();
  buzzerStateStart = millis();
}

void loop() {
  Serial.println("Loop is running...");
  unsigned long right_now = millis();
  unsigned long elapsed = right_now - stateStartTime;

  TrafficState currentState = stateSequence[currentStateIndex];

  if (currentState == STATE_RED) {
    Serial.println("RED state entered");
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);

    buzzerFunction(true);

    if (elapsed >= RED_TIME) {
      switchState();
    }
  }
  else if(currentState == STATE_RED_YELLOW) {
    digitalWrite(RED_LED, HIGH);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    buzzerFunction(true);

    if (elapsed >= RED_YELLOW_TIME) {
      switchState();
    }
  }
  else if(currentState == STATE_GREEN) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);

    buzzerFunction(false);

    if (digitalRead(BUTTON) == LOW) {
      if (!buttonOn) {
        buttonOn = true;
        buttonPressTime = right_now;
      }
    }

    if (buttonOn && (right_now - buttonPressTime >= GREEN_TIME)) {
      switchState();
    }
  }
  else if(currentState == STATE_YELLOW) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);

    noTone(BUZZER); 

    if (elapsed >= YELLOW_TIME) {
      switchState();
    }
  }
}

// red = True, green = false
void buzzerFunction(bool red_or_green) {
  unsigned long right_now = millis();
  unsigned long elapsed = right_now - buzzerStateStart;

  unsigned long period, onTimer;
  if (red_or_green) {
    period = BUZZER_RED_ON + BUZZER_RED_OFF;
    onTimer = BUZZER_RED_ON;
  } else {
    period = BUZZER_GREEN_ON + BUZZER_GREEN_OFF;
    onTimer = BUZZER_GREEN_ON;
  }

  if (elapsed >= period) {
    buzzerStateStart = right_now;
    elapsed = 0;
  }

  if (elapsed < onTimer) {
    tone(BUZZER, 2000);
  } else {
    noTone(BUZZER);
  }
}

void switchState() {
  // Move to next stateIndex
  currentStateIndex++;

  // If at yellow wrap back around to red
  if (currentStateIndex >= 4) {
    currentStateIndex = 0;
  }

  stateStartTime = millis();
  buzzerStateStart = millis();
  buttonOn = false;
}