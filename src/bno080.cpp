
#include "SparkFun_BNO080_Arduino_Library.h"
#include "config.h"
#include <ArduinoJson.h>
#include <Wire.h>

static BNO080 myIMU;

bool bno_init(void) {

// boolean BNO080::begin(uint8_t deviceAddress, TwoWire &wirePort, uint8_t intPin)

  if (!myIMU.begin(0x60, Wire, 255)) {
    Serial.println("IMU fail");
    return false;
  }
  myIMU.enableMagnetometer(500); // Send data update every 500ms
  return true;
}

bool bno_update(JsonDocument &jd) {
  if (myIMU.dataAvailable() == true) {
    float x = myIMU.getMagX();
    float y = myIMU.getMagY();
    float z = myIMU.getMagZ();
    byte accuracy = myIMU.getMagAccuracy();
    JsonObject j = jd.createNestedObject("bnomag");
    j["acc"] = myIMU.getMagAccuracy();
    j["x"] = x;
    j["y"] = y;
    j["z"] = z;
    j["ts"] = micros();
  }
  return true;
}
