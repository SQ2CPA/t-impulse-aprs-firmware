#include "STM32LowPower.h"
#include <ICM_20948.h>
#include <LoRa.h>
#include <STM32RTC.h>
#include <TinyGPS++.h>
#include <U8g2lib.h>
#include <EEPROM.h>

#define VERSION 0x01

// GPS
#define GPS_RST_PIN               PB2
#define GPS_RX_PIN                PC11
#define GPS_TX_PIN                PC10
#define GPS_LEVEL_SHIFTER_EN      PC6
#define GPS_BAUD_RATE             115200

// SSD1306
#define IIC_SDA_PIN               PB7
#define IIC_SCL_PIN               PB6
#define OLED_RESET_PIN            PA8

// TOUCH
#define TTP223_VDD_PIN            PA2
#define TOUCH_PIN                 PA0
#define GPS_PWR_ON_PIN            PA3
#define _1_8V_PWR_ON_PIN          PB0
#define CONVERSION_PWR_ON_PIN     PB5
#define BAT_VOLT_PIN              PC4
#define MOTOR_PIN                 PA5

// IIC_ADDRESS
#define SSD1306_ADDR              0x3c
#define ICM20948_ADDR             0x69
#define ICM20648_ADDR             0x68

// CONFIGURATION
#define CONF_BEGIN             0x01
#define CONF_INIT             0x10
#define CONF_READ             0x11
#define CONF_WRITE             0x12
#define CONF_SUBMIT             0xFF

struct ConfigData {
  char callsign[12]; // 11 chars
  char path[19]; // 18 chars
  char overlay; // 1 char
  char symbol; // 1 char
  char comment[25]; // 24 chars
  uint16_t interval; // 60-900
  uint8_t vibration; // bool
  uint8_t batteryInfo; // bool
};

ConfigData config;

int x = sizeof(ConfigData);

static bool isConfigured = EEPROM.read(0) == 0x10;

void menuGPS(void);
void menuAPRS(void);
void menuTime(void);

// OLED
U8G2_SSD1306_64X32_1F_F_HW_I2C u8g2(U8G2_R0, OLED_RESET_PIN, IIC_SCL_PIN, IIC_SDA_PIN);
bool show = true;

// GPS
TinyGPSPlus gps;
HardwareSerial gpsPort(GPS_RX_PIN, GPS_TX_PIN);
double gpsLatitude = 0, gpsLongitude = 0;
int gpsSatellites = 0;
static uint32_t gpsLastLocation = 0;
bool isFirstFix = true;

// RTC
STM32RTC &rtc = STM32RTC::getInstance();

// Menu
static uint8_t menuFunctionsCount = 3;
typedef void (*funcCallBackTypedef)(void);
funcCallBackTypedef menuFunctions[] = {menuAPRS, menuTime, menuGPS};
static uint8_t menuIndex = 0;
static uint32_t menuMillis = 0;

// Voltage
ADC_HandleTypeDef hadc;
float voltage = 0;
uint32_t voltageTotal = 0, voltageMillis = 0, voltageGetMillis = 0;
uint8_t voltageCount = 0;

void repetitionsIncrease(void) {}

void setup() {
  LowPower.begin();
  LowPower.attachInterruptWakeup(TOUCH_PIN, repetitionsIncrease, RISING, DEEP_SLEEP_MODE);

  boardInit();

  if (isConfigured) {
    readConfigFromEEPROM();

    Serial.println(config.callsign);
    Serial.println(config.path);
    Serial.println(config.overlay);
    Serial.println(config.symbol);
    Serial.println(config.comment);
    Serial.println(config.interval);
    Serial.println(config.vibration);
    Serial.println(config.batteryInfo);

    if (config.interval > 900) {
      clearEEPROM();

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
      u8g2.drawStr(0, 8, "BAD CONFIG");
      u8g2.sendBuffer();

      delay(5000);

      NVIC_SystemReset();
    }

    LoRa.setFrequency(434855000);
    LoRa.setCodingRate4(7);
    LoRa.setSpreadingFactor(9);
    LoRa.setSignalBandwidth(125000);
  }
}

