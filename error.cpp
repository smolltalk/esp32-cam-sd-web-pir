#include "error.h"

void switchOnRedLed() {
  digitalWrite(RED_LED_PIN, LOW);
}

void switchOffRedLed() {
  digitalWrite(RED_LED_PIN, HIGH);
}

void signalError(statusCode code) {
  Serial.print("Signal error: ");
  Serial.println(code);

  // Blink the number of code
  switchOffRedLed();
  delay(2000);
  for (int i = 0; i < code; i++) {
    switchOnRedLed();
    delay(500);
    switchOffRedLed();
    delay(500);
  }
  delay(5000);
}