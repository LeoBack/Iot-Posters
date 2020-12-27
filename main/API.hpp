/*
   Metodos AP
   Guarda configuracion.
   VER - 20/11/13
*/
void writeWifi(String ssid, String password) {
  libWiFi wifi;
  Serial.print("Configuracion Wifi: ");

  if (config.debug) {
    Serial.printf("SSID: %s\n", ssid.c_str());
    Serial.printf("PWD: %s\n", password.c_str());
  }
  
  config.wifi.ssid = ssid;
  config.wifi.password = password;

  if (!save_config())
    Serial.println("Save failed");
  else {
    Serial.println("Save successful");
    Serial.println("Rebooting...");
    ESP.restart();
  }
}
