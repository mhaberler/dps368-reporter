
#include <Adafruit_INA219.h>
#include <ArduinoJson.h>
#include <Wire.h>

static Adafruit_INA219 ina219;

bool ina219_init(void) {
  if (!ina219.begin()) {
    return false;
  }
  ina219.setCalibration_32V_1A();
  // ina219.setCalibration_16V_400mA();
  return ina219.success();
}

bool ina219_update(JsonDocument &jd) {

  JsonObject ina = jd.createNestedObject("ina219");
  ina["bus"] = ina219.getBusVoltage_V();
  ina["shunt"] = ina219.getShuntVoltage_mV();
  ina["mA"] = ina219.getCurrent_mA();
  ina["ts"] = micros();
  return true;
}