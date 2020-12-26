
#define NORMAL 0
#define INVERTIDO 1

struct Execute_sec
{
  unsigned int id = 0;
  String name = "";               // Nombre de la secuencia.
  signed int array_IC[IC_NUMBER]; // Array donde se almacena el valor a escribir.
  byte index_IC = 0;              // IC seleccionado del array.
  int index_Byte = 0;             // Byte seleccionado dentro del array de secuencia en ejecucion.
  byte toggle = 0;                // Bandera blink
  byte rotation = NORMAL;
  byte repeat_counter = 0;      // Lleva la cuenta de la cantidad de repeticiones.
  boolean is_completed = false; // Secuencia completada
  boolean firts_run = true;     // Primer arranque
  byte out_default = 0x00;      // Estado inicial de la salida
};

struct NOW_scheduledTask {
  unsigned int id = 0;
  unsigned int sec_select = 0;   // Secuencia seleccionada
  unsigned int sec_index = 1;    // Secuencia a ejecutar.
  String sec_Rgb = "000000";     // Color de toda la secuencia.
  long sec_timeMillis = 100;     // Tiempo de cada paso.
  boolean sec_rotation = false; // Invierte o no el sentido de ejecucion.
  unsigned int sec_repeat = 0;   // Cantidad de veces que se debe ejecutar.
};

struct taskSchedulingSec {
  unsigned int sec_index = 0;
  String sec_rgb = "000000";
  unsigned int sec_millis = 0;
  boolean sec_rotation = false;
  unsigned int sec_repeat = 1;
};

struct taskScheduling {
  unsigned int id = 0;
  String name = "Taskname_default";
  boolean enable = true;
  DateTime iniDate = DateTime(2020, 1, 1);
  DateTime endDate = DateTime(2030, 12, 30, 23, 59, 59);
  taskSchedulingSec TaskSec[MAX_SECUENCES];
};


taskScheduling pGrm;
NOW_scheduledTask NowScheduledTask;
libRTC R;
Execute_sec Execute;

//---Config. Secuencias----------------------------------------------

byte const Blink_size = 2;
byte const Blink[] = {
  0xFF, 0x00
};

int const EvenOrOdd_size = 2;
byte const EvenOrOdd[] = {
  0xAA, 0x55
};

byte const Point_size = 8;
byte const Point[] = {
  0x01, 0x02, 0x04, 0x08,
  0x10, 0x20, 0x40, 0x80
};

byte const Hole_size = 8;
byte const Hole[] = {
  0xFE, 0xFD, 0xFB, 0xF7,
  0xEF, 0xDF, 0xBF, 0x7F
};

byte const Ladder_size = 8;
byte const Ladder[] = {
  0x01, 0x03, 0x07, 0x0F,
  0x1F, 0x3F, 0x7F, 0xFF
};

byte const nLadder_size = 8;
byte const nLadder[] = {
  0xFE, 0xFC, 0xF8, 0xF0,
  0xE0, 0xC0, 0x80, 0x00
};

//---SETUP Programa--------------------------------------------------
void setup_sec() {
  pinMode(pinData, OUTPUT);
  pinMode(pinClock, OUTPUT);
  pinMode(pinRegister, OUTPUT);
  digitalWrite(pinRegister, LOW);
}

//---Funciones-------------------------------------------------------

// OK - 20/12/24
void print_debug() {
  if (DEBUG) {
    Serial.printf("#3 Sec.: %s, Byte: %d, Toggle: %d, Value: ", Execute.name.c_str(), Execute.index_Byte, Execute.toggle);
    for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++) {
      Serial.printf("0x%2X ", Execute.array_IC[index_IC]);
      //if (index_IC < IC_NUMBER - 1)
      //  Serial.print(" ");
    }
    Serial.println("");
  }
}

// OK
void write_74HC595(int regIC[]) {
  for (int i = 0; i < IC_NUMBER; i++)
    shiftOut(pinData, pinClock, LSBFIRST, regIC[i]);
  digitalWrite(pinRegister, HIGH);
  digitalWrite(pinRegister, LOW);
}

