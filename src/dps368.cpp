
#include "config.h"
#include <ArduinoJson.h>
#include <Dps368.h>
#include <SPI.h>

static Dps368 sensor = Dps368();

static uint8_t oversampling = 7;

bool dps368_init(void) {
  sensor.begin(SPI, DPS368_CS);

  return true;
}

bool dps368_update(JsonDocument &jd) {
  float temperature;
  float pressure;
  JsonObject j = jd.createNestedObject("dps368");
  if (!sensor.measureTempOnce(temperature, oversampling)) {
    j["temp"] = temperature;
  }
  if (!sensor.measurePressureOnce(pressure, oversampling)) {
    j["pressure"] = pressure/100.0;
  }
  j["ts"] = micros();
  return true;
}