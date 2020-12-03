AsyncWebServer server(80);

void InitServer()
{
  if (WiFi.getMode() == WIFI_STA) {
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    ModeAp = false;
  }
  else {
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("ap.html");
    ModeAp = true;
  }

  server.onNotFound([](AsyncWebServerRequest * request) {
    request->send(400, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("HTTP server started");
}
