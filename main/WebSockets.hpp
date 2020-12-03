AsyncWebSocket ws("/ws");

/*
  WEB MAIN
  Carga inicial para la pagina web.
  OK - 20/11/15
*/
void mainOnload() {
  Serial.println("WEB-PAGE-> Load");
  libRTC Rtc;

  String response;
  StaticJsonDocument<500> doc;
  doc["command"] = "main_load";
  doc["on_off"] = on_off;
  doc["clock"] = Rtc.getDatetime();
  doc["run_sec"] = run_sec_name;
  doc["isRGB"] = ENABLE_RGB;
  JsonArray arr = doc.createNestedArray("tasks");

  int nTasks = 0;
  Dir files = SPIFFS.openDir(DIR_PROGRAM);
  while (files.next()) {
    if (readProgramHeader(files.fileName(), false)) {
      String line = (String)pGrm.enable;
      line.concat("|");
      line.concat(pGrm.name);
      line.concat("|");
      line.concat(Rtc.datetimeToString(pGrm.iniDate));
      line.concat("|");
      line.concat(Rtc.datetimeToString(pGrm.endDate));
      arr.add(line);

      Serial.print("WS: #");
      Serial.print(nTasks);
      Serial.print(" - ");
      Serial.println(line);

      ESP.wdtFeed();
    }
    nTasks++;
  }

  doc["n_tasks"] = nTasks;
  serializeJson(doc, response);
  ESP.wdtFeed();
  Serial.println("WS: Send");
  ws.textAll(response);
}

/*
  WEB MAIN
  Devuelve el detalles de una tarea.
  OK - 20/11/16
*/
void readTask(String name_task)
{
  libRTC Rtc;
  String response;
  StaticJsonDocument<700> doc;
  doc["command"] = "detail_task";

  String fileRead = DIR_PROGRAM;
  fileRead.concat(name_task);
  fileRead.concat(".json");

  Serial.print("WS: Read task: ");
  Serial.println(fileRead);

  if (readProgram(fileRead, false))
  {
    doc["name"] = pGrm.name;
    doc["enable"] = pGrm.enable;
    doc["iniDate"] = Rtc.datetimeToString(pGrm.iniDate);
    doc["endDate"] = Rtc.datetimeToString(pGrm.endDate);

    doc["sec1_index"] = pGrm.sec1_index;
    doc["sec1_rgb"] = pGrm.sec1_rgb;
    doc["sec1_millis"] = pGrm.sec1_millis;
    doc["sec1_rotation"] = pGrm.sec1_rotation;
    doc["sec1_repeat"] = pGrm.sec1_repeat;

    doc["sec2_index"] = pGrm.sec2_index;
    doc["sec2_rgb"] = pGrm.sec2_rgb;
    doc["sec2_millis"] = pGrm.sec2_millis;
    doc["sec2_rotation"] = pGrm.sec2_rotation;
    doc["sec2_repeat"] = pGrm.sec2_repeat;

    doc["sec3_index"] = pGrm.sec3_index;
    doc["sec3_rgb"] = pGrm.sec3_rgb;
    doc["sec3_millis"] = pGrm.sec3_millis;
    doc["sec3_rotation"] = pGrm.sec3_rotation;
    doc["sec3_repeat"] = pGrm.sec3_repeat;

    doc["sec4_index"] = pGrm.sec4_index;
    doc["sec4_rgb"] = pGrm.sec4_rgb;
    doc["sec4_millis"] = pGrm.sec4_millis;
    doc["sec4_rotation"] = pGrm.sec4_rotation;
    doc["sec4_repeat"] = pGrm.sec4_repeat;
  }
  serializeJson(doc, response);
  ws.textAll(response);
}

/*
   Establece la fecha y hora desde la pagina web.
   OK - 20/11/16
*/
void setClock(String vDate, String vTime) {
  libRTC Rtc;

  String response;
  StaticJsonDocument<100> doc;
  doc["command"] = "set_clock_result";
  doc["result"] = Rtc.setDatetime(vDate, vTime) ? 1 : 0;
  serializeJson(doc, response);
  ws.textAll(response);
}