// OK
void out_set_value(unsigned int value) {
  for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++)
    Execute.array_IC[index_IC] = value;
}

// OK
byte out_clear() {
  byte value = 0x00;
  for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++)
    Execute.array_IC[index_IC] = value;
  return value;
}

// OK
byte out_full() {
  byte value = 0xFF;
  for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++)
    Execute.array_IC[index_IC] = value;
  return value;
}

//---Secuencias------------------------------------------------------

// OK
void sec_blink(boolean vPrint = false) {
  Execute.name = "Blink";

  if (Execute.is_completed)
    Execute.toggle = 0;

  Execute.index_IC = 0;
  while (Execute.index_IC < IC_NUMBER) {
    Execute.array_IC[Execute.index_IC] = Blink[Execute.toggle];
    Execute.index_IC++;
  }

  Execute.is_completed = Execute.toggle == Blink_size - 1;
  Execute.toggle = Execute.toggle < Blink_size - 1 ? Execute.toggle + 1 : 0;

  if (vPrint)
    print_debug();

  write_74HC595(Execute.array_IC);
}

// OK
void sec_even_or_odd(boolean vPrint = false) {
  Execute.name = "Even Or Odd";

  if (Execute.is_completed)
    Execute.toggle = 0;

  Execute.index_IC = 0;
  while (Execute.index_IC < IC_NUMBER) {
    Execute.array_IC[Execute.index_IC] = EvenOrOdd[Execute.toggle];
    Execute.index_IC++;
  }

  Execute.is_completed = Execute.toggle == EvenOrOdd_size - 1;
  Execute.toggle = Execute.toggle < EvenOrOdd_size - 1 ? Execute.toggle + 1 : 0;

  if (vPrint)
    print_debug();

  write_74HC595(Execute.array_IC);
}

