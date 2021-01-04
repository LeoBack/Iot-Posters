#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//#include <ESP8266mDNS_Legacy.h>
//#include <LEAmDNS.h>
//#include <LEAmDNS_lwIPdefs.h>
//#include <LEAmDNS_Priv.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <ListLib.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <RTClib.h>
#include "config.h"
#include "variables.h"
#include "libFile.hpp"
#include "libWiFi.hpp"
#include "mqtt.hpp"
#include "libRTC.hpp"
#include "libColor.hpp"
#include "Sequences.hpp"
#include "API.hpp"
#include "WebSockets.hpp"
#include "Server.hpp"
#include "ESP8266_Utils_AWS.hpp"
//#include "ReactiveArduinoLib.h"

libRTC Rtc;
libFile File;
libWiFi Wifi;
taskScheduling pSelect;

bool mqtt_refresh = false;

// Para notificar cambios desde el servidor al cliente
//auto obsD0 = Reactive::FromDigitalInput(D0);
//auto obsD5 = Reactive::FromDigitalInput(D5);
//auto obsD6 = Reactive::FromDigitalInput(D6);
//auto obsD7 = Reactive::FromDigitalInput(D7);

void setup(void)
{
  Serial.begin(115200);
  delay(100);

  timerRunSecuence.interval = INTERVAL_RUN_SECUENCE;
  timerCheckSecuence.interval = INTERVAL_CHECK_SECUENCE;
  timerMqtt.interval = 5000;

  Rtc.begin();
  File.begin();

  //  obsD0.Distinct().Do([](int i) { updateGPIO("D0", i); });
  //  obsD5.Distinct().Do([](int i) { updateGPIO("D5", i); });
  //  obsD6.Distinct().Do([](int i) { updateGPIO("D6", i); });
  //  obsD7.Distinct().Do([](int i) { updateGPIO("D7", i); });

  Serial.println("------------------------------");
  Serial.printf("\n\nSdk version: %s\n", ESP.getSdkVersion());
  Serial.printf("Core Version: %s\n", ESP.getCoreVersion().c_str());
  Serial.printf("Boot Version: %u\n", ESP.getBootVersion());
  Serial.printf("Boot Mode: %u\n", ESP.getBootMode());
  Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Reset reason: %s\n", ESP.getResetReason().c_str());
  Serial.println("------------------------------");

  load_config();
  setup_sec();

  Wifi.begin(config.wifi, LED_BUILTIN);
  ModeAp = Wifi.connectWiFi();
  InitServer();
  InitWebSockets();

  //ESP.wdtDisable();     // Desactivo WDG soft
  //ESP.wdtEnable(1000);  // Habilita WDG soft

  mqtt_begin();
  mqtt_conn = mqtt_connect();
}

void loop(void)
{
  //  obsD0.Next();
  //  obsD5.Next();
  //  obsD6.Next();
  //  obsD7.Next();
  mqtt_client.loop();

  if (!ModeAp) {

    // Lectura desde la consola
    while (Serial.available() > 0 ) {
      inputConsole.readConsole += (char)Serial.read();
      if (Serial.available() == 0) {
        inputConsole.isRead = true;
      }
    }
    delay(20);
    // Proceso array
    if (inputConsole.isRead) {
      if (config.debug)
        Serial.printf("> Command input= %s\n", inputConsole.readConsole.c_str());

      inputConsole.readConsole.trim();
      //inputConsole.readConsole.toLowerCase();
      inputConsole.module = inputConsole.readConsole.substring(0, inputConsole.readConsole.indexOf(" "));
      inputConsole.module.trim();
      inputConsole.module.toLowerCase();
      inputConsole.command = inputConsole.readConsole.substring(inputConsole.readConsole.indexOf(" "));
      inputConsole.command.trim();
      inputConsole.parameters = inputConsole.command.substring(inputConsole.command.indexOf(" "));
      inputConsole.command = inputConsole.command.substring(0, inputConsole.command.indexOf(" "));
      inputConsole.parameters.trim();
      inputConsole.readConsole = "";

      if (config.debug) {
        Serial.printf("Module= %s\n", inputConsole.module.c_str());
        Serial.printf("Command= %s\n", inputConsole.command.c_str());
        Serial.printf("Parameters= %s\n", inputConsole.parameters.c_str());
      }

      if (inputConsole.module == "file")
        File.menu(inputConsole.command, inputConsole.parameters);
      else if (inputConsole.module == "color") {
        libColor Color;
        Color.begin(pinRED, pinGREEN, pinBLUE);
        Color.menu(inputConsole.command, inputConsole.parameters);
      }
      else if (inputConsole.module == "secmode")
        Secuences_Menu(inputConsole.command, inputConsole.parameters);
      else if (inputConsole.module == "rtc")
        Rtc.menu(inputConsole.command, inputConsole.parameters);
      else if (inputConsole.module == "wifi")
        Wifi.menu(inputConsole.command, inputConsole.parameters);
      else if (inputConsole.module == "debug")
        debug = !debug;
      else if (inputConsole.module == "off")
        on_off = false;
      else if (inputConsole.module == "on")
        on_off = true;
      else
        Serial.println("Module no valid");
      inputConsole.isRead = false;
    }

    if (on_off) {
      // Selecciona taera a ejecutar
      timerRunSecuence.currentMillis = millis();
      if (timerRunSecuence.currentMillis - timerRunSecuence.previousMillis >= timerRunSecuence.interval) {
        timerRunSecuence.previousMillis = timerRunSecuence.currentMillis;

        // Executa
        timerRunSecuence.interval = executeNextProgramming(pSelect, true);
      }
      
      // Ejecuta secuencia de tarea actual
      timerCheckSecuence.currentMillis = millis();
      if (timerCheckSecuence.currentMillis - timerCheckSecuence.previousMillis >= timerCheckSecuence.interval) {
        timerCheckSecuence.previousMillis = timerCheckSecuence.currentMillis;

        // Busca la configuracion que se debe ejecutar
        pSelect = FindScheduleFile();
        run_sec_name = pSelect.name;
      }
    }
    else {
      sec_reset();
    }
    
    //
    timerMqtt.currentMillis = millis();
    if (timerMqtt.currentMillis - timerMqtt.previousMillis >= timerMqtt.interval) {
      timerMqtt.previousMillis = timerMqtt.currentMillis;

      if (!mqtt_conn) {
        mqtt_conn = mqtt_connect();
      } else {
        mqtt_publish_status(run_sec_name);
      }

    }
  }
  else {
    //Serial.println("RUN Mode AP");
  }

  if (config.on != on_off) {
    config.on = on_off;
    mqtt_refresh = true;
  }

  if (config.debug != debug) {
    config.debug = debug;
    mqtt_refresh = true;
  }

  if (mqtt_refresh){
    save_config();
    result_on_off(config.on);
    mqtt_refresh = false;
  }
}
