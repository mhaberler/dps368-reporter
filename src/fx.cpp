#include "config.h"
#include <Adafruit_FXOS8700.h>
#include <ArduinoJson.h>

Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);

void displaySensorDetails(void) {
  sensor_t accel, mag;
  accelmag.getSensor(&accel, &mag);
  Serial.println("------------------------------------");
  Serial.println("ACCELEROMETER");
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(accel.name);
  Serial.print("Driver Ver:   ");
  Serial.println(accel.version);
  Serial.print("Unique ID:    0x");
  Serial.println(accel.sensor_id, HEX);
  Serial.print("Min Delay:    ");
  Serial.print(accel.min_delay);
  Serial.println(" s");
  Serial.print("Max Value:    ");
  Serial.print(accel.max_value, 4);
  Serial.println(" m/s^2");
  Serial.print("Min Value:    ");
  Serial.print(accel.min_value, 4);
  Serial.println(" m/s^2");
  Serial.print("Resolution:   ");
  Serial.print(accel.resolution, 8);
  Serial.println(" m/s^2");
  Serial.println("------------------------------------");
  Serial.println("");
  Serial.println("------------------------------------");
  Serial.println("MAGNETOMETER");
  Serial.println("------------------------------------");
  Serial.print("Sensor:       ");
  Serial.println(mag.name);
  Serial.print("Driver Ver:   ");
  Serial.println(mag.version);
  Serial.print("Unique ID:    0x");
  Serial.println(mag.sensor_id, HEX);
  Serial.print("Min Delay:    ");
  Serial.print(accel.min_delay);
  Serial.println(" s");
  Serial.print("Max Value:    ");
  Serial.print(mag.max_value);
  Serial.println(" uT");
  Serial.print("Min Value:    ");
  Serial.print(mag.min_value);
  Serial.println(" uT");
  Serial.print("Resolution:   ");
  Serial.print(mag.resolution);
  Serial.println(" uT");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

bool fx_init(void) {
  if (!accelmag.begin()) {
    return false;
  }
  /* Set accelerometer range (optional, default is 2G) */
  // accelmag.setAccelRange(ACCEL_RANGE_8G);

  /* Set the sensor mode (optional, default is hybrid mode) */
  // accelmag.setSensorMode(ACCEL_ONLY_MODE);

  /* Set the magnetometer's oversampling ratio (optional, default is 7) */
  // accelmag.setMagOversamplingRatio(MAG_OSR_7);

  /* Set the output data rate (optional, default is 100Hz) */
  // accelmag.setOutputDataRate(ODR_400HZ);

  /* Display some basic information on this sensor */
  displaySensorDetails();
  return true;
}

bool fx_update(JsonDocument &jd) {
  sensors_event_t aevent, mevent;
  accelmag.getEvent(&aevent, &mevent);

  JsonObject j = jd.createNestedObject("fxaccel");

  j["x"] = aevent.acceleration.x;
  j["y"] = aevent.acceleration.y;
  j["z"] = aevent.acceleration.z;
  j["ts"] = micros();

  JsonObject m = jd.createNestedObject("fxmag");
  m["x"] = mevent.magnetic.x;
  m["y"] = mevent.magnetic.y;
  m["z"] = mevent.magnetic.z;
  m["ts"] = micros();
  return true;
}