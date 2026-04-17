#include <Arduino.h>
#include "Clock.h"
#include "SCD30.h"
#include "Adafruit_MPL3115A2.h"
#include <Adafruit_MAX31865.h>

#define RREF      4300.0
#define RNOMINAL  1000.0
#define MAX3185_CS 4

Adafruit_MPL3115A2 baro;
Adafruit_MAX31865 thermo = Adafruit_MAX31865(MAX3185_CS);
void initSensors() {
  Clock::init();
  scd30.initialize();
  baro.begin();
  baro.setSeaPressure(1013.26);
  thermo.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary
}
void testSensors() {
  Serial.println("-----------------");
  Clock::test();
  float result[3] = {0};
  if (scd30.isAvailable()) {
        scd30.getCarbonDioxideConcentration(result);
        Serial.println("-----------------");
        Serial.print("Carbon Dioxide Concentration is: ");
        Serial.print(result[0]);
        Serial.println(" ppm");
        Serial.print("Temperature = ");
        Serial.print(result[1]);
        Serial.println(" ℃");
        Serial.print("Humidity = ");
        Serial.print(result[2]);
        Serial.println(" %");
    }
  Serial.print("\n");
  float pressure = baro.getPressure();
  float altitude = baro.getAltitude();
  float temperature = baro.getTemperature();

  Serial.println("-----------------");
  Serial.print("pressure = "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("altitude = "); Serial.print(altitude); Serial.println(" m");
  Serial.print("temperature = "); Serial.print(temperature); Serial.println(" C");

  uint16_t rtd = thermo.readRTD();

  Serial.println("-----------------");
  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

    uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
}

void setup() {
  Serial.begin(115200);
  initSensors();
}
void loop() {
  testSensors();
  Serial.println("IMG180230");

  delay(5000);
}