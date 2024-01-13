int getTouchTime(void) {
  int time = millis();

  if (digitalRead(TOUCH_PIN)) {
    while (digitalRead(TOUCH_PIN)) {
      if ((int)millis() - time > 3000)
        break;
    }

    return ((int)millis() - time);
  }

  return 0;
}
