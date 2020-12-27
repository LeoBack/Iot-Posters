//enum statusRun { ON, OFF, PAUSE };
struct ConfigWifi {
  String ssid = "";
  String password = "";
};

struct ConfigMqtt { 
  String pub = "";
  String sub = "";
};

struct Config 
  unsigned int id = 0;
  String name = "";
  boolean on = true;
  boolean debug = true;
  ConfigWifi wifi; 
  ConfigMqtt mqtt;
};

struct InputConsole {
  boolean isRead = false;
  String readConsole = "";
  String module = "";
  String command = "";
  String parameters = "";
};

struct Timer {
  unsigned long previousMillis;
  unsigned long currentMillis;
  unsigned long interval = 1000;
};

struct ClientRequest {
  boolean isUp = false;
  String command;
  String va1;
  String va2;
};


boolean isDebug = false;
boolean ModeAp = false;
byte on_off = 1;
byte on_off_temp = 1;
String run_sec_name = "none";

//statusRun isRun = ON;
InputConsole inputConsole;
Timer timerRunSecuence;
Timer timerCheckSecuence;
ClientRequest clientResquest;
Config config;

void print_config() {
  Serial.printf("ID: %d", config.id);
  Serial.printf("Name: %s", config.name.c_str());
  Serial.printf("ON: %s", config.on ? "True" : "false");
  Serial.printf("Debug: %s", config.debug  ? "True" : "false");
  Serial.printf("SSID: %s", config.wifi.ssid.c_str());
  Serial.printF("Password: %s", config.wifi.password.c_str());
  Serial.printf("Suscribe: %s", config.mqtt.sub.c_str());
  Serial.printf("Publisher: %s", config.mqtt.pub.c_str());
}

// Config
boolean load_config() {
  File configFile = SPIFFS.open(CONFIG, "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }

  if (configFile.size() > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  StaticJsonDocument<600> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.println("Error deserializing the configuration file");
    return false;
  }

  const unsigned int id = doc["id"];
  const char* name = doc["name"];
  const boolean on = doc["on"];
  const boolean debug = doc["debug"];
  config.id = id;
  config.name = name;
  config.on = on;
  config.debug = debug;

  const char* ssid = doc["wifi"]["ssid"];
  const char* password = doc["wifi"]["password"];
  config.wifi.ssid = ssid;
  config.wifi.password = password;

  const char* sub = doc["mqtt"]["ssid"];
  const char* pub = doc["mqtt"]["password"];
  config.mqtt.sub = sub;
  config.mqtt.pub = pub;

  if (config.debug) {
    Serial.println("--- Load config");
    print_config();
  }

  return true;
}

boolean save_config() {
  StaticJsonDocument<500> doc;
  doc["id"] = config.id;
  doc["name"] = config.name;
  doc["on"] = config.on;
  doc["debug"] = config.debug;
  doc["wifi"]["ssid"] = config.wifi.ssid;
  doc["wifi"]["password"] = config.wifi.password;
  doc["mqtt"]["sub"] = config.mqtt.sub;
  doc["mqtt"]["pub"] = config.mqtt.pub;

  File configFile = SPIFFS.open(CONFIG, "w");
  if (!configFile) {
    Serial.println("Error opening configuration file for writing");
    return false;
  }
  serializeJson(doc, configFile);

  if (config.debug) {
    Serial.println("--- Save config");
    print_config();
  }

  return true;
}
