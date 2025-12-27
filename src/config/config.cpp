#include "config.h"
#include <FS.h>
 #include <SD_MMC.h>

AppConfig config;  // تعریف جهانی اگر لازم باشه

bool loadConfig() {
  File file = SD_MMC.open("/config.json", "r");
  if (!file) return false;
  // اینجا می‌تونی از ArduinoJson استفاده کنی برای parse واقعی
  // فعلاً ساده فرض می‌کنیم
  file.close();
  return true;
}

bool saveConfig() {
  // مشابه با ArduinoJson ذخیره کن
  return true;
}

void saveDefaultConfig() {
  // ذخیره پیش‌فرض
  saveConfig();
}