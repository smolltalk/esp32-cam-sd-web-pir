#ifndef SD_H
#define SD_H

#include "FS.h"
#include "SD_MMC.h"
#include "error.h"

#define FILES_META_DATA_FILE_NAME "/files.mtd"

typedef struct {
  uint16_t pictureNumber;
  uint16_t uploadedPictureNumber;
} FilesMetaData;

statusCode initSDCard();
void endSDCard();
statusCode savePictureOnSDCard(char * pictureName, uint8_t * buf, size_t len);
statusCode readFilesMetaData(FilesMetaData * filesMetaData);
statusCode writeFilesMetaData(FilesMetaData * filesMetaData);
statusCode readOrCreateFilesMetaData(FilesMetaData * filesMetaData);

#endif