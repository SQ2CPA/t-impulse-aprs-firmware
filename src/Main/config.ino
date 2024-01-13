void readConfigFromEEPROM() {
  EEPROM.get(1, config);
}

void saveConfigToEEPROM() {
  EEPROM.put(1, config);
}

void clearEEPROM() {
  for (int i=0; i<512; i++) {
    EEPROM.write(i, 0);
  }
}

void sendConfig() {
  Serial.write((const uint8_t*)&config, sizeof(ConfigData));
}