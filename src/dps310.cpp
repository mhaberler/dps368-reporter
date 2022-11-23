#include "config.h"
#include <Adafruit_DPS310.h>
#include <ArduinoJson.h>

static Adafruit_DPS310 dps;
static Adafruit_Sensor *dps_temp = dps.getTemperatureSensor();
static Adafruit_Sensor *dps_pressure = dps.getPressureSensor();

bool dps310_init(void) {

  if (!dps.begin_I2C()) {
    return false;
  }
  // Setup highest precision
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  dps_temp->printSensorDetails();
  dps_pressure->printSensorDetails();
  return true;
}

bool dps310_update(JsonDocument &jd) {
  sensors_event_t temp_event, pressure_event;

  JsonObject j = jd.createNestedObject("dps310");

  if (dps.pressureAvailable()) {
    dps_pressure->getEvent(&pressure_event);
    j["pressure"] = pressure_event.pressure;
  }
  if (dps.temperatureAvailable()) {
    dps_temp->getEvent(&temp_event);
    j["temp"] = temp_event.temperature;
  }
  j["ts"] = micros();
  return true;
}
