/*
   2020/09/22 - Creacion de la libreria.


*/
class libWiFi {
  private:
    int pSTATUS = 0;
  public:
    String connectSsid;
    String connectPassword;
    void begin(int pin_STATUS);
    String scanWiFi();
    boolean load();
    boolean save(String ssid, String password);
    boolean wifi_STA(bool useStaticIP = false);
    void wifi_AP(bool useStaticIP = false);
    boolean connectWiFi();
    void help();
    void menu(String command, String parameters);
};

void libWiFi::begin(int pin_STATUS) {
  pSTATUS = pin_STATUS;
  pinMode(pSTATUS, OUTPUT);
}

String libWiFi::scanWiFi() {
  int scanWiFiSize = WiFi.scanNetworks();
  String mensaje = "Escaneo terminado:\n";
  String response = "";

  if (scanWiFiSize != 0) {
    mensaje += scanWiFiSize + " redes encontradas.\n";
    StaticJsonDocument<400> doc;
    for (int i = 0; i < scanWiFiSize; ++i) {
      String item = String(i + 1) + ", " + WiFi.SSID(i) + ", " + WiFi.RSSI(i) + ", " + WiFi.channel(i) + ", " + WiFi.encryptionType(i);
      doc.add(item);
#if DEBUG == 1
      String line = String(i + 1) + ": " + WiFi.SSID(i) + ", (" + WiFi.RSSI(i) + "), Ch: " + WiFi.channel(i) + ", Enc: " + WiFi.encryptionType(i) + "\r\n";
      //WiFi.encryptionType 5:WEP 2:WPA/PSK 4:WPA2/PSK 7:open network 8:WPA/WPA2/PSK
      mensaje += line;
#endif
    }
    serializeJson(doc, response);
  }
  else {
    mensaje = "No se encontraron redes\n";
  }
#if DEBUG == 1
  delay(10);
  Serial.println(mensaje);
#endif
  return response;
}

