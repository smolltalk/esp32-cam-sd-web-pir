#include "camera.h"

statusCode initCamera() {
  // Disable brownout detector
  // https://iotespresso.com/how-to-disable-brownout-detector-in-esp32-in-arduino/
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  // Disable lamp/flash
  disableLamp();

  camera_config_t config;
  
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 8000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_XGA;
  config.jpeg_quality = 16;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 2;
  config.grab_mode = CAMERA_GRAB_LATEST;

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return cameraInitError;
  }

  // Camera quality adjustments
  sensor_t *s = esp_camera_sensor_get();
  // Adjustments from https://forum.arduino.cc/t/about-esp32cam-image-too-dark-how-to-fix/1015490/5

  s->set_gain_ctrl(s, 1);      // auto gain on
  s->set_exposure_ctrl(s, 1);  // auto exposure on
  s->set_awb_gain(s, 1);       // Auto White Balance enable (0 or 1)
  s->set_brightness(s, 1);

  // Wait until camera is ready: avoid green dark pictures.
  // Less than 1s does not work.
  delay(CAMERA_GET_READY_DURATION);
  return ok;
}

void disableLamp(){
    // Configure Lamp
  ledcSetup(LAMP_CHANNEL, PWM_FREQ, PWM_RESOLUTION);  // configure LED PWM channel
  ledcAttachPin(LAMP_PIN, LAMP_CHANNEL);
  setLamp(0);
}


// Lamp Control
void setLamp(int newVal) {
#if defined(LAMP_PIN)
  if (newVal != -1) {
    // Apply a logarithmic function to the scale.
    int pwmMax = pow(2, PWM_RESOLUTION) - 1;
    int brightness = round((pow(2, (1 + (newVal * 0.02))) - 2) / 6 * pwmMax);
    ledcWrite(LAMP_CHANNEL, brightness);
    Serial.print("Lamp: ");
    Serial.print(newVal);
    Serial.print("%, pwm = ");
    Serial.println(brightness);
  }
#endif
}

statusCode takePicture(camera_fb_t **fb) {
  *fb = NULL;

  // Take Picture with Camera
  *fb = esp_camera_fb_get();
  if (!*fb) {
    Serial.println("Camera capture failed");
    return cameraTakePictureError;
  }
  return ok;
}
