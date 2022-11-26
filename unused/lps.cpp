
#include "config.h"
#include <Adafruit_LPS2X.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <Wire.h>

static Adafruit_LPS22 lps22;
static Adafruit_Sensor *lps_temp, *lps_pressure;

bool lps22_init(void) {
  if (!lps22.begin_I2C()) {
    return false;
  }
  lps22.setDataRate(LPS22_RATE_10_HZ);

  Serial.println("LPS2X Found!");
  lps_temp = lps22.getTemperatureSensor();
  lps_temp->printSensorDetails();

  lps_pressure = lps22.getPressureSensor();
  lps_pressure->printSensorDetails();
  return true;
}

bool lps22_update(JsonDocument &jd) {
  sensors_event_t temp;
  sensors_event_t pressure;
  lps_temp->getEvent(&temp);
  lps_pressure->getEvent(&pressure);
  JsonObject j = jd.createNestedObject("lps22");
  j["temp"] = temp.temperature;
  j["pressure"] = pressure.pressure;
  j["ts"] = micros();
  return true;
}
