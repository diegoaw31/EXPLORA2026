#include <Arduino.h>
#include "Clock.h"
#include "SCD30.h"
#include "Adafruit_MPL3115A2.h"
#include <Adafruit_MAX31865.h>
#include <SPI.h>
#include <SD.h>

#define RREF      4300.0
#define RNOMINAL  1000.0
#define MAX3185_CS 5
#define SDMODULE_CS 4
#define OZONE_ADC 36

#define FOLDER "/EXPLORA26"
#define FILE "/EXPLORA26/data.csv"

#define PERIOD 5000 // 5 seconds in milliseconds

Adafruit_MPL3115A2 baro;
Adafruit_MAX31865 thermo = Adafruit_MAX31865(MAX3185_CS);

void initSensors() {
  Clock::init();
  scd30.initialize();
  baro.begin();
  baro.setSeaPressure(1013.26);
  thermo.begin(MAX31865_2WIRE);  // set to 2WIRE or 4WIRE as necessary
  pinMode(OZONE_ADC, INPUT);
}

void writeToSD(std::string content, std::string path);

void initSD() {
  if (SD.begin(SDMODULE_CS)) {
    Serial.print("SD card initialized\b");
  } // initialize the SD card
  if (!(SD.exists(FOLDER))) {
    SD.mkdir(FOLDER);
    delay(100);
    writeToSD("time,rtdtemp,rtdfault,scd30co2,scd30temp,scd30hum,baropres,baroalt,barotemp,rawozone,ozone", FILE);
    // Serial.print("Folders are made!\n");
  }
}

void writeToSD(std::string content, std::string path) {
  File file = SD.open(path.c_str(), FILE_APPEND, true);
  if (file) {
    file.print(content.c_str());
    file.close();
    Serial.print("Wrote to file!\n");
  } else {
    Serial.print("File does not exist!\n");
  }
}

// List of available data: time, rtdtemp, rtdfault, scd30co2, scd30temp, scd30hum, baropres, baroalt, barotemp, rawozone, ozone
void DumpData(String time, float rtdtemp, uint8_t rtdfault, float scd30co2, float scd30temp, float scd30hum, float baropres, float baroalt, float barotemp, float rawozone, float ozone) {
  String content = time + "," + String(rtdtemp) + "," + String(rtdfault) + "," + String(scd30co2) + "," + String(scd30temp) + "," + String(scd30hum) + "," + String(baropres) + "," + String(baroalt) + "," + String(barotemp) + "," + String(rawozone) + "," + String(ozone) + "\n";
  writeToSD(content.c_str(), FILE);
}

void testSensors() {
  Serial.println("-----------------");
  Clock::test();
  writeToSD("Testing!", "/EXPLORA26/data/testing.txt");
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
  pinMode(4, OUTPUT);
  initSensors();
  initSD();
}

int timer = 0;
String timeString;
char timeChar[23];
float scd30co2 = 0;
float scd30temp = 0;
float scd30hum = 0;
float rtdtemp = 0;
uint8_t rtdfault = 0;
float result[3] = {0};
float baropres = 0;
float baroalt = 0;
float barotemp = 0;
float rawozone = 0;
float ozone = 0;

char buf[10];
String imageName;

void loop() {
  if(millis() - timer >= PERIOD) {
    timer = millis();

    timeString = Clock::readClock();
    snprintf(buf, sizeof(buf), "IMG%02d%02d%02d", Clock::hour(), Clock::minute(), Clock::second());
    imageName = String(buf);
    Serial.println(imageName);

    rtdtemp = thermo.temperature(RNOMINAL, RREF);
    rtdfault = thermo.readFault();
    thermo.clearFault();

    result[0] = 0;
    result[1] = 0;
    result[2] = 0;
    if (scd30.isAvailable()) {
        scd30.getCarbonDioxideConcentration(result);
        scd30co2 = result[0];
        scd30temp = result[1];
        scd30hum = result[2];
    }

    baropres = baro.getPressure();
    baroalt = baro.getAltitude();
    barotemp = baro.getTemperature();

    rawozone = analogRead(OZONE_ADC);
    ozone = rawozone * (3.3 / 4095); // Convert ADC value to voltage

    timeString.toCharArray(timeChar, sizeof(timeChar));
    DumpData(timeChar, rtdtemp, rtdfault, scd30co2, scd30temp, scd30hum, baropres, baroalt, barotemp, rawozone, ozone);
  }

  // testSensors();

}