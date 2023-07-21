#include <ArduinoJson.h>
#include <Dps3xx.h>
#include <Wire.h>

#define I2C_SDA 4
#define I2C_SCL 5

#define DPS368_I2C_ADDR 0x76 // pull down INT/GPIO3 pin - default is 0x77
#define INTERVAL 500         // msec
#define JSON_SIZE 4096

StaticJsonDocument<JSON_SIZE> doc;

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

static Dps3xx sensor = Dps3xx();

bool dps368_i2c_init(void) {

  sensor.begin(Wire, DPS368_I2C_ADDR);
  uint8_t product = sensor.getProductId();
  uint8_t revision = sensor.getRevisionId();

  // dps368: product=0x0 0 rev=0x1 1
  Serial.printf("dps368 i2c: product=0x%x %d rev=0x%x %d\n", product, product,
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

bool dps368_i2c_update(JsonDocument &jd) {
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

void i2cScanner(TwoWire &wire) {
  Serial.printf("I2C scanner. Scanning bus..\n");
  byte count = 0;

  for (byte i = 8; i < 125; i++) {
    wire.beginTransmission(i);       // Begin I2C transmission Address (i)
    if (wire.endTransmission() == 0) // Receive 0 = success (ACK response)
    {
      Serial.printf("found i2c device at 0x%x\n", i);
      count++;
    }
  }
  Serial.printf("Found %d devices\n", count);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(1);
  }
  delay(2000);
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);
  i2cScanner(Wire);
  dps368_i2c_init();
}

void loop() {

  dps368_i2c_update(doc);
  serializeJson(doc, Serial);
  Serial.print("\n");
  doc.clear();
  vTaskDelay(INTERVAL / portTICK_RATE_MS);
}
