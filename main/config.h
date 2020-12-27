
#define DEBUG 1

/*
   Colores

*/
const byte ENABLE_RGB = 1;
const byte COLOR_INV = 0;

/*
   Nombre de las secuencias

*/
const char* BLINK = "blink";
const char* EVEN_OR_ODD = "even_or_odd";
const char* MOBILE_POINT = "mobile_point";
const char* MOBILE_HOLLOW = "mobile_hollow";
const char* POSITIVE_LADDER = "positive_ladder";
const char* NEGATIVE_LADDER = "negative_ladder";
const char* ON = "on";
const char* OFF = "off";
const char* SEC_INV = "inv";

// Cantidad de secuencias a configurar. 
const int MAX_SECUENCES = 4;

/*
   Tiempos

*/
const unsigned long INTERVAL_RUN_SECUENCE = 10000;
const unsigned long INTERVAL_CHECK_SECUENCE = 10000;

/*
   Configuracion WIFI

*/
const char* DOMAIN_NAME = "ESP8266";
const char* ssid = "ABCK";
const char* ssidAP = "AP_IOT";
const char* password = "ot3079996";
const char* hostname = "ESP8266_1";
const IPAddress ip(192, 168, 1, 200);
const IPAddress gateway(192, 168, 1, 1);
const IPAddress subnet(255, 255, 255, 0);
const int attemptsToReconnect = 90;

/*
   Directorio

*/
const char* CONFIG = "/config.json";
const char* DIR_PROGRAM = "/task/";

/*
   Set cantidada de canales
   IC_NUMBER * 8 = Cant. canales

*/
const byte IC_NUMBER = 2;
const byte INV_POLARITY = 0;
/*
   Configuracion GPIO
   D4 - 74HC595 DS (pin 14) entrada de datos.
   D5 - 74HC595 SCTP (pin 12) fija los datos.
   D3 - 74HC595 CLK (pin 11) pulso de reloj.
*/
const byte pinPULSADOR = D0;
const byte pinSCL = D1;
const byte pinSDA = D2;
const byte pinData = D5;
const byte pinRegister = D4;
const byte pinClock = D3;
const byte pinRED = D7;
const byte pinGREEN = D8;
const byte pinBLUE  = D6;
