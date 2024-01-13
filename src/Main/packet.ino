String generateBasePacket(String callsign, String tocall, String path) {
  String packet = callsign + ">" + tocall;
  if (path != "") {
    packet += "," + path;
  }
  return packet;
}

String generateGPSBeaconPacket(String callsign, String tocall, String path, String overlay, String gps) {
  return generateBasePacket(callsign,tocall,path) + ":!" + overlay + gps;
}