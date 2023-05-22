#include "sd.h"

statusCode initSDCard() {
  //Serial.println("Starting SD Card");
  if (!SD_MMC.begin("/sd", true)) {
    Serial.println("SD Card Mount Failed");
    return sdInitError;
  }

  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD Card attached");
    return sdInitError;
  }

  Serial.println("SD Card mounted.");
  return ok;
}

void endSDCard() {
  Serial.println("SD Card unmounted.");
  SD_MMC.end();
}

statusCode readFilesMetaData(FilesMetaData * filesMetaData){
  fs::FS &fs = SD_MMC;
  File file = fs.open(FILES_META_DATA_FILE_NAME, FILE_READ);
  if (file) {
    size_t len = sizeof(FilesMetaData);
    return (file.read((uint8_t *) filesMetaData, len) == len)?ok:sdReadError;
  }
  // Else
  return sdReadError;
}

statusCode writeFilesMetaData(FilesMetaData * filesMetaData){
  Serial.println("writeFilesMetaData...");
  fs::FS &fs = SD_MMC;
  File file = fs.open(FILES_META_DATA_FILE_NAME, FILE_WRITE);
  if (file) {
    Serial.printf("File %s open in writing mode.\n", FILES_META_DATA_FILE_NAME);
    size_t len = sizeof(FilesMetaData);
    return (file.write((uint8_t *) filesMetaData, len) == len)?ok:sdWriteError;
  }
  Serial.printf("Failed to open file %s in writing mode.\n", FILES_META_DATA_FILE_NAME);
  // Else
  return sdWriteError;
}

statusCode createFilesMetaData(FilesMetaData * filesMetaData){
  Serial.println("createFilesMetaData...");
  filesMetaData->pictureNumber = 0;
  filesMetaData->uploadedPictureNumber = 0;
  return writeFilesMetaData(filesMetaData);
}

statusCode readOrCreateFilesMetaData(FilesMetaData * filesMetaData){
  Serial.println("readOrCreateFilesMetaData...");
  return (readFilesMetaData(filesMetaData) == ok)?ok:createFilesMetaData(filesMetaData);
}

statusCode savePictureOnSDCard(char * pictureName, uint8_t * buf, size_t len) {
  statusCode result = ok;
  if ((result = initSDCard()) != ok){
    return result;
  }

  // Path where new picture will be saved in SD Card
  // Format file name like "picture-xxxxx.jpg"
  // where x is the current picture number with leading 0.
  char path[20];
  sprintf(path, "/%s", pictureName);

  fs::FS &fs = SD_MMC;
  Serial.printf("Picture file name: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file in writing mode");
    result = sdWriteError;
  } else {
    file.write(buf, len);  // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path);
  }
  file.close();
  return result;
}
