 #include "src/camera/camera.h"

bool cameraInit(CameraType type) {
  if (type != CAM_ESP32_S3_OV5640) return false;

  // init OV5640 here
  return true;
}
