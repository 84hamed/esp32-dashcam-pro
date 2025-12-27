#include <Arduino.h>
#include <esp_camera.h>
#include <SD_MMC.h>
#include "camera.h"

// <<<--- انتخاب بورد --->
#define SELECTED_BOARD BOARD_ESP32_S3_WROOM_CAM     // یا BOARD_ESP32_CAM_AI_THINKER

// <<<--- انتخاب سنسور دوربین --->
#define CAMERA_SENSOR SENSOR_OV5640                 // یا SENSOR_OV2640

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);  // منتظر سریال در S3
  Serial.println("\n=== ESP32 Dashcam Pro - Compatible with Core 3.0.5 ===");

  // راه‌اندازی دوربین
  if (!cameraInit(SELECTED_BOARD, CAMERA_SENSOR)) {
    Serial.println("Camera initialization failed!");
    while (true) delay(1000);
  }
  Serial.println("Camera initialized successfully");

  // راه‌اندازی SD Card - سازگار با نسخه 3.0.5
  if (!SD_MMC.begin("/sdcard", true, true)) {  // 1-bit mode, formatIfMountFailed = true
    Serial.println("SD Card Mount Failed!");
    // اختیاری: فرمت خودکار در صورت نیاز
    // if (SD_MMC.format()) Serial.println("SD Card formatted");
    while (true) delay(1000);
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %llu MB\n", cardSize);

  // تست ساده: یک عکس بگیر و ذخیره کن
  Serial.println("Taking test photo in 3 seconds...");
  delay(3000);
}

void loop() {
  camera_fb_t *fb = cameraGetFrame();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    return;
  }

  // ساخت نام فایل با زمان
  char filename[32];
  snprintf(filename, sizeof(filename), "/photo_%lu.jpg", millis());

  File file = SD_MMC.open(filename, FILE_WRITE);
  if (file) {
    file.write(fb->buf, fb->len);
    file.close();
    Serial.printf("Photo saved: %s (%d bytes)\n", filename, fb->len);
  } else {
    Serial.println("Failed to open file for writing");
  }

  cameraReleaseFrame(fb);

  delay(10000);  // هر ۱۰ ثانیه یک عکس برای تست
}