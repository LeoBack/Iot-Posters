WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE  (50)

unsigned long lastMsg = 0;
char msg[MSG_BUFFER_SIZE];
int value = 0;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.printf("Message arrived [%s]\n", topic);
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();

 /*
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1')
    digitalWrite(BUILTIN_LED, LOW);   
    // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because it is acive low on the ESP-01)
  else
    digitalWrite(BUILTIN_LED, HIGH);  
    // Turn the LED off by making the voltage HIGH
*/
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(config.name.c_str())) {
      Serial.println("connected");
      client.publish(config.mqtt.pub.c_str(), "hello world");
      client.subscribe(config.mqtt.sub.c_str());
    } else {
      Serial.printf("failed, rc=%d try again in 5 seconds\n", client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void mqtt_begin() {
  IPAddress Ip(192, 168, 100, 70);//(config.mqtt.IP_server);
  client.setServer(Ip, 1883);
  client.setCallback(callback);
}

void mqtt_publish() {
 
  if (!client.connected())
    reconnect();

  client.loop();
 
  //unsigned long now = millis();
  //if (now - lastMsg > 2000) {
  //  lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.printf("Publish message: %s", msg);
    client.publish(config.mqtt.pub.c_str(), msg);
  //}
}
