
#include <Adafruit_BMP3XX.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "config.h"

static Adafruit_BMP3XX bmp;

bool bmp390_init(void) {
  if (!bmp.begin_I2C(BMP390_ADDR)) {
    return false;
  }
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
  return true;
}

bool bmp390_update(JsonDocument &jd) {
  if (!bmp.performReading()) {
    return false;
  }
  sensors_event_t temp;
  sensors_event_t pressure;
  JsonObject j = jd.createNestedObject("bmp390");
  j["temp"] = bmp.temperature;
  j["pressure"] = bmp.pressure/100.0;
  j["altitude"] = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  j["ts"] = micros();
  return true;
}