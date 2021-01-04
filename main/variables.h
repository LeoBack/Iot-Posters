
struct ConfigWifi {
  String ssid = "";
  String password = "";
};

struct ConfigMqtt {
  String IP_server = "";
  String sub_on = ""; 
  String sub_debug = "";
  String pub_status = "";
  String sub_task = "";
  String pub_task = "";
  String sub_tasklist = "";
  String pub_tasklist = "";
};

struct Config {
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


boolean ModeAp = false;
boolean on_off = true;
boolean debug = true;
String run_sec_name = "none";
boolean mqtt_conn = false; 

InputConsole inputConsole;
Timer timerRunSecuence;
Timer timerCheckSecuence;
Timer timerMqtt;
ClientRequest clientResquest;
Config config;

void print_config() {
  Serial.printf("ID: %d\n", config.id);
  Serial.printf("Name: %s\n", config.name.c_str());
  Serial.printf("ON: %s\n", config.on ? "True" : "false");
  Serial.printf("Debug: %s\n", config.debug  ? "True" : "false");
  Serial.printf("WIFI SSID: %s\n", config.wifi.ssid.c_str());
  Serial.printf("WIFI Password: %s\n", config.wifi.password.c_str());
  Serial.printf("MQTT IP_server: %s\n", config.mqtt.IP_server.c_str());
  Serial.printf("MQTT sub_on: %s\n", config.mqtt.sub_on.c_str());
  Serial.printf("MQTT sub_debug: %s\n", config.mqtt.sub_debug.c_str());
  Serial.printf("MQTT pub_status: %s\n", config.mqtt.pub_status.c_str());
  Serial.printf("MQTT sub_task: %s\n", config.mqtt.sub_task.c_str());
  Serial.printf("MQTT pub_task: %s\n", config.mqtt.pub_task.c_str());
  Serial.printf("MQTT sub_tasklist: %s\n", config.mqtt.sub_tasklist.c_str());
  Serial.printf("MQTT pub_tasklist: %s\n", config.mqtt.pub_tasklist.c_str());
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

  StaticJsonDocument<650> doc;
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

  const char* IP_server = doc["mqtt"]["IP_server"];
  const char* sub_on = doc["mqtt"]["sub_on"];
  const char* sub_debug = doc["mqtt"]["sub_debug"];
  const char* pub_status = doc["mqtt"]["pub_status"];
  const char* sub_task = doc["mqtt"]["sub_task"];
  const char* pub_task = doc["mqtt"]["pub_task"];
  const char* sub_tasklist = doc["mqtt"]["sub_tasklist"];
  const char* pub_tasklist = doc["mqtt"]["pub_tasklist"];
  config.mqtt.IP_server = IP_server;
  config.mqtt.sub_on = sub_on;
  config.mqtt.sub_debug = sub_debug;
  config.mqtt.pub_status = pub_status;
  config.mqtt.sub_task = sub_task;
  config.mqtt.pub_task = pub_task;
  config.mqtt.sub_tasklist = sub_tasklist;
  config.mqtt.pub_tasklist = pub_tasklist;

  if (config.debug) {
    Serial.println("Load config ---");
    print_config();
  }

  return true;
}

boolean save_config() {
  StaticJsonDocument<550> doc;
  doc["id"] = config.id;
  doc["name"] = config.name;
  doc["on"] = config.on;
  doc["debug"] = config.debug;
  doc["wifi"]["ssid"] = config.wifi.ssid;
  doc["wifi"]["password"] = config.wifi.password;
  doc["mqtt"]["IP_server"] = config.mqtt.IP_server;  
  doc["mqtt"]["sub_on"] = config.mqtt.sub_on;
  doc["mqtt"]["sub_debug"] = config.mqtt.sub_debug;
  doc["mqtt"]["pub_status"] = config.mqtt.pub_status;
  doc["mqtt"]["sub_task"] = config.mqtt.sub_task;
  doc["mqtt"]["pub_task"] = config.mqtt.pub_task;
  doc["mqtt"]["sub_tasklist"] = config.mqtt.sub_tasklist;
  doc["mqtt"]["pub_tasklist"] = config.mqtt.pub_tasklist;

  File configFile = SPIFFS.open(CONFIG, "w");
  if (!configFile) {
    Serial.println("Error opening configuration file for writing");
    return false;
  }
  serializeJson(doc, configFile);

  if (config.debug) {
    Serial.println("Save config ---");
    print_config();
  }

  return true;
}
