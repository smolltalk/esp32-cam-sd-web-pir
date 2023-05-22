/*********
  MakerSpace56
  Sébastien Morvan

  Thanks to Rui Santos
  for its project details at https://RandomNerdTutorials.com/esp32-cam-take-photo-save-microsd-card
  See also https://randomnerdtutorials.com/esp32-timer-wake-up-deep-sleep/
  
  IMPORTANT!!! 
   - Select Board "AI Thinker ESP32-CAM"
   - GPIO 0 must be connected to GND to upload a sketch
   - After connecting GPIO 0 to GND, press the ESP32-CAM on-board RESET button to put your board in flashing mode
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// TODO DURATION_WAKEUP ? ON ISR ?
// TODO avoid String in upload

#include "camera.h"
#include "upload.h"
#include "sd.h"
#include "error.h"
#include "filename.h"

void zzzzZZZZ() {
  // Switch off the red led to inform that the program is stopped
  switchOffRedLed();
  // Wake up on up edge on pin 13
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, 1);
  // Go to sleep
  Serial.println("Going to sleep now");
  Serial.flush();
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

statusCode runActions() {
  statusCode result = ok;
  bool pictureSavedOnSd = false;
  camera_fb_t *fb = NULL;
  char pictureName[20];
  FilesMetaData filesMetaData;
  connectionInfo_t connectionInfo = {.serverName = UPLOAD_SERVER_NAME, .serverPort = UPLOAD_SERVER_PORT, .uploadPath = UPLOAD_SERVER_PATH};

  // Init camera
  if ((result = initCamera()) != ok) {
    return result;
  }

  // Take picture and save it
  if ((result = takePicture(&fb)) != ok) {
    return result;
  }

  if (SAVE_PICTURE_ON_SD_CARD) {
    if ((result = initSDCard()) == ok && (result = readOrCreateFilesMetaData(&filesMetaData)) == ok) {
      computePictureNameFromIndex(pictureName, filesMetaData.pictureNumber + 1);
      if ((result = savePictureOnSDCard(pictureName, fb->buf, fb->len)) == ok) {
        filesMetaData.pictureNumber ++;
        writeFilesMetaData(&filesMetaData);
        pictureSavedOnSd = true;
      }
    }
  }

  if (UPLOAD_PICTURE) {
      if (!pictureSavedOnSd){
        statusCode uploadResult;
        // Failed to saved on SD card, then try to upload.
        computePictureNameFromRandom(pictureName);
        uploadResult = uploadPicture(&connectionInfo, pictureName, fb->buf, fb->len);
        // Don't override result when result already contains an error code.
        if (result == ok){
          result = uploadResult;
        }
      }
      else {
        // Upload a bunch of files.
        result = uploadPictureFiles(&connectionInfo, &filesMetaData);
      }
      endWifi();
  }
  endSDCard();

  return result;
}

void setup() {
  statusCode result;
  // Switch on the red led to inform that the program is running
  pinMode(RED_LED_PIN, OUTPUT);
  switchOnRedLed();
  // Init serial
  Serial.begin(115200);
  
  // Run all actions
  result = runActions();
  signalError(result);

  // Wait to avoid photo galore
  delay(AWAKE_DURATION);
  zzzzZZZZ();
}

void loop() {
}
