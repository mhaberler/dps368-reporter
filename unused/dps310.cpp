#include "config.h"
#include <ArduinoJson.h>
#include <Dps310.h>
#include <Wire.h>

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


static Dps310 sensor = Dps310();

bool dps310_init(void) {

  sensor.begin(Wire, DPS__STD_SLAVE_ADDRESS);
  uint8_t product = sensor.getProductId();
  uint8_t revision = sensor.getRevisionId();

  // dps310: product=0x0 0 rev=0x1 1
  Serial.printf("dps310: product=0x%x %d rev=0x%x %d\n", product, product,
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

bool dps310_update(JsonDocument &jd) {
  float temperature[DPS__FIFO_SIZE];
  float pressure[DPS__FIFO_SIZE];
  uint8_t tempCount, prsCount;
  uint32_t now = micros();
  int16_t r = sensor.getContResults(temperature, tempCount, pressure, prsCount);
  JsonObject j = jd.createNestedObject("dps310");
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