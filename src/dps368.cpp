
#include "config.h"
#include <ArduinoJson.h>
#include <Dps368.h>
#include <SPI.h>

// settings - see datasheet p17 5.1  Measurement Settings and Use Case Examples
// recommended values for:
// Sports (High precision, high rate, background mode)
// PM_PRC = 0x26
//  ..... PM_RATE = 2       4 measurements/second
//  ..... PM_PRC = 6        0110 - 64 times oversampling (High Precision).
//                          note P_SHIFT bit in CFG_REG - must be 1
#define PRESSURE_RATE 2
#define PRESSURE_OVERSAMPLING 6
// TMP_CFG = 0xA0
// ...... TMP_EXT = 1       use external MEMS sensor for temperature
// ...... TMP_RATE = 2      4 measurements/second
// ...... TMP_PRC  = 0      no temperature oversampling
#define TEMPERATURE_RATE 2
#define TEMPERATURE_OVERSAMPLING 0

static Dps368 sensor = Dps368();

static uint8_t temperature_oversampling = 0;
static uint8_t pressure_oversampling = 6;

bool dps368_init(void) {
  sensor.begin(SPI, DPS368_CS);

  // dps368: product=0xf 15 rev=0x1 1
  uint8_t product = sensor.getProductId();
  uint8_t revision = sensor.getRevisionId();
  Serial.printf("dps368: product=0x%x %d rev=0x%x %d\n", product, product,
                revision, revision);

  int16_t n =
      sensor.startMeasureBothCont(TEMPERATURE_RATE, TEMPERATURE_OVERSAMPLING,
                                  PRESSURE_RATE, PRESSURE_OVERSAMPLING);
  if (n != DPS__SUCCEEDED) {
    Serial.printf("startMeasureBothCont 0x%x %d\n", n, n);
    return false;
  }
  return true;
}

bool dps368_update(JsonDocument &jd) {
  float temperature[DPS__FIFO_SIZE];
  float pressure[DPS__FIFO_SIZE];
  uint8_t tempCount, prsCount;
  uint32_t now = micros();
  int16_t r = sensor.getContResults(temperature, tempCount, pressure, prsCount);
  JsonObject j = jd.createNestedObject("dps368");
  j["ts"] = now;

  if (tempCount) {
    JsonArray temps = j.createNestedArray("temps");
    if (temps) {
      for (int i = 0; i < tempCount; i++) {
        temps.add(temperature[i]);
      }
    }
  }
  if (prsCount) {
    JsonArray press = j.createNestedArray("press");
    if (press) {
      for (int i = 0; i < prsCount; i++) {
        press.add(pressure[i] / 100.0);
      }
    }
  }
  return true;
}

// bool dps368_update(JsonDocument &jd) {
//   float temperature;
//   float pressure;
//   JsonObject j = jd.createNestedObject("dps368");
//   if (!sensor.measureTempOnce(temperature, temperature_oversampling)) {
//     j["temp"] = temperature;
//   }
//   if (!sensor.measurePressureOnce(pressure, pressure_oversampling)) {
//     j["pressure"] = pressure / 100.0;
//   }
//   j["ts"] = micros();
//   return true;
// }