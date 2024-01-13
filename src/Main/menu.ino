void menuConfigure(void) {
  if ((millis() - menuMillis) > 1000) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
    u8g2.setCursor(0, 8);
    u8g2.print("PLEASE");
    u8g2.setCursor(0, 24);
    u8g2.print("CONFIGURE");
    u8g2.sendBuffer();
  
    menuMillis = millis();
  }
}


void menuAPRS(void) {
  if ((millis() - menuMillis) > 500) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
    u8g2.setCursor(0, 8);
    u8g2.print(config.callsign);
    u8g2.print(" ");

    int percentage = calculateBatteryPercentage();

    u8g2.setCursor(0, 24);
    u8g2.print(voltage);
    u8g2.print("V ");
    u8g2.print(percentage);
    u8g2.print("%");

    u8g2.sendBuffer();
  
    menuMillis = millis();
  }
}

void menuTime(void) {
  if ((millis() - menuMillis) > 100) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
    u8g2.setCursor(0, 8);
    u8g2.printf("  %02d/%02d/%02d ", rtc.getDay(), rtc.getMonth(), rtc.getYear());
    u8g2.setCursor(0, 24);
    u8g2.printf("   %02d:%02d:%02d", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
    u8g2.sendBuffer();

    menuMillis = millis();
  }
}

void menuGPS(void) {
  if (millis() - menuMillis > 500) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_IPAandRUSLCD_tr);
    u8g2.drawStr(0, 8, "GPS loc.");

    u8g2.setCursor(0, 16);
    u8g2.print(F("Satt. ="));
    u8g2.setCursor(48, 16);
    u8g2.print(gpsSatellites);

    u8g2.setCursor(0, 24);
    u8g2.print(F("Lat="));
    u8g2.print(gpsLatitude);

    u8g2.setCursor(0, 32);
    u8g2.print(F("Lng="));
    u8g2.print(gpsLongitude);
    u8g2.sendBuffer();

    menuMillis = millis();
  }
}