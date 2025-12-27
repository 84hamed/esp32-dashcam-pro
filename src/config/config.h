#pragma once
 #include <Arduino.h>

enum AppWifiMode { APP_WIFI_AP, APP_WIFI_STA };

struct AppConfig {
  AppWifiMode wifi_mode = APP_WIFI_STA;
  bool auto_record = true;

  // این‌ها رو بعداً از WiFiManager یا وب‌سرور پر کن
  String sta_ssid = "YOUR_SSID";
  String sta_password = "YOUR_PASSWORD";
  String ap_ssid = "ESP32-Dashcam";
  String ap_password = "12345678";
};

bool loadConfig();
bool saveConfig();
void saveDefaultConfig();