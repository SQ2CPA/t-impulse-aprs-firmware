// LORA :Note here that the SX1276 may be the 0X13 version. Note when using other libraries
#define LORA_RST_PIN              PB10
#define LORA_DIO0_PIN             PB11
#define LORA_DIO1_PIN             PC13
#define LORA_DIO2_PIN             PB9
#define LORA_DIO3_PIN             PB4
#define LORA_DIO4_PIN             PB3
#define LORA_DIO5_PIN             PA15
#define LORA_MOSI_PIN             PB15
#define LORA_MISO_PIN             PB14
#define LORA_SCK_PIN              PB13
#define LORA_NSS_PIN              PB12
#define RADIO_ANT_SWITCH_RXTX_PIN PA1
#define LORA_TCXO_OE_PIN          PD7
#define LORA_OSC_SEL_PIN          PC1

void setRadioDirection(bool rx) {
  digitalWrite(RADIO_ANT_SWITCH_RXTX_PIN, rx ? HIGH : LOW);
}

void setRadioTX() {
  setRadioDirection(false);
}

void setRadioRX() {
  setRadioDirection(true);
}

void LoRa_Sleep(void) {
  LoRa.end();
  GPIO_InitTypeDef GPIO_Struct;
  __HAL_RCC_GPIOD_CLK_ENABLE();
  // for LoRa sx1276 TCXO OE Pin
  GPIO_Struct.Pin = GPIO_PIN_7;
  GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Struct.Pull = GPIO_NOPULL;
  GPIO_Struct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_Struct);
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
}

#define REG_LR_TCXO              0x4B
#define RFLR_TCXO_TCXOINPUT_MASK 0xEF
#define RFLR_TCXO_TCXOINPUT_ON   0x10
#define RFLR_TCXO_TCXOINPUT_OFF  0x00 // Default

bool LoRa_Init(void) {
  SPI.setMISO(LORA_MISO_PIN);
  SPI.setMOSI(LORA_MOSI_PIN);
  SPI.setSCLK(LORA_SCK_PIN);
  SPI.begin();
  LoRa.setSPI(SPI);
  LoRa.setPins(LORA_NSS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(433000000)) {
    return false;
  }

  LoRa.enableCrc();
  LoRa.setTxPower(20);

  pinMode(RADIO_ANT_SWITCH_RXTX_PIN, OUTPUT);

  setRadioRX();

  pinMode(LORA_OSC_SEL_PIN, INPUT);
  delay(10);

  int pin_state = digitalRead(LORA_OSC_SEL_PIN);
  if (pin_state == false) {
    uint8_t tcxo = LoRa.readRegister(REG_LR_TCXO) & RFLR_TCXO_TCXOINPUT_MASK;
    LoRa.writeRegister(REG_LR_TCXO, tcxo | RFLR_TCXO_TCXOINPUT_OFF);

    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Struct;

    // Use for LoRa sx1276 TCXO OE Pin
    GPIO_Struct.Pin = GPIO_PIN_7;
    GPIO_Struct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Struct.Pull = GPIO_NOPULL;
    GPIO_Struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_Struct);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
    delay(10);
  }
  return true;
}