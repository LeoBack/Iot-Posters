/*
   Metodos AP
   Guarda configuracion.
   OK - 20/11/13
*/
void writeWifi(String ssid, String password) {
  libWiFi wifi;
  Serial.print("Configuracion Wifi: ");

  if (DEBUG) {
    Serial.print("SSID: "); Serial.println(ssid);
    Serial.print("PWD: "); Serial.println(password);
  }

  if (wifi.save(ssid, password)) {
    Serial.println("Save successful");
    Serial.println("Rebooting...");
    ESP.restart();
  }
  else
    Serial.println("Save failed");
}

//void setGPIO(String id, bool state)
//{
//  Serial.print("Set GPIO ");
//  Serial.print(id);
//  Serial.print(": ");
//  Serial.println(state);
//}
//
//void setPWM(String id, int pwm)
//{
//  Serial.print("Set PWM ");
//  Serial.print(id);
//  Serial.print(": ");
//  Serial.println(pwm);
//}
//
//void doAction(String actionId)
//{
//  Serial.print("Doing action: ");
//  Serial.println(actionId);
//}
