void SSD1306_Init(void) {
  u8g2.begin();
  u8g2.setContrast(0x00);
  u8g2.clearBuffer();
  u8g2.setFontMode(1); // Transparent
  u8g2.setFontDirection(0);
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.drawStr(2, 18, "APRS");
  u8g2.sendBuffer();
  u8g2.drawFrame(2, 25, 60, 6);

  for (int i = 0; i < 0xFF; i++) {
    u8g2.setContrast(i);
    u8g2.drawBox(3, 26, (uint8_t)(0.231 * i), 5);
    u8g2.sendBuffer();
  }

  for (int i = 0; i < 0xFF; i++) {
    u8g2.setContrast(0xFF - i);
    delay(3);
  }
}

void SSD1306_Sleep(void) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
  u8g2.drawStr(12, 8, "Bye, 73!");
  u8g2.sendBuffer();
  delay(3000);
  u8g2.sleepOn();
}

void GPS_Init(void) {
  gpsPort.begin(GPS_BAUD_RATE);
  pinMode(GPS_LEVEL_SHIFTER_EN, OUTPUT);
  digitalWrite(GPS_LEVEL_SHIFTER_EN, HIGH);
  pinMode(GPS_RST_PIN, GPIO_PULLUP);
  digitalWrite(GPS_RST_PIN, LOW);
  delay(200);
  digitalWrite(GPS_RST_PIN, HIGH);
  delay(500);

  sendToGPS("@GSTP");
  sendToGPS("@BSSL", "0x2EF");
  sendToGPS("@GSOP", "1 1000 0");
  sendToGPS("@GNS", "0x03");

  sendToGPS("@GSR");
}

void GPS_Cold(void) {
  sendToGPS("@GCD");
}

void GPS_Sleep(void) {
  gpsPort.flush();
  sendToGPS("@GSTP");
  sendToGPS("@SLP", "2");
  gpsPort.end();
}

void boardSleep(void) {
  GPS_Sleep();
  LoRa_Sleep();
  SSD1306_Sleep();
  pinMode(_1_8V_PWR_ON_PIN, OUTPUT);
  digitalWrite(_1_8V_PWR_ON_PIN, LOW);
  Serial.end();
  SPI.end();
  Wire.end();

  HAL_ADC_Stop(&hadc);
  HAL_ADC_DeInit(&hadc);

  pinMode(IIC_SDA_PIN, OUTPUT);
  pinMode(IIC_SCL_PIN, OUTPUT);
  pinMode(_1_8V_PWR_ON_PIN, OUTPUT);
  pinMode(GPS_PWR_ON_PIN, OUTPUT);
  pinMode(BAT_VOLT_PIN, INPUT_ANALOG);
  pinMode(LORA_MISO_PIN, INPUT_ANALOG);
  pinMode(LORA_MOSI_PIN, INPUT_ANALOG);
  pinMode(LORA_SCK_PIN, INPUT_ANALOG);
  pinMode(LORA_NSS_PIN, INPUT_ANALOG);
  pinMode(LORA_DIO0_PIN, INPUT_ANALOG);
  pinMode(LORA_RST_PIN, INPUT_ANALOG);
  pinMode(GPS_RST_PIN, INPUT_ANALOG);
  pinMode(GPS_RX_PIN, INPUT_ANALOG);
  pinMode(GPS_TX_PIN, INPUT_ANALOG);
  pinMode(PB8, INPUT_ANALOG);
  pinMode(PA11, INPUT_ANALOG);
  pinMode(PA12, INPUT_ANALOG);
  pinMode(GPS_LEVEL_SHIFTER_EN, INPUT_ANALOG);
  pinMode(CONVERSION_PWR_ON_PIN, OUTPUT);

  digitalWrite(IIC_SDA_PIN, HIGH);
  digitalWrite(IIC_SCL_PIN, HIGH);
  digitalWrite(_1_8V_PWR_ON_PIN, LOW);
  digitalWrite(GPS_PWR_ON_PIN, LOW);
  digitalWrite(CONVERSION_PWR_ON_PIN, LOW);

  LL_GPIO_LockPin(GPIOB, LL_GPIO_PIN_6 | LL_GPIO_PIN_7);

  // LowPower.deepSleep();
  LowPower.shutdown();

  boardInit();
}

void boardInit(void) {
  pinMode(_1_8V_PWR_ON_PIN, OUTPUT);
  pinMode(GPS_PWR_ON_PIN, OUTPUT);
  pinMode(CONVERSION_PWR_ON_PIN, OUTPUT);

  digitalWrite(_1_8V_PWR_ON_PIN, HIGH);
  digitalWrite(GPS_PWR_ON_PIN, HIGH);
  digitalWrite(CONVERSION_PWR_ON_PIN, HIGH);

  pinMode(OLED_RESET_PIN, OUTPUT);
  digitalWrite(OLED_RESET_PIN, HIGH);

  delay(2000);
  Serial.begin(115200);

  Wire.setSCL(IIC_SCL_PIN);
  Wire.setSDA(IIC_SDA_PIN);
  Wire.begin();
  rtc.begin();
  LoRa_Init();
  GPS_Init();
  MX_ADC_Init();
  SSD1306_Init();

  pinMode(BAT_VOLT_PIN, INPUT_ANALOG);
  pinMode(TTP223_VDD_PIN, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  digitalWrite(TTP223_VDD_PIN, HIGH);
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
}