static uint32_t packetMillis = 0, lastPacket = 0;
void sendPacket() {
  if ((millis() - packetMillis) > config.interval * 1000) {
    if (millis() - gpsLastLocation < 30 * 1000) {
      setRadioTX();

      String newPacket = generateGPSBeaconPacket(String(config.callsign), "APLRT1", String(config.path), String(config.overlay), encondeGPS(gps.location.lat(),gps.location.lng(), gps.course.deg(), gps.speed.knots(), String(config.symbol), false, gps.altitude.feet(), false, "GPS"));

      newPacket += String(config.comment);
      
      if (config.batteryInfo == 1) {
        newPacket += " Batt=" + String(voltage) + "V";
      }

      LoRa.beginPacket();
      LoRa.write('<');
      LoRa.write(0xFF);
      LoRa.write(0x01);
      LoRa.write((const uint8_t *)newPacket.c_str(), newPacket.length());
      LoRa.endPacket();

      if (config.vibration == 1) {
        digitalWrite(MOTOR_PIN, HIGH);
        delay(100);
        digitalWrite(MOTOR_PIN, LOW);
      }

      setRadioRX();

      lastPacket = millis();
    }

    packetMillis = millis();
  }
}

void loop() {
  while (gpsPort.available() > 0) {
    gps.encode(gpsPort.read());
  }

  int touchTime = getTouchTime();

  if (touchTime > 3000) {
    if (isConfigured && menuIndex == 2 && !show) {
      digitalWrite(MOTOR_PIN, HIGH);
      delay(200);
      digitalWrite(MOTOR_PIN, LOW);

      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
      u8g2.drawStr(0, 8, "GPS");
      u8g2.drawStr(0, 16, "COLD START");
      u8g2.sendBuffer();

      delay(2000);

      GPS_Cold();
    } else {
      boardSleep();
    }
  }

  if (isConfigured) {
    if (touchTime > 1000) {
      digitalWrite(MOTOR_PIN, HIGH);
      delay(100);
      digitalWrite(MOTOR_PIN, LOW);

      show = !show;

      if (!show) {
        u8g2.clearBuffer();
        u8g2.sendBuffer();
      }
    } else if (touchTime > 100 && show) {
      digitalWrite(MOTOR_PIN, HIGH);
      delay(200);
      digitalWrite(MOTOR_PIN, LOW);

      menuIndex++;
      menuIndex %= menuFunctionsCount;
    }

    if (gps.date.isUpdated()) {
      rtc.setDate(calcFromGPSToRTC(gps.date.year(), gps.date.month(), gps.date.day()), gps.date.day(), gps.date.month(), gps.date.year() - 2000);
    }

    if (gps.time.isUpdated()) {
      rtc.setTime(gps.time.hour(), gps.time.minute(), gps.time.second());
    }

    if (gps.location.isUpdated()) {
      gpsLatitude = gps.location.lat();
      gpsLongitude = gps.location.lng();
      gpsLastLocation = millis();

      if (isFirstFix) {
        packetMillis = 0;
        isFirstFix = false;
      } else if (millis() - lastPacket > config.interval * 1000) {
        packetMillis = 0;
      }
    }

    if (gps.satellites.isUpdated()) {
      gpsSatellites = gps.satellites.value();
    }

    readVoltage();

    sendPacket();

    if (show) {
      if (menuFunctions[menuIndex]) {
        menuFunctions[menuIndex]();
      }
    }
  } else {
    menuConfigure();
  }

  if (Serial.available() > 0) {
    bool flush = false;

    if (Serial.read() == CONF_BEGIN) {
      uint8_t cmd = Serial.read();

      if (cmd == CONF_INIT) {
        Serial.write(CONF_BEGIN);
        Serial.write(CONF_INIT);
        Serial.write(isConfigured ? 0x02 : 0x01);
        Serial.write(VERSION);
        Serial.flush();
      } else if (cmd == CONF_READ) {
        Serial.write(CONF_BEGIN);
        Serial.write(CONF_READ);
        Serial.flush();

        delay(150);

        sendConfig();
        Serial.flush();
      } else if (cmd == CONF_WRITE) {
        Serial.write(CONF_BEGIN);
        Serial.write(CONF_WRITE);
        Serial.flush();

        delay(150);

        while (Serial.available() < sizeof(ConfigData));
        Serial.readBytes((char*)&config, sizeof(ConfigData));

        saveConfigToEEPROM();

        Serial.write(0xFF);
        Serial.flush();
      } else if (cmd == CONF_SUBMIT) {
        EEPROM.write(0, 0x10);

        NVIC_SystemReset();
      } else {
        flush = true;
      }
    } else {
      flush = true;
    }

    if (flush) {
      while (Serial.available() > 0) {
        Serial.read();
      }
    }
  }

  u8g2.setContrast(0xFF);
}