boolean libWiFi::load() {
  File configFile = SPIFFS.open(wifiConfig, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  if (configFile.size() > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println("Error deserializing the configuration file");
    return false;
  }

  const char* ssid = doc["ssid"];
  const char* password = doc["password"];
  libWiFi::connectSsid = ssid;
  libWiFi::connectPassword = password;

#if DEBUG == 1
  Serial.println("Configuracion Wifi leida:");
  Serial.print("SSID: "); Serial.println(libWiFi::connectSsid);
  Serial.print("Password: "); Serial.println(libWiFi::connectPassword);
#endif

  return true;
}

boolean libWiFi::save(String pSsid, String pPassword) {
  pSsid.trim();
  pPassword.trim();
  libWiFi::connectSsid = pSsid;
  libWiFi::connectPassword = pPassword;
  /*
    Serial.println(pSsid);
    Serial.println(libWiFi::connectSsid);
    Serial.println(pPassword);
    Serial.println(libWiFi::connectPassword);
  */
  StaticJsonDocument<100> doc;
  doc["ssid"] = libWiFi::connectSsid;
  doc["password"] = libWiFi::connectPassword;

  File configFile = SPIFFS.open(wifiConfig, "w");
  if (!configFile) {
    Serial.println("Error opening configuration file for writing");
    return false;
  }
  serializeJson(doc, configFile);

#if DEBUG == 1
  Serial.println("Configuracion Wifi guardada:");
  Serial.print("SSID: "); Serial.println(libWiFi::connectSsid);
  Serial.print("Password: "); Serial.println(libWiFi::connectPassword);
#endif

  return true;
}

boolean libWiFi::wifi_STA(bool useStaticIP) {
  WiFi.mode(WIFI_STA);
  Serial.println("SET mode_STA");

  int _reconnect = 0;
  String _ssid = ssid;
  String _password = password;

  if (libWiFi::load()) {
    _ssid = libWiFi::connectSsid;
    _password = libWiFi::connectPassword;
  }

  WiFi.begin(_ssid, _password);

  if (useStaticIP)
    WiFi.config(ip, gateway, subnet);

  Serial.printf("Hostname por defecto: %s\n", WiFi.hostname().c_str());
  //WiFi.hostname(DOMAIN_NAME);


    if (!MDNS.begin(DOMAIN_NAME))
      Serial.println("Error al inicinar mDNS.");
    else {
      Serial.println("Run mDNS");
      // Add service to MDNS-SD
      //MDNS.addService("http", "tcp", 80);
    }
  
  //WL_NO_SHIELD: 255
  //WL_IDLE_STATUS: 0
  //WL_NO_SSID_AVAIL: 1
  //WL_SCAN_COMPLETED: 2
  //WL_CONNECTED: 3
  //WL_CONNECTION_LOST: 5
  //WL_DISCONNECTED: 6

  //  int status = 0;
  boolean isValid = true;
  //  while (isValid)
  //  {
  //    isValid = _reconnect < attemptsToReconnect;
  //    status = WiFi.begin(_ssid, _password);
  //    if (status == WL_CONNECTED)
  //      isValid =  false;
  //    else
  //      _reconnect++;
  //
  //    Serial.print('.');
  //    delay(500);
  //  }

  //    while (status != WL_CONNECTED)
  //    {
  //      status = WiFi.begin(_ssid, _password);
  //      Serial.print(" ."); Serial.println(status);
  //      delay(1000);
  //    }
  //status = WiFi.status();
  //Serial.print("Status:\t"); Serial.println(status);
  
  Serial.println("Initiated STA");
  Serial.printf("SSID: %s\n", WiFi.SSID().c_str());
  Serial.printf("IP address: %d.%d.%d.%d\n", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  Serial.printf("MAC address: %s\n", WiFi.softAPmacAddress().c_str());
  Serial.printf("Hostname: %s\n", WiFi.hostname().c_str());

  return  isValid;
}

void libWiFi::wifi_AP(bool useStaticIP) {
  WiFi.mode(WIFI_AP);
  Serial.println("SET mode_AP");

  while (!WiFi.softAP(ssidAP, password)) {
    Serial.println(".");
    delay(100);
  }

  if (useStaticIP)
    WiFi.softAPConfig(ip, gateway, subnet);

  Serial.println("");
  Serial.print("Initiated AP:\t"); Serial.println(ssidAP);
  Serial.print("IP address:\t"); Serial.println(WiFi.softAPIP());
}

boolean libWiFi::connectWiFi() {
  int count = 10;
  bool isAP = false;

  while (count >= 0) {
    if (!isAP) {
      if (digitalRead(pinPULSADOR) == LOW) {
        delay(80);
        if (digitalRead(pinPULSADOR) == LOW) {
          isAP = true;
          count = 0;
          wifi_AP();
        }
      }
      Serial.print("press AP mode "); Serial.print(count); Serial.println("s remaining time");
      count--;
      digitalWrite(pSTATUS, LOW);
      ESP.wdtFeed();
      delay(500);
      digitalWrite(pSTATUS, HIGH);
      ESP.wdtFeed();
      delay(500);
    }
  }

  if (!isAP)
    wifi_STA();

  return isAP;
}

void libWiFi::help() {
  Serial.println("WiFi - Help");
  Serial.println("set   \t - Establece SSID y Password de una red wifi seleccionada.");
  Serial.println("reload\t - Lee configuracion de los datos de wifi almacenado.");
  Serial.println("scan  \t - Escaner de redes wifi.");
  Serial.println("help  \t - Manual de intrucciones.");
  Serial.println();
}

void libWiFi::menu(String command, String parameters) {
  command.toLowerCase();
  if (command == "set" & parameters.length() > 0) {
    int isSSID = 0;
    String parameter = "";
    for (int nchar = 0; nchar < parameters.length() + 1; nchar++) {
      if (parameters[nchar] != ',' & nchar < parameters.length()) {
        parameter += parameters[nchar];
      }
      else {
        if (isSSID == 0) {
          Serial.print("> SSID= "); Serial.println(parameter);
          libWiFi::connectSsid = parameter;
          isSSID++;
          parameter = "";
        }
        else if (isSSID == 1) {
          Serial.print("> Password= "); Serial.println(parameter);
          libWiFi::connectPassword = parameter;
          isSSID++;
          parameter = "";
        }
        else {
          Serial.println("> Solo se admiten 2 atributos.");
          nchar = parameters.length();
        }
      }
    }
    libWiFi::save(libWiFi::connectSsid, libWiFi::connectPassword);
  }
  else if (command == "reload") {
    if (libWiFi::load()) {
      if (libWiFi::connectWiFi()) {
        if (libWiFi::wifi_STA())
          Serial.println("> Successful recharge completed.");
        else
          Serial.println("> Full failed recharge");
      }
      else
        Serial.println("> Error while restarting WiFi AP module.");
    }
    else
      Serial.println("> Reload configuration file read failure.");
  }
  else if (command == "scan") {
    libWiFi::scanWiFi();
  }
  else if (command == "help") {
    libWiFi::help();
  }
  else {
    Serial.println("WIFI - Invalid command. Type help");
  }
}