// OK
void sec_mobile_point(boolean vPrint = false) {

  byte array_size = INV_POLARITY == 0 ? Point_size : Hole_size;

  if (Execute.firts_run) {
    Execute.name = "Mobile Point";
    Execute.out_default = INV_POLARITY == 0 ? out_clear() : out_full();
    Execute.index_IC = Execute.rotation == NORMAL ? 0 : (IC_NUMBER - 1);
    Execute.index_Byte = Execute.rotation == NORMAL ? 0 : (array_size - 1);
    Execute.is_completed = false;
    Execute.firts_run = false;
  }

  Execute.array_IC[Execute.index_IC] =  (INV_POLARITY == 0 ? Point : Hole)[Execute.index_Byte];
  write_74HC595(Execute.array_IC);

  if (vPrint)
    print_debug();

  if (Execute.rotation == NORMAL) {

    if (Execute.index_Byte < array_size) {
      Execute.index_Byte++;

      if (Execute.index_Byte == array_size) {
        Execute.array_IC[Execute.index_IC] = Execute.out_default;
        Execute.index_Byte = 0;

        if (Execute.index_IC < IC_NUMBER - 1)
          Execute.index_IC++;
        else {
          Execute.index_IC = 0;
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
  // INVERTIDO
  else {
    if (Execute.index_Byte >= 0) {
      Execute.index_Byte--;

      if (Execute.index_Byte == -1) {
        Execute.array_IC[Execute.index_IC] = Execute.out_default;
        Execute.index_Byte = (array_size - 1);

        if (Execute.index_IC > 0)
          Execute.index_IC--;
        else {
          Execute.index_IC = IC_NUMBER - 1;
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
}

// OK
void sec_mobile_hollow(boolean vPrint = false) {

  byte array_size = INV_POLARITY == 0 ? Point_size : Hole_size;

  if (Execute.firts_run) {
    Execute.name = "Mobile Hollow";
    Execute.out_default = INV_POLARITY == 0 ? out_full() : out_clear();
    Execute.index_IC = Execute.rotation == NORMAL ? 0 : (IC_NUMBER - 1);
    Execute.index_Byte = Execute.rotation == NORMAL ? 0 : (array_size - 1);
    Execute.is_completed = false;
    Execute.firts_run = false;
  }

  Execute.array_IC[Execute.index_IC] =  (INV_POLARITY == 0 ? Hole : Point)[Execute.index_Byte];
  write_74HC595(Execute.array_IC);

  if (vPrint)
    print_debug();

  if (Execute.rotation == NORMAL) {
    if (Execute.index_Byte < array_size) {
      Execute.index_Byte++;
      if (Execute.index_Byte == array_size) {
        Execute.array_IC[Execute.index_IC] = Execute.out_default;
        Execute.index_Byte = 0;

        if (Execute.index_IC < IC_NUMBER - 1)
          Execute.index_IC++;
        else {
          Execute.index_IC = 0;
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
  else {
    if (Execute.index_Byte >= 0) {
      Execute.index_Byte--;
      if (Execute.index_Byte == -1) {
        Execute.array_IC[Execute.index_IC] = Execute.out_default;
        Execute.index_Byte = 7;

        if (Execute.index_IC > 0)
          Execute.index_IC--;
        else {
          Execute.index_IC = IC_NUMBER - 1;
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
}

// OK
void sec_positive_ladder(boolean vPrint = false) {

  byte array_size = INV_POLARITY == 0 ? Ladder_size : nLadder_size;

  if (Execute.firts_run) {
    Execute.name = "Positive Ladder";
    Execute.out_default = INV_POLARITY == 0 ? out_clear() : out_full();
    Execute.index_IC = Execute.rotation == NORMAL ? 0 : (IC_NUMBER - 1);
    Execute.index_Byte = Execute.rotation == NORMAL ? 0 : (array_size - 1);
    Execute.is_completed = false;
    Execute.firts_run = false;
  }

  if (Execute.rotation == NORMAL)
    Execute.array_IC[Execute.index_IC] = (INV_POLARITY == 0 ? Ladder : nLadder)[Execute.index_Byte];
  else
    Execute.array_IC[Execute.index_IC] = ~(INV_POLARITY == 0 ? Ladder : nLadder)[Execute.index_Byte];

  write_74HC595(Execute.array_IC);

  if (vPrint)
    print_debug();

  if (Execute.rotation == NORMAL) {
    if (Execute.index_Byte < array_size) {
      Execute.index_Byte++;
      if (Execute.index_Byte == array_size) {
        Execute.index_Byte = 0;

        if (Execute.index_IC < IC_NUMBER - 1) {
          Execute.array_IC[Execute.index_IC] = Execute.out_default == 0x00 ? 0xFF : 0x00;
          Execute.index_IC++;
        }
        else {
          Execute.index_IC = 0;
          out_set_value(Execute.out_default);//out_clear();
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
  else {
    if (Execute.index_Byte >= 0) {
      Execute.index_Byte--;

      if (Execute.index_Byte == -1) {
        Execute.array_IC[Execute.index_IC] = Execute.out_default == 0x00 ? 0xFF : 0x00;
        Execute.index_Byte = 7;
        if (Execute.index_IC > 0) {
          Execute.index_IC--;
        }
        else {
          Execute.index_IC = IC_NUMBER - 1;
          out_set_value(Execute.out_default);
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
}

// OK
void sec_negative_ladder(boolean vPrint = false) {

  byte array_size = INV_POLARITY == 0 ? nLadder_size : Ladder_size;

  if (Execute.firts_run) {
    Execute.name = "Negative Ladder";
    Execute.out_default = INV_POLARITY == 0 ? out_full() : out_clear();
    Execute.index_IC = Execute.rotation == NORMAL ? 0 : (IC_NUMBER - 1);
    Execute.index_Byte = Execute.rotation == NORMAL ? 0 : (array_size - 1);
    Execute.is_completed = false;
    Execute.firts_run = false;
  }

  if (Execute.rotation == NORMAL)
    Execute.array_IC[Execute.index_IC] = (INV_POLARITY == 0 ? nLadder : Ladder)[Execute.index_Byte];
  else
    Execute.array_IC[Execute.index_IC] = ~(INV_POLARITY == 0 ? nLadder : Ladder)[Execute.index_Byte];

  write_74HC595(Execute.array_IC);

  if (vPrint)
    print_debug();

  if (Execute.rotation == NORMAL) {
    if (Execute.index_Byte < array_size) {
      Execute.index_Byte++;
      if (Execute.index_Byte == array_size) {
        Execute.index_Byte = 0;

        if (Execute.index_IC < IC_NUMBER - 1) {
          Execute.array_IC[Execute.index_IC] = INV_POLARITY == 0 ? 0x00 : 0xFF;
          Execute.index_IC++;
        }
        else {
          Execute.index_IC = 0;
          out_clear();
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
  else {
    if (Execute.index_Byte >= 0) {
      Execute.index_Byte--;
      if (Execute.index_Byte == -1) {
        Execute.array_IC[Execute.index_IC] = INV_POLARITY == 0 ? 0x00 : 0xFF; //0xFF;
        Execute.index_Byte = 7;
        if (Execute.index_IC > 0) {
          Execute.index_IC--;
        }
        else {
          Execute.index_IC = IC_NUMBER - 1;
          out_clear();
          Execute.is_completed = true;
          Execute.firts_run = true;
        }
      }
    }
  }
}

// OK
void sec_off(boolean vPrint = false) {
  Execute.name = "Off";

  if (Execute.is_completed)
    Execute.toggle = 0;

  out_clear();
  Execute.is_completed = true;

  if (vPrint)
    print_debug();

  write_74HC595(Execute.array_IC);
}

// OK
void sec_on(boolean vPrint = false) {
  Execute.name = "On";

  if (Execute.is_completed)
    Execute.toggle = 0;

  out_full();
  Execute.is_completed = true;

  if (vPrint)
    print_debug();

  write_74HC595(Execute.array_IC);
}

//---Lectura/Escritura-----------------------------------------------

// OK - 20/12/23 - TEST
boolean readProgramHeader(String vName, boolean vPrint = false) {
  File configFile = SPIFFS.open(vName, "r");
  if (!configFile) {
    Serial.println("Failed to open file");
    return false;
  }

  if (configFile.size() > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  ESP.wdtFeed();

  StaticJsonDocument<700> doc;
  DeserializationError error = deserializeJson(doc, configFile);

  if (error) {
    Serial.println("Error deserializing file");
    return false;
  }

  const unsigned int id = doc["id"];
  const char *name = doc["name"];
  const boolean enable = doc["enable"];
  const char *iniDate = doc["iniDate"];
  const char *endDate = doc["endDate"];
  pGrm.id = id;
  pGrm.name = name;
  pGrm.enable = enable;
  pGrm.iniDate = R.stringToDatetime(iniDate);
  pGrm.endDate = R.stringToDatetime(endDate);

  if (vPrint) {
    Serial.println("------- Read header -------");
    Serial.printf("[id= %d], name= %s\n", pGrm.id, pGrm.name.c_str());
    Serial.printf("enable= %s\n", (pGrm.enable ? "true" : "false"));
    Serial.printf("iniDate= %s\n", R.datetimeToString(pGrm.iniDate).c_str());
    Serial.printf("endDate= %s\n", R.datetimeToString(pGrm.endDate).c_str());
    Serial.println("---------------------------");
  }

  return true;
}

// OK - 20/12/23
boolean readProgram(String vName, boolean vPrint = false) {
  File configFile = SPIFFS.open(vName, "r");
  if (!configFile) {
    Serial.println("Failed to open file");
    return false;
  }

  if (configFile.size() > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  StaticJsonDocument<700> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.print("Error deserializing file: ");
    Serial.println(vName);
    return false;
  }

  const unsigned int id = doc["id"];
  const char *name = doc["name"];
  const boolean enable = doc["enable"];
  const char *iniDate = doc["iniDate"];
  const char *endDate = doc["endDate"];
  pGrm.name = name;
  pGrm.enable = enable;
  pGrm.iniDate = R.stringToDatetime(iniDate);
  pGrm.endDate = R.stringToDatetime(endDate);
  for (int i = 0; i < MAX_SECUENCES; i++) {
    String sec_read = "secuence_" + String(i);
    const unsigned int sec_index = doc[sec_read]["sec_index"];
    const char *sec_rgb = doc[sec_read]["sec_rgb"];
    const unsigned int sec_millis = doc[sec_read]["sec_millis"];
    const boolean sec_rotation = doc[sec_read]["sec_rotation"];
    const unsigned int sec_repeat = doc[sec_read]["sec_repeat"];
    pGrm.id = id;
    pGrm.TaskSec[i].sec_index = sec_index;
    pGrm.TaskSec[i].sec_rgb = sec_rgb;
    pGrm.TaskSec[i].sec_millis = sec_millis;
    pGrm.TaskSec[i].sec_rotation = sec_rotation;
    pGrm.TaskSec[i].sec_repeat = sec_repeat;
  }
  ESP.wdtFeed();

  if (vPrint) {
    Serial.println("------- Read config -------");
    Serial.printf("[id= %d], name= %s\n", pGrm.id, pGrm.name.c_str());
    Serial.printf("enable= %s\n", (pGrm.enable ? "true" : "false"));
    Serial.printf("iniDate= %s\n", R.datetimeToString(pGrm.iniDate).c_str());
    Serial.printf("endDate= %s\n", R.datetimeToString(pGrm.endDate).c_str());
    for (int i = 0; i < MAX_SECUENCES; i++) {
      Serial.printf("# Secuence_%d\n", i);
      Serial.printf("sec_index= %d\n", pGrm.TaskSec[i].sec_index);
      Serial.printf("sec_rgb= %s\n", pGrm.TaskSec[i].sec_rgb.c_str());
      Serial.printf("sec_millis= %d\n", pGrm.TaskSec[i].sec_millis);
      Serial.printf("sec_rotation= %s\n", (pGrm.TaskSec[i].sec_rotation ? "true" : "false"));
      Serial.printf("sec_repeat n= %d\n", pGrm.TaskSec[i].sec_repeat);
    }
    Serial.println("---------------------------");
  }

  return true;
}

// OK - 20/12/23 - TEST
boolean saveProgram(taskScheduling vPgrm, boolean vPrint = false) {
  StaticJsonDocument<600> doc;
  doc["id"] = pGrm.id;
  doc["name"] = vPgrm.name;
  doc["enable"] = vPgrm.enable;
  doc["iniDate"] = R.datetimeToString(vPgrm.iniDate);
  doc["endDate"] = R.datetimeToString(vPgrm.endDate);
  for (int i = 0; i < MAX_SECUENCES; i++) {
    JsonObject obj = doc.createNestedObject("secuence_" + String(i));
    obj["sec_index"] = vPgrm.TaskSec[i].sec_index;
    obj["sec_rgb"] = vPgrm.TaskSec[i].sec_rgb;
    obj["sec_millis"] = vPgrm.TaskSec[i].sec_millis;
    obj["sec_rotation"] = vPgrm.TaskSec[i].sec_rotation;
    obj["sec_repeat"] = vPgrm.TaskSec[i].sec_repeat;
  }
  ESP.wdtFeed();

  String nameFile = DIR_PROGRAM + '/' + vPgrm.name + ".json";
  File configFile = SPIFFS.open(nameFile, "w");
  if (!configFile) {
    Serial.println("Error opening configuration file for writing");
    return false;
  }
  serializeJson(doc, configFile);

  if (vPrint)
    Serial.println("Configuracion guardada");

  return true;
}
//--------------------------------------------------------------------------

TimeSpan timeProgram(DateTime dtIni, DateTime dtEnd, DateTime dtNOW) {
  DateTime vIni = DateTime(dtNOW.year(), dtNOW.month(), dtNOW.day(),
                           dtIni.hour(), dtIni.minute(), dtIni.second());
  DateTime vEnd = DateTime(dtNOW.year(), dtNOW.month(), dtNOW.day(),
                           dtEnd.hour(), dtEnd.minute(), dtEnd.second());
  TimeSpan vDiff = vEnd - vIni;

  if (vDiff.hours() < 0)
    vDiff = TimeSpan(1, 0, 0, 0) + vDiff;

  return vDiff;
}

boolean CompareTime(DateTime dtIni, DateTime dtEnd) {
  DateTime NOW = R.Now();
  DateTime vIni = DateTime(NOW.year(), NOW.month(), NOW.day(),
                           dtIni.hour(), dtIni.minute(), dtIni.second());
  DateTime vEnd = DateTime(NOW.year(), NOW.month(), NOW.day(),
                           dtEnd.hour(), dtEnd.minute(), dtEnd.second());
  TimeSpan vDiff = vEnd - vIni;
  return vDiff.totalseconds() > 0;
}

taskScheduling FindScheduleFile(boolean vPrint = false) {
  DateTime NOW = R.Now();
  int countfile = 0;
  taskScheduling pTemp;

  Dir files = SPIFFS.openDir(DIR_PROGRAM);

  Serial.printf("Find schedule\tNOW= %s\n", R.datetimeToString(NOW).c_str());

  while (files.next()) {
    if (readProgram(files.fileName(), vPrint)) {
      countfile++;

      if (!pGrm.enable) {
        Serial.printf("--- # %d\nname= %s\t (disabled)", countfile, pGrm.name.c_str());
        break;
      }

      boolean Time_enable = false;
      boolean Time_disable = false;

      // Duracion de la Tarea
      int Task_duration = R.duration(pGrm.iniDate, pGrm.endDate);
      // Tiempo faltante para aplicar la Tarea
      TimeSpan Missing_time = timeProgram(pGrm.iniDate, pGrm.endDate, NOW);
      //La tarea es actual?
      boolean Valid_date = (pGrm.iniDate <= NOW) && (pGrm.endDate >= NOW);

      // Time - Inicio de tarea
      DateTime vIniTime = DateTime(NOW.year(), NOW.month(), NOW.day(),
                                   pGrm.iniDate.hour(), pGrm.iniDate.minute(), pGrm.iniDate.second());

      // Time - Fin de tarea
      DateTime vEndTime = DateTime(NOW.year(), NOW.month(), NOW.day(),
                                   pGrm.endDate.hour(), pGrm.endDate.minute(), pGrm.endDate.second());

      // La Tarea continua al dia siguiente?
      TimeSpan diff = vEndTime - vIniTime; 
      TimeSpan tsDiffEnable = vIniTime - NOW;
      TimeSpan tsDiffDisable = vEndTime - NOW;

      //Date_enable = 2020/01/01 01:01:01 - 2020/12/31 17:59:59 = OK
      //Date_enable = 2020/12/01 15:00:00 - 2020/12/01 01:00:00 = VER      
      if (diff.totalseconds() >= 0) {
        Serial.printf("TEST #0- (diff >= 0) = %d\t", diff.totalseconds());
        Time_enable = Valid_date && tsDiffEnable.totalseconds() < 0;
        Time_disable = Valid_date && tsDiffDisable.totalseconds() < 0;
      }
      else {
        //--VER--------------------------------------------------------------------------
        TimeSpan diffEndTime = NOW - vEndTime;
        TimeSpan diffIniTime = NOW - vIniTime;
        
        Serial.printf("TEST #1- (diff < 0) = %d\n", diff.totalseconds());
        Serial.printf("now - vIniTime = diffIniTime\t");
        Serial.printf("%s - %s = %d\n", R.datetimeToString(NOW).c_str(), R.datetimeToString(vIniTime).c_str(), diffIniTime.totalseconds());
        Serial.printf("now - vEndTime = diffEndTime\t");
        Serial.printf("%s - %s = %d\n", R.datetimeToString(NOW).c_str(), R.datetimeToString(vEndTime).c_str(), diffEndTime.totalseconds());

        if (Valid_date && NOW >= diffIniTime.totalseconds() && NOW <= diffEndTime.totalseconds()) {
          Serial.println("TEST #1.2.1- Entre ini y end");
          Time_enable = false;
          Time_disable = true;
        }
        else {
          Serial.println("TEST #1.2.2- No esta dentro de ini y end");
          Time_enable = true;
          Time_disable = false;
        }
      }

      Serial.printf("--- # %d\n", countfile);
      Serial.printf("name= %s\t", pGrm.name.c_str());
      Serial.printf("[%s - %s]\n", R.datetimeToString(pGrm.iniDate).c_str(), R.datetimeToString(pGrm.endDate).c_str());
      Serial.printf("valid= %s\tduration= %s\n", (Valid_date ? "Yes" : "No"), R.toStringTimeSpan(Task_duration).c_str());
      Serial.printf("%d & !%d=%d\t", Time_enable, Time_disable, (Time_enable & !Time_disable));
      Serial.printf("period= %s\n", R.toStringTimeSpan(Missing_time.totalseconds()).c_str());
      Serial.printf("\t\ttime to run= %s\n", R.toStringTimeSpan(tsDiffEnable.totalseconds()).c_str());

      if (Time_enable & !Time_disable) {
        if (pTemp.id == 0) {
          pTemp = pGrm;
          Serial.printf("TEST #2.1- Default= %s\n", pTemp.name.c_str());
        }
        else if (CompareTime(pTemp.iniDate, pGrm.iniDate)) {
          pTemp = pGrm;
          Serial.printf("TEST #2.2- Compare= %s\n", pTemp.name.c_str());
        }
        else {
          //pTemp = taskScheduling();
          Serial.printf("TEST #2.3- None= %s\n", pTemp.name.c_str());
        }
      }
    }
  }

  //pGrm = pTemp;
  taskScheduling pSelect;
  pSelect.id = pTemp.id;
  pSelect.name = pTemp.name;
  pSelect.iniDate = pTemp.iniDate;
  pSelect.endDate = pTemp.endDate;
  for (int i = 0; i < MAX_SECUENCES; i++) {
    pSelect.TaskSec[i].sec_index = pTemp.TaskSec[i].sec_index;
    pSelect.TaskSec[i].sec_rgb = pTemp.TaskSec[i].sec_rgb;
    pSelect.TaskSec[i].sec_millis = pTemp.TaskSec[i].sec_millis;
    pSelect.TaskSec[i].sec_rotation = pTemp.TaskSec[i].sec_rotation;
    pSelect.TaskSec[i].sec_repeat = pTemp.TaskSec[i].sec_repeat;
  }

  Serial.printf("RUN=  %s\n", pSelect.name.c_str());
  return pSelect;
}

//--------------------------------------------------------------------------

void help() {
  Serial.printf("SECMODE - Help\nModos de secuencia\n");
  Serial.printf("ID= 1, Name= %s\n", BLINK);
  Serial.printf("ID= 2, Name= %s\n", EVEN_OR_ODD);
  Serial.printf("ID= 3, Name= %s\n", MOBILE_POINT);
  Serial.printf("ID= 4, Name= %s\n", MOBILE_HOLLOW);
  Serial.printf("ID= 5, Name= %s\n", POSITIVE_LADDER);
  Serial.printf("ID= 6, Name= %s\n", NEGATIVE_LADDER);
  Serial.printf("ID= 7, Name= %s\n", ON);
  Serial.printf("ID= 8, Name= %s\n", OFF);
  Serial.printf("Modificador= %s\n", SEC_INV);
  Serial.println("help       \t - Manual de intrucciones.\n");
}

void Secuences_Menu(String command, String parameters) {
  Execute.rotation = NORMAL;
  if (parameters.length() > 0 & parameters == SEC_INV)
    Execute.rotation = INVERTIDO;
  if (command == BLINK | command == "1")
    NowScheduledTask.sec_select = 1;
  else if (command == EVEN_OR_ODD | command == "2")
    NowScheduledTask.sec_select = 2;
  else if (command == MOBILE_POINT | command == "3")
    NowScheduledTask.sec_select = 3;
  else if (command == MOBILE_HOLLOW | command == "4")
    NowScheduledTask.sec_select = 4;
  else if (command == POSITIVE_LADDER | command == "5")
    NowScheduledTask.sec_select = 5;
  else if (command == NEGATIVE_LADDER | command == "6")
    NowScheduledTask.sec_select = 6;
  else if (command == ON | command == "7")
    NowScheduledTask.sec_select = 7;
  else if (command == OFF | command == "8")
    NowScheduledTask.sec_select = 8;
  else if (command == "help")
    help();
  else
    Serial.println("SECMODE - Invalid command. Type help");
}

void sec_execute(boolean vPrint = false) {
  switch (NowScheduledTask.sec_select) {
    case 1: {
        sec_blink(vPrint);
        break;
      }
    case 2: {
        sec_even_or_odd(vPrint);
        break;
      }
    case 3: {
        sec_mobile_point(vPrint);
        break;
      }
    case 4: {
        sec_mobile_hollow(vPrint);
        break;
      }
    case 5: {
        sec_positive_ladder(vPrint);
        break;
      }
    case 6: {
        sec_negative_ladder(vPrint);
        break;
      }
    case 7: {
        sec_on(vPrint);
        break;
      }
    case 8: {
        sec_off(vPrint);
        break;
      }
    default: {
        break;
      }
  }
}

// OK - 20/12/24
long executeNextProgramming(taskScheduling pGrm, boolean vPrint = false) {
  if (pGrm.id != 0) {
    libColor Color;
    Color.begin(pinRED, pinGREEN, pinBLUE);

    // Cantidad de secuencias validas
    int maxSelect = 0;
    for (int i = 0; i < MAX_SECUENCES; i++) {
      if (pGrm.TaskSec[i].sec_index != 0)
        maxSelect++;
    }

    // Finalizo la ejecucion de una secuencia
    if (Execute.is_completed) {
      Execute.repeat_counter++;

      // La secuencia finalizo las repeticiones?
      if (Execute.repeat_counter > NowScheduledTask.sec_repeat) {
        NowScheduledTask.sec_index = NowScheduledTask.sec_index < maxSelect ? NowScheduledTask.sec_index + 1 : 1;
        Execute.repeat_counter = 0;
      }
    }

    NowScheduledTask.sec_select = pGrm.TaskSec[NowScheduledTask.sec_index].sec_index;
    NowScheduledTask.sec_Rgb = pGrm.TaskSec[NowScheduledTask.sec_index].sec_rgb;
    NowScheduledTask.sec_timeMillis = pGrm.TaskSec[NowScheduledTask.sec_index].sec_millis;
    NowScheduledTask.sec_rotation = pGrm.TaskSec[NowScheduledTask.sec_index].sec_rotation;
    NowScheduledTask.sec_repeat = pGrm.TaskSec[NowScheduledTask.sec_index].sec_repeat;

    Color.Rgb = NowScheduledTask.sec_Rgb;
    Execute.rotation = NowScheduledTask.sec_rotation;

    if (DEBUG) {
      float total_progress = maxSelect * NowScheduledTask.sec_repeat;
      float now_progress = NowScheduledTask.sec_index * (Execute.repeat_counter + 1);
      float exec_percent = (now_progress * 100) / total_progress;

      Serial.println("---");
      Serial.printf("#0 Progress: %.1f%s\n", exec_percent, INV_POLARITY == 1 ? ", logic denied" : "");
      Serial.printf("#1 Task: %s, Sec_ID: %d", pGrm.name.c_str(), NowScheduledTask.sec_select);
      Serial.printf(", Item: %d/%d", NowScheduledTask.sec_index, maxSelect);
      Serial.printf(", Time: %dms", NowScheduledTask.sec_timeMillis);
      Serial.printf(", Repeat: %d/%d", Execute.repeat_counter + 1, NowScheduledTask.sec_repeat);
      Serial.printf(", Rotate: %s\n", NowScheduledTask.sec_rotation ? "true" : "false");
    }

    Color.println();
    sec_execute(vPrint);
  }

  return NowScheduledTask.sec_timeMillis;
}

void sec_reset() {
  out_clear();
  write_74HC595(Execute.array_IC);
  //
  libColor Color;
  Color.begin(pinRED, pinGREEN, pinBLUE);
  Color.Rgb = "000000";
}

void sec_pause() {
  //out_set_value(0);
  //out_clear();
  //write_74HC595(Execute.array_IC);
  //
  //libColor Color;
  //Color.begin(pinRED, pinGREEN, pinBLUE);
  //Color.Rgb = "000000";
}
