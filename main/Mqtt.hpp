WiFiClient espClient;
PubSubClient mqtt_client(espClient);

#define MSG_BUFFER_SIZE  (50)

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message arrived [%s]\n", topic);
  String vPayload = "";
  for (int i = 0; i < length; i++)
    vPayload += (char)payload[i];
  Serial.printf("payload: %s\n", vPayload.c_str());

  String vTopic = String(topic);
  if (vTopic == config.mqtt.sub_on) {
    on_off = vPayload == "true" ? true : false;
  }
  if (vTopic == config.mqtt.sub_debug) {
    debug = vPayload == "true" ? true : false;
  }

}

void mqtt_begin() {
  Serial.printf("Ip: %s", config.mqtt.IP_server.c_str());
  //IPAddress Ip;  
  //if (Ip.fromString(config.mqtt.IP_server)) {
  //  mqtt_client.setServer(Ip, 1883);
  //  mqtt_client.setCallback(callback);
  //}

  //IPAddress Ip(192, 168, 100, 26);
  IPAddress Ip;
  if (Ip.fromString(config.mqtt.IP_server))
    Serial.println("Ip.fromString True");
  else
      Serial.println("Ip.fromString False");
      
  mqtt_client.setServer(Ip, 1883);
  mqtt_client.setCallback(callback);
}

boolean mqtt_connect() {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  if (mqtt_client.connect(config.name.c_str())) {
    Serial.println("connected");

    if (config.mqtt.sub_on != "") {
      mqtt_client.subscribe(config.mqtt.sub_on.c_str());
      Serial.printf("Subscribe [%s]\n", config.mqtt.sub_on.c_str());
    }

    if (config.mqtt.sub_debug != "") {
      mqtt_client.subscribe(config.mqtt.sub_debug.c_str());
      Serial.printf("Subscribe [%s]\n", config.mqtt.sub_debug.c_str());
    }

    if (config.mqtt.sub_task != "") {
      mqtt_client.subscribe(config.mqtt.sub_task.c_str());
      Serial.printf("Subscribe [%s]\n", config.mqtt.sub_task.c_str());
    }

    if (config.mqtt.sub_tasklist != "") {
      mqtt_client.subscribe(config.mqtt.sub_tasklist.c_str());
      Serial.printf("Subscribe [%s]\n", config.mqtt.sub_tasklist.c_str());
    }

    return true;
  } else {
    Serial.printf("failed, rc=%d try again in 5 seconds\n", mqtt_client.state());
    return false;
  }
}

void mqtt_publish_status(String sec_name) {
  mqtt_client.loop();
  
  RTC_DS3231 rtc;
  float temp = rtc.getTemperature();

  String json;
  StaticJsonDocument<500> doc;
  doc["id"] = config.id;
  doc["name"] = config.name;
  doc["on"] = config.on;
  doc["debug"] = config.debug;
  doc["sec_name"] = sec_name;
  doc["temp"] = temp;
  serializeJson(doc, json);

  Serial.printf("Publish message: %s\n", json.c_str());
  mqtt_client.publish(config.mqtt.pub_status.c_str(), json.c_str());
}



// -- Original ejemplo
/*
  void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message arrived [%s]\n", topic);
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
    digitalWrite(BUILTIN_LED, LOW);
  // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because it is acive low on the ESP-01)
  else
    digitalWrite(BUILTIN_LED, HIGH);
  // Turn the LED off by making the voltage HIGH

  }

  void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(config.name.c_str())) {
      Serial.println("connected");
      client.subscribe(config.mqtt.sub_on.c_str());
      client.subscribe(config.mqtt.sub_task.c_str());
      client.subscribe(config.mqtt.sub_tasklist.c_str());
      client.publish(config.mqtt.pub_status.c_str(), "hello world");
    } else {
      Serial.printf("failed, rc=%d try again in 5 seconds\n", client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  }

  void publish() {

  if (!client.connected())
    reconnect();

  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.printf("Publish message: %s\n", msg);
    client.publish(config.mqtt.pub_status.c_str(), msg);
  }
  }
*/
