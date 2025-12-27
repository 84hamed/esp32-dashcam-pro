#include <Arduino.h>
#include <esp_camera.h>
#include <SD_MMC.h>

// <<<--- انتخاب بورد اینجا (تغییر بده) --->
#define SELECTED_BOARD BOARD_ESP32_S3_WROOM_CAM  // یا BOARD_ESP32_CAM_AI_THINKER

// <<<--- انتخاب سنسور دوربین اینجا (تغییر بده) --->
#define CAMERA_SENSOR SENSOR_OV5640              // یا SENSOR_OV2640

// تعریف enumها (چون فایل .h جداگانه نداریم، اینجا می‌ذاریم)
enum BoardType { BOARD_ESP32_CAM_AI_THINKER, BOARD_ESP32_S3_WROOM_CAM };
enum CameraSensor { SENSOR_OV2640, SENSOR_OV5640 };

// توابع دوربین (بعداً به فایل جدا منتقل می‌کنیم)
bool cameraInit(BoardType board, CameraSensor sensor);
camera_fb_t* cameraGetFrame();
void cameraReleaseFrame(camera_fb_t* fb);

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("\n=== ESP32 Dashcam Pro - G-Al-S3 Branch ===");

  if (!cameraInit(SELECTED_BOARD, CAMERA_SENSOR)) {
    Serial.println("Camera init failed!");
    while (true) delay(1000);
  }
  Serial.println("Camera OK");

  if (!SD_MMC.begin("/sdcard", true, true)) {
    Serial.println("SD Card Mount Failed");
    while (true) delay(1000);
  }
  Serial.printf("SD Card Size: %llu MB\n", SD_MMC.cardSize() / (1024ULL * 1024));

  Serial.println("System ready - Taking test photo in 5s...");
  delay(5000);
}

void loop() {
  camera_fb_t *fb = cameraGetFrame();
  if (!fb) {
    Serial.println("Capture failed");
    delay(1000);
    return;
  }

  char filename[32];
  snprintf(filename, sizeof(filename), "/dash_%lu.jpg", millis());

  File file = SD_MMC.open(filename, FILE_WRITE);
  if (file) {
    file.write(fb->buf, fb->len);
    file.close();
    Serial.printf("Saved: %s (%d bytes)\n", filename, fb->len);
  } else {
    Serial.println("File open failed");
  }

  cameraReleaseFrame(fb);
  delay(10000);  // هر ۱۰ ثانیه یک عکس
}

// پیاده‌سازی توابع دوربین
bool cameraInit(BoardType board, CameraSensor sensor) {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_pwdn = -1;
  config.pin_reset = -1;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.fb_count = 2;

  if (board == BOARD_ESP32_CAM_AI_THINKER) {
    config.pin_d0 = 5; config.pin_d1 = 18; config.pin_d2 = 19; config.pin_d3 = 21;
    config.pin_d4 = 36; config.pin_d5 = 39; config.pin_d6 = 34; config.pin_d7 = 35;
    config.pin_xclk = 0; config.pin_pclk = 22; config.pin_vsync = 25; config.pin_href = 23;
    config.pin_sscb_sda = 26; config.pin_sscb_scl = 27;
    config.xclk_freq_hz = 20000000;
    config.jpeg_quality = 12;
    config.frame_size = FRAMESIZE_UXGA;
  } else if (board == BOARD_ESP32_S3_WROOM_CAM) {
    config.pin_d0 = 11; config.pin_d1 = 9; config.pin_d2 = 8; config.pin_d3 = 10;
    config.pin_d4 = 12; config.pin_d5 = 18; config.pin_d6 = 17; config.pin_d7 = 16;
    config.pin_xclk = 15; config.pin_pclk = 13; config.pin_vsync = 6; config.pin_href = 7;
    config.pin_sscb_sda = 4; config.pin_sscb_scl = 5;
    config.xclk_freq_hz = (sensor == SENSOR_OV5640) ? 24000000 : 20000000;
    config.jpeg_quality = (sensor == SENSOR_OV5640) ? 10 : 12;
    config.frame_size = (sensor == SENSOR_OV5640) ? FRAMESIZE_QXGA : FRAMESIZE_SVGA;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s) Serial.printf("Detected sensor PID: 0x%04x\n", s->id.PID);

  return true;
}

camera_fb_t* cameraGetFrame() { return esp_camera_fb_get(); }
void cameraReleaseFrame(camera_fb_t* fb) { if (fb) esp_camera_fb_return(fb); }