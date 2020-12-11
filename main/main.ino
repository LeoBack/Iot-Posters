#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//#include <ESP8266mDNS_Legacy.h>
//#include <LEAmDNS.h>
//#include <LEAmDNS_lwIPdefs.h>
//#include <LEAmDNS_Priv.h>
#include <ESPAsyncWebServer.h>
#include <ListLib.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <RTClib.h>
#include "config.h"
#include "variables.h"
#include "libFile.hpp"
#include "libWiFi.hpp"
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
  
  File.begin();
  Rtc.begin();
  
  setup_sec();

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

  Wifi.begin(LED_BUILTIN);
  ModeAp = Wifi.connectWiFi();
  InitServer();
  InitWebSockets();

  //ESP.wdtDisable();     // Desactivo WDG soft
  //ESP.wdtEnable(1000);  // Habilita WDG soft
}

void loop(void)
{
  //  obsD0.Next();
  //  obsD5.Next();
  //  obsD6.Next();
  //  obsD7.Next();

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
      if (isDebug)
        Serial.print("> Command input= "); Serial.println(inputConsole.readConsole);
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

      if (isDebug) {
        Serial.print("Module= "); Serial.println(inputConsole.module);
        Serial.print("Command= "); Serial.println(inputConsole.command);
        Serial.print("Parameters= "); Serial.println(inputConsole.parameters);
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
        isDebug = !isDebug;
      else if (inputConsole.module == "off")
        on_off = 0;
      else if (inputConsole.module == "on")
        on_off = 1;
      //      else if (inputConsole.module == "pause")
      //        isRun = PAUSE;
      else
        Serial.println("Module no valid");
      inputConsole.isRead = false;
    }

    if (on_off == 1) {
      timerRunSecuence.currentMillis = millis();
      if (timerRunSecuence.currentMillis - timerRunSecuence.previousMillis >= timerRunSecuence.interval) {
        timerRunSecuence.previousMillis = timerRunSecuence.currentMillis;

        // Executa
        timerRunSecuence.interval = executeNextProgramming(pSelect, true);
      }

      timerCheckSecuence.currentMillis = millis();
      if (timerCheckSecuence.currentMillis - timerCheckSecuence.previousMillis >= timerCheckSecuence.interval) {
        timerCheckSecuence.previousMillis = timerCheckSecuence.currentMillis;

        // Busca la configuracion que se debe ejecutar
        pSelect = FindScheduleFile();
        run_sec_name = pSelect.name;
      }
    }
    //    else if (isRun == PAUSE) {
    //      sec_pause();
    //    }
    else {
      sec_reset();
    }

    if (clientResquest.isUp) {
      //writeMain(String(isRun), pSelect.name);
      //detailTask("Blue");

      clientResquest.isUp = false;
    }
  }
  else {
    //Serial.println("RUN Mode AP");
  }

  if (on_off_temp != on_off) {
    on_off_temp = on_off;
    result_on_off(on_off_temp);
  }
}
