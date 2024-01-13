void readVoltage() {
  HAL_ADC_Start(&hadc);
 
  if (millis() - voltageMillis > 5 && voltageCount < 20) {
    voltageCount++;

    voltageTotal += HAL_ADC_GetValue(&hadc);
    voltageMillis = millis();
  }

  if (millis() - voltageGetMillis > 500) {
    voltageTotal /= voltageCount;

    voltage = (voltageTotal * 3.3 * 2) / 4096;

    voltageGetMillis = millis();
    voltageCount = 0;
    voltageTotal = 0;
  }
}

static void MX_ADC_Init(void) {
  ADC_ChannelConfTypeDef sConfig = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  hadc.Instance = ADC1;
  hadc.Init.OversamplingMode = DISABLE;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.SamplingTime = ADC_SAMPLETIME_39CYCLES_5;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerFrequencyMode = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  if (HAL_ADC_Init(&hadc) != HAL_OK) {
    Error_Handler();
  }
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
}
