#ifndef UPLOADER_H
#define UPLOADER_H

#include "Arduino.h"
#include <HTTPClient.h>
#include "FS.h"
#include "config.h"
#include "sd.h"
#include "filename.h"
#include "error.h"

typedef struct {
  char * serverName;
  int serverPort;
  char * uploadPath;
} connectionInfo_t;

statusCode initWifi();
void endWifi();
statusCode uploadPictureFile(connectionInfo_t * connectionInfo, uint16_t i);
bool canUploadPictures(FilesMetaData * filesMetaData);
statusCode uploadPictureFiles(connectionInfo_t * connectionInfo, FilesMetaData * filesMetaData);
statusCode uploadPicture(connectionInfo_t * connectionInfo, char * pictureName, uint8_t * buf, size_t len);

class DataUploader {
public:
  DataUploader(connectionInfo_t * connectionInfo, uint32_t dataLen, String fileName);
  statusCode upload();

protected:
  connectionInfo_t * connectionInfo;
  uint32_t dataLen;
  String fileName;
  WiFiClient client;

private:
  virtual void sendData();
};

class BufferDataUploader : public DataUploader {

public:
  BufferDataUploader(connectionInfo_t * connectionInfo, uint8_t * buf, size_t len, String fileName);

private:
  uint8_t * buf;

  void sendData();

};

class FileDataUploader : public DataUploader {
public:
  FileDataUploader(connectionInfo_t * connectionInfo, File file);

private:
  File file;

  void sendData();
};

#endif
