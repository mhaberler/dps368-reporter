#include <Adafruit_INA219.h>
#include <Adafruit_LPS2X.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <Dps368.h>
#include <SPI.h>
#include <Wire.h>

#define BUTTON 3
#define LED 2
#define NUMPIXELS 1

#define I2C_SDA 4
#define I2C_SCL 5

Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);
TaskHandle_t TaskHandle_buttonRead;
TaskHandle_t TaskHandle_Led;

int buttonLastState = HIGH;
int buttonCurrentState;
int buttonCount = 1;

int high = 255;
int mid = 128;
int low = 0;

Dps368 Dps368PressureSensor = Dps368();
Adafruit_LPS22 lps;
Adafruit_Sensor *lps_temp, *lps_pressure;

DynamicJsonDocument doc(1024);

void buttonRead(void *pvParameters) {
  while (true) {
    buttonCurrentState = digitalRead(BUTTON);
    if (buttonLastState == LOW && buttonCurrentState == HIGH) {
      Serial.println("Button Pressed!");
      buttonCount++;
      if (buttonCount > 6) {
        buttonCount = 1;
      }
      doc["count"] = buttonCount;
      serializeJson(doc, Serial);

      float temperature;
      float pressure;
      uint8_t oversampling = 7;
      int16_t ret;

      if (!Dps368PressureSensor.measureTempOnce(temperature, oversampling)) {
        doc["temp"] = temperature;
      }
      if (!Dps368PressureSensor.measurePressureOnce(pressure, oversampling)) {
        doc["pressure"] = pressure;
      }
    }
    buttonLastState = buttonCurrentState;
    vTaskDelay(10 / portTICK_RATE_MS);
  }
}

void changeLedState(void *pvParameters) {
  while (true) {
    pixels.clear();
    switch (buttonCount) {
    case 1:
      pixels.setPixelColor(0, pixels.Color(high, low, low));
      break;
    case 2:
      pixels.setPixelColor(0, pixels.Color(high, mid, mid));
      break;
    case 3:
      pixels.setPixelColor(0, pixels.Color(low, high, low));
      break;
    case 4:
      pixels.setPixelColor(0, pixels.Color(mid, high, mid));
      break;
    case 5:
      pixels.setPixelColor(0, pixels.Color(low, low, high));
      break;
    case 6:
      pixels.setPixelColor(0, pixels.Color(mid, mid, high));
      break;
    default:
      break;
    }
    pixels.show();
    vTaskDelay(100 / portTICK_RATE_MS);
  }
}

void i2cScanner(TwoWire *wire) {
  Serial.printf("I2C scanner. Scanning bus..\n");
  byte count = 0;

  for (byte i = 8; i < 125; i++) {
    wire->beginTransmission(i);       // Begin I2C transmission Address (i)
    if (wire->endTransmission() == 0) // Receive 0 = success (ACK response)
    {
      Serial.printf("found i2c device at 0x%x\n", i);
      count++;
    }
  }
  Serial.printf("Found %d devices\n", count);
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  i2cScanner(&Wire);

  // Dps368PressureSensor.begin(Wire);

  if (!lps.begin_I2C(LPS2X_I2CADDR_DEFAULT, &Wire, 0)) {
    Serial.println("Failed to find LPS2X chip");
    // while (1) {
    //   delay(10);
    // }
  }
#define MOSI 6       // la: orange
#define SCK 7        // la: rot
#define MISO 8       // la: braun
#define DPS368_CS 10 // la: sw

  SPI.begin(SCK, MISO, MOSI);
  Dps368PressureSensor.begin(SPI, DPS368_CS);

  pixels.begin();
  pinMode(BUTTON, INPUT_PULLUP);

  xTaskCreate(buttonRead, "buttonRead", 2048 * 1, nullptr, 128 * 10,
              &TaskHandle_buttonRead);
  xTaskCreate(changeLedState, "changeLedState", 2048 * 1, nullptr, 128 * 1,
              &TaskHandle_Led);
}

void loop() {}