/*
   Establece estado desde la pagina web.
   OK - 20/11/18
*/
void std_status(String v_on_off) {
  on_off = v_on_off.toInt();
  Serial.print("WS: std_status=> std_status: "); Serial.print(on_off);
}

/*
   Respuesta del estado desde la pagina web.
   OK - 20/11/20
*/
void result_on_off(byte v_on_off) {
  String response;
  StaticJsonDocument<100> doc;
  doc["command"] = "std_status_result";
  doc["result"] = v_on_off ? 1 : 0;
  serializeJson(doc, response);
  ws.textAll(response);
}

/*
   Resive una tarea. Nueva o Reemplaza si el nombre ya existe.
   VER - 20/11/24
*/
void set_task(String task) {
  
  byte v_ok = 1;
  
  String response;
  StaticJsonDocument<100> doc;
  doc["command"] = "set_task_result";
  doc["result"] = v_ok ? 1 : 0;
  serializeJson(doc, response);
  ws.textAll(response);
}

/*
   Elimina una tarea.
   CHECK - 20/11/24
*/
void rm_task(String name_task) {
  libFile file;
  String dir = DIR_PROGRAM + name_task + ".json";
  boolean v_ok = file.rm(dir);

  String response;
  StaticJsonDocument<100> doc;
  doc["command"] = "rm_task_result";
  doc["result"] = v_ok ? 1 : 0;
  serializeJson(doc, response);
  ws.textAll(response);
}

/*
   Atiende las solicitudes de la pagina web.
   VER - 20/11/15
*/
void ProcessRequest(AsyncWebSocketClient *client, String request)
{
  StaticJsonDocument<1000> doc;
  DeserializationError error = deserializeJson(doc, request);
  if (error)
    return;

  String command = doc["command"];

  if (command == "write_wifi")              // OK
    writeWifi(doc["ssid"], doc["password"]);
  else if (command == "main_onload")        // OK
    mainOnload();
  else if (command == "select_task")        // OK
    readTask(doc["name"]);
  else if (command == "set_clock")          // OK
    setClock(doc["date"], doc["time"]);
  else if (command == "set_std_status")     // OK
    std_status(doc["on_off"]);
  else if (command == "set_task")           // OK
    set_task(doc["command"]);
  else if (command == "rm_task")            // OK
    rm_task(doc["name_task"]);
  else {                                    // OK
    Serial.print("WS invalid command: ");
    Serial.println(command);
  }
  //else if (command == "scan_wifi")
  // scanWifi();
}



///*
//   #- WifiAP
//   1- Retorna lista de wifi detectadas
//*/
//void scanWifi()
//{
//  clientResquest.isUp = true;
//
//  Serial.println("scanWifi");
//
//  String response;
//  StaticJsonDocument<1000> doc;
//  doc["command"] = "responce_scan_wifi";
//
//  libWiFi Wifi;
//  //int n = WiFi.scanNetworks();
//  int n = WiFi.scanNetworks(false, true);
//  Serial.print("scan detetec:");
//  Serial.println(n);
//
//  doc["n_detected_networks"] = n;
//  JsonObject obj = doc.createNestedObject("networks");
//  for (int i = 0; i < n; i++)
//  {
//    obj["SSID"] = WiFi.SSID(i);
//    obj["RSSI"] = WiFi.RSSI(i);
//    obj["ENC"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
//  }
//
//  serializeJson(doc, response);
//  ws.textAll(response);
//}
//
///*
//   #- WifiAP
//   1- Devuelve los datos del wifi.
//*/
//void readWifi()
//{
//  libWiFi Wifi;
//  String response;
//  StaticJsonDocument<300> doc;
//  doc["command"] = "readWifi";
//  if (Wifi.load())
//  {
//    doc["ssid"] = Wifi.connectSsid;
//    doc["password"] = Wifi.connectPassword;
//  }
//  else
//  {
//    doc["ssid"] = "";
//    doc["password"] = "";
//  }
//  serializeJson(doc, response);
//  ws.textAll(response);
//}
