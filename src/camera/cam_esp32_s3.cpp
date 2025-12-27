 #include "src/camera/camera.h"

bool cameraInit(CameraBoard board) {
  if (board != BOARD_ESP32_S3_GENERIC) return false;

  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;

  // ⚠️ پین‌های generic – اگر برد خاصی داشتی بعداً اصلاح می‌کنیم
  config.pin_pwdn  = -1;
  config.pin_reset = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size   = FRAMESIZE_SVGA;
  config.jpeg_quality = 10;
  config.fb_count     = 2;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) return false;

  // auto detect sensor
  sensor_t *s = esp_camera_sensor_get();
  if (s) {
    Serial.printf("[CAM] Sensor PID: 0x%04X\n", s->id.PID);

    if (s->id.PID == OV5640_PID) {
      Serial.println("[CAM] OV5640 detected");
      s->set_framesize(s, FRAMESIZE_UXGA);
    } else {
      Serial.println("[CAM] OV2640 or compatible detected");
      s->set_framesize(s, FRAMESIZE_VGA);
    }
  }

  return true;
}

camera_fb_t* cameraGetFrame() {
  return esp_camera_fb_get();
}

void cameraReturnFrame(camera_fb_t* fb) {
  esp_camera_fb_return(fb);
}

sensor_t* cameraGetSensor() {
  return esp_camera_sensor_get();
}
