uint8_t calcFromGPSToRTC(uint16_t _year, uint8_t _mon, uint8_t _day) {
  uint8_t y, c, m, d;
  int16_t w;
  if (_mon >= 3) {
    m = _mon;
    y = _year % 100;
    c = _year / 100;
    d = _day;
  } else {
    m = _mon + 12;
    y = (_year - 1) % 100;
    c = (_year - 1) / 100;
    d = _day;
  }

  w = y + y / 4 + c / 4 - 2 * c + ((uint16_t)26 * (m + 1)) / 10 + d - 1;
  if (w == 0) {
    w = 7;
  } else if (w < 0) {
    w = 7 - (-w) % 7;
  } else {
    w = w % 7;
  }
  return w;
}

int calculateBatteryPercentage() {
    const float fullyChargedVoltage = 4.20f;
    const float dischargedVoltage = 3.3f;

    if (voltage < dischargedVoltage) {
        voltage = dischargedVoltage;
    } else if (voltage > fullyChargedVoltage) {
        voltage = fullyChargedVoltage;
    }

    float percentage = ((voltage - dischargedVoltage) / (fullyChargedVoltage - dischargedVoltage)) * 100.0f;

    int roundedPercentage = static_cast<int>(percentage + 0.5f);

    return roundedPercentage;
}