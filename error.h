#ifndef ERROR_H
#define ERROR_H

#include "Arduino.h"

// Red LED
#define RED_LED_PIN 33

enum statusCode {
  ok = 0,
  cameraInitError = 1,
  cameraTakePictureError = 2,
  sdInitError = 3,
  sdReadError = 4,
  sdWriteError = 5,
  uploadWifiInitError = 6,
  uploadPictureError = 7
};

void switchOnRedLed();
void switchOffRedLed();
void signalError(statusCode code);

#endif