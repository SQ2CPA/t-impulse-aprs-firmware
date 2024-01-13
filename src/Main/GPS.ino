bool sendToGPS(String cmd, String arg = "") {
  gpsPort.flush();

  if (arg != "") {
    gpsPort.print(cmd);
    gpsPort.print(" ");
    gpsPort.println(arg);
  } else {
    gpsPort.println(cmd);
  }

  uint32_t smap = millis() + 500;
  while (millis() < smap) {
    if (gpsPort.available() > 0) {
      if (gpsPort.find("Done")) {
        return true;
      }
    }
  }

  return false;
}

char *ax25_base91enc(char *s, uint8_t n, uint32_t v) {
  for(s += n, *s = '\0'; n; n--) {
    *(--s) = v % 91 + 33;
    v /= 91;
  }
  return(s);
}

String encondeGPS(float latitude, float longitude, float course, float speed, String symbol, bool sendAltitude, int altitude, bool sendStandingUpdate, String packetType) {
  String encodedData;
  uint32_t aprs_lat, aprs_lon;
  aprs_lat = 900000000 - latitude * 10000000;
  aprs_lat = aprs_lat / 26 - aprs_lat / 2710 + aprs_lat / 15384615;
  aprs_lon = 900000000 + longitude * 10000000 / 2;
  aprs_lon = aprs_lon / 26 - aprs_lon / 2710 + aprs_lon / 15384615;

  String Ns, Ew, helper;
  if(latitude < 0) { Ns = "S"; } else { Ns = "N"; }
  if(latitude < 0) { latitude= -latitude; }

  if(longitude < 0) { Ew = "W"; } else { Ew = "E"; }
  if(longitude < 0) { longitude= -longitude; }

  char helper_base91[] = {"0000\0"};
  int i;
  ax25_base91enc(helper_base91, 4, aprs_lat);
  for (i=0; i<4; i++) {
    encodedData += helper_base91[i];
  }
  ax25_base91enc(helper_base91, 4, aprs_lon);
  for (i=0; i<4; i++) {
    encodedData += helper_base91[i];
  }
  if (packetType=="Wx") {
    encodedData += "_";
  } else {
    encodedData += symbol;
  }

  if (sendAltitude) {           
    int Alt1, Alt2;
    if(altitude>0) {
      double ALT=log(altitude)/log(1.002);
      Alt1= int(ALT/91);
      Alt2=(int)ALT%91;
    } else {
      Alt1=0;
      Alt2=0;
    }
    if (sendStandingUpdate) {
      encodedData += " ";
    } else {
      encodedData +=char(Alt1+33);
    }
    encodedData +=char(Alt2+33);
    encodedData +=char(0x30+33);
  } else {                      
    ax25_base91enc(helper_base91, 1, (uint32_t) course/4 );
    if (sendStandingUpdate) {
      encodedData += " ";
    } else {
      encodedData += helper_base91[0];
    }
    ax25_base91enc(helper_base91, 1, (uint32_t) (log1p(speed)/0.07696));
    encodedData += helper_base91[0];
    encodedData += "\x47";
  }
  return encodedData;
}