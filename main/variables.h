//enum statusRun { ON, OFF, PAUSE };


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
