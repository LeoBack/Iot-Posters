
#define NORMAL 0
#define INVERTIDO 1

struct Execute_sec
{
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

struct NOW_scheduledTask
{
  unsigned int sec_select = 0;   // Secuencia seleccionada
  unsigned int sec_index = 1;    // Secuencia a ejecutar.
  String sec_Rgb = "000000";     // Color de toda la secuencia.
  long sec_timeMillis = 100;     // Tiempo de cada paso.
  unsigned int sec_rotation = 0; // Invierte o no el sentido de ejecucion.
  unsigned int sec_repeat = 0;   // Cantidad de veces que se debe ejecutar.
};

struct taskScheduling
{
  String name = "NameDefault";
  unsigned int enable = 1;
  DateTime iniDate = DateTime(2020, 1, 1);
  DateTime endDate = DateTime(2030, 12, 30);
  unsigned int sec1_index = 0;
  String sec1_rgb = "000000";
  unsigned int sec1_millis = 0;
  unsigned int sec1_rotation = 0;
  unsigned int sec1_repeat = 1;
  unsigned int sec2_index = 0;
  String sec2_rgb = "000000";
  unsigned int sec2_millis = 0;
  unsigned int sec2_rotation = 0;
  unsigned int sec2_repeat = 1;
  unsigned int sec3_index = 0;
  String sec3_rgb = "000000";
  unsigned int sec3_millis = 0;
  unsigned int sec3_rotation = 0;
  unsigned int sec3_repeat = 1;
  unsigned int sec4_index = 0;
  String sec4_rgb = "000000";
  unsigned int sec4_millis = 0;
  unsigned int sec4_rotation = 0;
  unsigned int sec4_repeat = 1;
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
void setup_sec()
{
  pinMode(pinData, OUTPUT);
  pinMode(pinClock, OUTPUT);
  pinMode(pinRegister, OUTPUT);
  digitalWrite(pinRegister, LOW);
}

//---Funciones-------------------------------------------------------

// OK
void print_debug() {
  if (DEBUG) {
    Serial.print("#3 Sec.: "); Serial.print(Execute.name);
    Serial.print(", Byte: "); Serial.print(Execute.index_Byte);
    Serial.print(", Toggle: "); Serial.print(Execute.toggle);
    Serial.print(", Value: ");
    for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++) {
      Serial.print("0x");
      Serial.print(Execute.array_IC[index_IC], HEX);
      if (index_IC < IC_NUMBER - 1)
        Serial.print(" ");
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
  byte value = 0x00; //INV_POLARITY ? 0xFF : 0x00;
  for (int index_IC = 0; index_IC < IC_NUMBER; index_IC++)
    Execute.array_IC[index_IC] = value;
  return value;
}

// OK
byte out_full() {
  byte value = 0xFF; //INV_POLARITY ? 0x00 : 0xFF;
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
void sec_off(boolean vPrint = false)
{
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
void sec_on(boolean vPrint = false)
{
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

boolean readProgramHeader(String vName, boolean vPrint = false)
{
  File configFile = SPIFFS.open(vName, "r");
  if (!configFile)
  {
    Serial.println("Failed to open file");
    return false;
  }

  if (configFile.size() > 1024)
  {
    Serial.println("Config file size is too large");
    return false;
  }

  ESP.wdtFeed();

  StaticJsonDocument<800> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.println("Error deserializing file");
    return false;
  }

  const char *name = doc["name"];
  const unsigned int enable = doc["enable"];
  const char *iniDate = doc["iniDate"];
  const char *endDate = doc["endDate"];
  pGrm.name = name;
  pGrm.enable = enable;
  pGrm.iniDate = R.stringToDatetime(iniDate);
  pGrm.endDate = R.stringToDatetime(endDate);

  if (vPrint)
  {
    Serial.println("--- Configuracion leida ---");
    Serial.print("name= ");
    Serial.println(pGrm.name);
    Serial.print("enable= ");
    Serial.println(pGrm.enable);
    Serial.print("iniDate= ");
    Serial.println(R.datetimeToString(pGrm.iniDate));
    Serial.print("endDate= ");
    Serial.println(R.datetimeToString(pGrm.endDate));
    Serial.println("---------------------------");
  }

  return true;
}

boolean readProgram(String vName, boolean vPrint = false)
{
  File configFile = SPIFFS.open(vName, "r");
  if (!configFile)
  {
    Serial.println("Failed to open file");
    return false;
  }

  if (configFile.size() > 1024)
  {
    Serial.println("Config file size is too large");
    return false;
  }

  StaticJsonDocument<800> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  if (error)
  {
    Serial.println("Error deserializing file");
    return false;
  }

  const char *name = doc["name"];
  const unsigned int enable = doc["enable"];
  const char *iniDate = doc["iniDate"];
  const char *endDate = doc["endDate"];
  pGrm.name = name;
  pGrm.enable = enable;
  pGrm.iniDate = R.stringToDatetime(iniDate);
  pGrm.endDate = R.stringToDatetime(endDate);

  const unsigned int sec1_index = doc["sec1_index"];
  const char *sec1_rgb = doc["sec1_rgb"];
  const unsigned int sec1_millis = doc["sec1_millis"];
  const unsigned int sec1_rotation = doc["sec1_rotation"];
  const unsigned int sec1_repeat = doc["sec1_repeat"];
  pGrm.sec1_index = sec1_index;
  pGrm.sec1_rgb = sec1_rgb;
  pGrm.sec1_millis = sec1_millis;
  pGrm.sec1_rotation = sec1_rotation;
  pGrm.sec1_repeat = sec1_repeat;

  const unsigned int sec2_index = doc["sec2_index"];
  const char *sec2_rgb = doc["sec2_rgb"];
  const unsigned int sec2_millis = doc["sec2_millis"];
  const unsigned int sec2_rotation = doc["sec2_rotation"];
  const unsigned int sec2_repeat = doc["sec2_repeat"];
  pGrm.sec2_index = sec2_index;
  pGrm.sec2_rgb = sec2_rgb;
  pGrm.sec2_millis = sec2_millis;
  pGrm.sec2_rotation = sec2_rotation;
  pGrm.sec2_repeat = sec2_repeat;

  const unsigned int sec3_index = doc["sec3_index"];
  const char *sec3_rgb = doc["sec3_rgb"];
  const unsigned int sec3_millis = doc["sec3_millis"];
  const unsigned int sec3_rotation = doc["sec3_rotation"];
  const unsigned int sec3_repeat = doc["sec3_repeat"];
  pGrm.sec3_index = sec3_index;
  pGrm.sec3_rgb = sec3_rgb;
  pGrm.sec3_millis = sec3_millis;
  pGrm.sec3_rotation = sec3_rotation;
  pGrm.sec3_repeat = sec3_repeat;

  const unsigned int sec4_index = doc["sec4_index"];
  const char *sec4_rgb = doc["sec4_rgb"];
  const unsigned int sec4_millis = doc["sec4_millis"];
  const unsigned int sec4_rotation = doc["sec4_rotation"];
  const unsigned int sec4_repeat = doc["sec4_repeat"];
  pGrm.sec4_index = sec4_index;
  pGrm.sec4_rgb = sec4_rgb;
  pGrm.sec4_millis = sec4_millis;
  pGrm.sec4_rotation = sec4_rotation;
  pGrm.sec4_repeat = sec4_repeat;

  if (vPrint)
  {
    Serial.println("--- Configuracion leida ---");
    Serial.print("name= ");
    Serial.println(pGrm.name);
    Serial.print("enable= ");
    Serial.println(pGrm.enable);
    Serial.print("iniDate= ");
    Serial.println(R.datetimeToString(pGrm.iniDate));
    Serial.print("endDate= ");
    Serial.println(R.datetimeToString(pGrm.endDate));
    Serial.print("sec1_index= ");
    Serial.println(pGrm.sec1_index);
    Serial.print("sec1_rgb= ");
    Serial.println(pGrm.sec1_rgb);
    Serial.print("sec1_millis= ");
    Serial.println(pGrm.sec1_millis);
    Serial.print("sec1_rotation= ");
    Serial.println(pGrm.sec1_rotation);
    Serial.print("sec1_repeat= ");
    Serial.println(pGrm.sec1_repeat);
    Serial.print("sec2_index= ");
    Serial.println(pGrm.sec2_index);
    Serial.print("sec2_rgb= ");
    Serial.println(pGrm.sec2_rgb);
    Serial.print("sec2_millis= ");
    Serial.println(pGrm.sec2_millis);
    Serial.print("sec2_rotation= ");
    Serial.println(pGrm.sec2_rotation);
    Serial.print("sec2_repeat= ");
    Serial.println(pGrm.sec2_repeat);
    Serial.print("sec3_index= ");
    Serial.println(pGrm.sec3_index);
    Serial.print("sec3_rgb= ");
    Serial.println(pGrm.sec3_rgb);
    Serial.print("sec3_millis= ");
    Serial.println(pGrm.sec3_millis);
    Serial.print("sec3_rotation= ");
    Serial.println(pGrm.sec3_rotation);
    Serial.print("sec3_repeat= ");
    Serial.println(pGrm.sec3_repeat);
    Serial.print("sec4_index= ");
    Serial.println(pGrm.sec4_index);
    Serial.print("sec4_rgb= ");
    Serial.println(pGrm.sec4_rgb);
    Serial.print("sec4_millis= ");
    Serial.println(pGrm.sec4_millis);
    Serial.print("sec4_rotation= ");
    Serial.println(pGrm.sec4_rotation);
    Serial.print("sec4_repeat= ");
    Serial.println(pGrm.sec4_repeat);
    Serial.println("---------------------------");
  }

  return true;
}

boolean saveProgram(taskScheduling vPgrm, boolean vPrint = false) {
  StaticJsonDocument<700> doc;

  doc["name"] = vPgrm.name;
  doc["enable"] = vPgrm.enable;
  doc["iniDate"] = R.datetimeToString(vPgrm.iniDate);
  doc["endDate"] = R.datetimeToString(vPgrm.endDate);

  doc["sec1_index"] = vPgrm.sec1_index;
  doc["sec1_rgb"] = vPgrm.sec1_rgb;
  doc["sec1_millis"] = vPgrm.sec1_millis;
  doc["sec1_rotation"] = vPgrm.sec1_rotation;
  doc["sec1_repeat"] = vPgrm.sec1_repeat;

  doc["sec2_index"] = vPgrm.sec2_index;
  doc["sec2_rgb"] = vPgrm.sec2_rgb;
  doc["sec2_millis"] = vPgrm.sec2_millis;
  doc["sec2_rotation"] = vPgrm.sec2_rotation;
  doc["sec2_repeat"] = vPgrm.sec2_repeat;

  doc["sec3_index"] = vPgrm.sec3_index;
  doc["sec3_rgb"] = vPgrm.sec3_rgb;
  doc["sec3_millis"] = vPgrm.sec3_millis;
  doc["sec3_rotation"] = vPgrm.sec3_rotation;
  doc["sec3_repeat"] = vPgrm.sec3_repeat;

  doc["sec4_index"] = vPgrm.sec4_index;
  doc["sec4_rgb"] = vPgrm.sec4_rgb;
  doc["sec4_millis"] = vPgrm.sec4_millis;
  doc["sec4_rotation"] = vPgrm.sec4_rotation;
  doc["sec4_repeat"] = vPgrm.sec4_repeat;

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

struct Temp {
  String secName = "";
  long duration = 0;
  DateTime iniDate = DateTime(2020, 1, 1);
  DateTime endDate = DateTime(2030, 12, 30);
};

Temp temp;

boolean readTemp(boolean vPrint = false)
{
  File tempFile = SPIFFS.open(TEMP, "r");
  if (!tempFile) {
    Serial.println("Failed to open file");
    return false;
  }
  if (tempFile.size() > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }

  StaticJsonDocument<800> doc;
  DeserializationError error = deserializeJson(doc, tempFile);
  if (error) {
    Serial.println("Error deserializing file");
    return false;
  }

  const char *secName = doc["secName"];
  const long duration = doc["duration"];
  const char *iniDate = doc["iniDate"];
  const char *endDate = doc["endDate"];
  temp.secName = secName;
  temp.duration = duration;
  temp.iniDate = R.stringToDatetime(iniDate);
  temp.endDate = R.stringToDatetime(endDate);

  if (vPrint) {
    Serial.println("--- Temporal leido ---");
    Serial.print("secName= ");
    Serial.println(temp.secName);
    Serial.print("duration= ");
    Serial.println(temp.duration);
    Serial.print("iniDate= ");
    Serial.println(R.datetimeToString(temp.iniDate));
    Serial.print("endDate= ");
    Serial.println(R.datetimeToString(temp.endDate));
    Serial.println("---------------------------");
  }

  return true;
}

boolean saveTemp(boolean vPrint = false)
{
  StaticJsonDocument<700> doc;
  doc["secName"] = temp.secName;
  doc["duration"] = temp.duration;
  doc["iniDate"] = R.datetimeToString(temp.iniDate);
  doc["endDate"] = R.datetimeToString(temp.endDate);

  File tempFile = SPIFFS.open(TEMP, "w");
  if (!tempFile){
    Serial.println("Error opening configuration file for writing");
    return false;
  }
  serializeJson(doc, tempFile);

  if (vPrint)
    Serial.println("Temporal guardado");

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

  Serial.print("Read files:\tNOW= ");
  Serial.println(R.datetimeToString(NOW));

  while (files.next())
  {
    if (readProgram(files.fileName(), vPrint) && pGrm.enable)
    {
      countfile++;

      boolean EnableTime = false;
      boolean DisableTime = false;

      // Duracion de la Tarea
      int duration = R.duration(pGrm.iniDate, pGrm.endDate);

      // Tiempo faltante para aplicar la Tarea
      TimeSpan timeRunTask = timeProgram(pGrm.iniDate, pGrm.endDate, NOW);

      // La Tarea es actual (Comparacion entre Fechas)
      boolean EnableDate = (pGrm.iniDate <= NOW) && (pGrm.endDate >= NOW);

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

      if (diff.totalseconds() > 0)
      {
        // EnableTime (Compara horario) - Etapa I
        EnableTime = EnableDate && tsDiffEnable.totalseconds() < 0;

        // DisableTime (Compara horario) - Etapa I
        DisableTime = EnableDate && tsDiffDisable.totalseconds() < 0;
      }
      else
      {
        //--VER--------------------------------------------------------------------------
        Serial.print("TEST #1- (diff < 0)\t"); Serial.println(diff.totalseconds());

        TimeSpan diffEndTime = NOW - vEndTime;
        TimeSpan diffIniTime = NOW - vIniTime;

        Serial.print("TEST #1.2- (now - vIniTime = diffIniTime)\t");
        Serial.print(R.datetimeToString(NOW));
        Serial.print(" - "); Serial.print(R.datetimeToString(vIniTime));
        Serial.print(" = "); Serial.println(diffIniTime.totalseconds());

        Serial.print("TEST #1.3- (now - vEndTime = diffEndTime)\t");
        Serial.print(R.datetimeToString(NOW));
        Serial.print(" - "); Serial.print(R.datetimeToString(vEndTime));
        Serial.print(" = "); Serial.println(diffEndTime.totalseconds());

        if (EnableDate && NOW >= diffIniTime.totalseconds() && NOW <= diffEndTime.totalseconds()) {
          Serial.println("TEST #1.4- Entre ini y end");
          EnableTime = false;
          DisableTime = true;
        }
        else {
          Serial.println("TEST #1.4- No esta dentro de ini y end");
          EnableTime = true;
          DisableTime = false;
        }
      }

      // Imprimo
      Serial.print("# "); Serial.print(countfile);
      Serial.print(" = "); Serial.print(pGrm.name);
      Serial.print("\t["); Serial.print(R.datetimeToString(pGrm.iniDate));
      Serial.print(" - "); Serial.print(R.datetimeToString(pGrm.endDate));
      Serial.println("]");
      Serial.print("Valid= "); Serial.print(EnableDate ? "Yes" : "No");
      Serial.print("\tPeriod= "); Serial.println(R.toStringTimeSpan(duration));
      Serial.print(EnableTime); Serial.print(" & !"); Serial.print(DisableTime);
      Serial.print(" = "); Serial.print(EnableTime & !DisableTime);
      Serial.print("\tDuration= "); Serial.println(R.toStringTimeSpan(timeRunTask.totalseconds()));
      Serial.print("\t\tTime to run= "); Serial.println(R.toStringTimeSpan(tsDiffEnable.totalseconds()));

      if (EnableTime & !DisableTime){
        if (pTemp.name == "NameDefault") {
          pTemp = pGrm;
          Serial.print("TEST #2.1- Default=");
          Serial.println(pTemp.name);
        }
        else if (CompareTime(pTemp.iniDate, pGrm.iniDate)) {
          pTemp = pGrm;
          Serial.print("TEST #2.2- Compare= ");
          Serial.println(pTemp.name);
        }
        else {
          //pTemp = taskScheduling();
          Serial.print("TEST #2.3- None= ");
          Serial.println(pTemp.name);
        }
      }
    }
  }

  pGrm = pTemp;
  Serial.print("Is RUN= ");
  Serial.println(pGrm.name);

  taskScheduling pSelect;
  pSelect.name = pGrm.name;
  pSelect.iniDate = pGrm.iniDate;
  pSelect.endDate = pGrm.endDate;

  pSelect.sec1_index = pGrm.sec1_index;
  pSelect.sec1_rgb = pGrm.sec1_rgb;
  pSelect.sec1_millis = pGrm.sec1_millis;
  pSelect.sec1_rotation = pGrm.sec1_rotation;
  pSelect.sec1_repeat = pGrm.sec1_repeat;

  pSelect.sec2_index = pGrm.sec2_index;
  pSelect.sec2_rgb = pGrm.sec2_rgb;
  pSelect.sec2_millis = pGrm.sec2_millis;
  pSelect.sec2_rotation = pGrm.sec2_rotation;
  pSelect.sec2_repeat = pGrm.sec2_repeat;

  pSelect.sec3_index = pGrm.sec3_index;
  pSelect.sec3_rgb = pGrm.sec3_rgb;
  pSelect.sec3_millis = pGrm.sec3_millis;
  pSelect.sec3_rotation = pGrm.sec3_rotation;
  pSelect.sec3_repeat = pGrm.sec3_repeat;

  pSelect.sec4_index = pGrm.sec4_index;
  pSelect.sec4_rgb = pGrm.sec4_rgb;
  pSelect.sec4_millis = pGrm.sec4_millis;
  pSelect.sec4_rotation = pGrm.sec4_rotation;
  pSelect.sec4_repeat = pGrm.sec4_repeat;

  return pSelect;
}

//--------------------------------------------------------------------------

void help() {
  Serial.println("SECMODE - Help");
  Serial.println("Modos de secuencia");
  Serial.print("ID= 1, Name= "); Serial.println(BLINK);
  Serial.print("ID= 2, Name= "); Serial.println(EVEN_OR_ODD);
  Serial.print("ID= 3, Name= "); Serial.println(MOBILE_POINT);
  Serial.print("ID= 4, Name= "); Serial.println(MOBILE_HOLLOW);
  Serial.print("ID= 5, Name= "); Serial.println(POSITIVE_LADDER);
  Serial.print("ID= 6, Name= "); Serial.println(NEGATIVE_LADDER);
  Serial.print("ID= 7, Name= "); Serial.println(ON);
  Serial.print("ID= 8, Name= "); Serial.println(OFF);
  Serial.print("Modificador= "); Serial.println(SEC_INV);
  Serial.println("help       \t - Manual de intrucciones.");
  Serial.println();
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

long executeNextProgramming(taskScheduling pGrm, boolean vPrint = false) {
  if (pGrm.name != "NameDefault") {
    libColor Color;
    Color.begin(pinRED, pinGREEN, pinBLUE);
    int maxSelect = 0;
    maxSelect = pGrm.sec1_index != 0 ? maxSelect + 1 : maxSelect;
    maxSelect = pGrm.sec2_index != 0 ? maxSelect + 1 : maxSelect;
    maxSelect = pGrm.sec3_index != 0 ? maxSelect + 1 : maxSelect;
    maxSelect = pGrm.sec4_index != 0 ? maxSelect + 1 : maxSelect;

    if (Execute.is_completed) {
      Execute.repeat_counter++;
      if (!(Execute.repeat_counter < NowScheduledTask.sec_repeat)) {
        NowScheduledTask.sec_index = NowScheduledTask.sec_index < maxSelect ? NowScheduledTask.sec_index + 1 : 1;
        Execute.repeat_counter = 0;
      }
    }

    switch (NowScheduledTask.sec_index) {
      case 1: {
          NowScheduledTask.sec_select = pGrm.sec1_index;
          NowScheduledTask.sec_Rgb = pGrm.sec1_rgb;
          NowScheduledTask.sec_timeMillis = pGrm.sec1_millis;
          NowScheduledTask.sec_rotation = pGrm.sec1_rotation;
          NowScheduledTask.sec_repeat = pGrm.sec1_repeat;
          break;
        }
      case 2: {
          NowScheduledTask.sec_select = pGrm.sec2_index;
          NowScheduledTask.sec_Rgb = pGrm.sec2_rgb;
          NowScheduledTask.sec_timeMillis = pGrm.sec2_millis;
          NowScheduledTask.sec_rotation = pGrm.sec2_rotation;
          NowScheduledTask.sec_repeat = pGrm.sec2_repeat;
          break;
        }
      case 3: {
          NowScheduledTask.sec_select = pGrm.sec3_index;
          NowScheduledTask.sec_Rgb = pGrm.sec3_rgb;
          NowScheduledTask.sec_timeMillis = pGrm.sec3_millis;
          NowScheduledTask.sec_rotation = pGrm.sec3_rotation;
          NowScheduledTask.sec_repeat = pGrm.sec3_repeat;
          break;
        }
      case 4: {
          NowScheduledTask.sec_select = pGrm.sec4_index;
          NowScheduledTask.sec_Rgb = pGrm.sec4_rgb;
          NowScheduledTask.sec_timeMillis = pGrm.sec4_millis;
          NowScheduledTask.sec_rotation = pGrm.sec4_rotation;
          NowScheduledTask.sec_repeat = pGrm.sec4_repeat;
          break;
        }
      default: {
          NowScheduledTask.sec_select = 0;
          NowScheduledTask.sec_Rgb = "000000";
          NowScheduledTask.sec_timeMillis = 0;
          NowScheduledTask.sec_rotation = 0;
          NowScheduledTask.sec_repeat = 1;
          break;
        }
    }

    Color.Rgb = NowScheduledTask.sec_Rgb;
    Execute.rotation = NowScheduledTask.sec_rotation;

    if (DEBUG) {
      float total_progress = maxSelect * NowScheduledTask.sec_repeat;
      float now_progress = NowScheduledTask.sec_index * (Execute.repeat_counter + 1);
      float exec_percent = (now_progress * 100) / total_progress;

      Serial.println("---");
      Serial.print("#0 Progress: "); Serial.print(exec_percent); Serial.print("%");
      Serial.print(" Logic denied: "); Serial.println(INV_POLARITY);
      Serial.print("#1 Task: "); Serial.print(pGrm.name);
      Serial.print(", Sec_ID: "); Serial.print(NowScheduledTask.sec_select);
      Serial.print(", Item: "); Serial.print(NowScheduledTask.sec_index);
      Serial.print("/"); Serial.print(maxSelect);
      Serial.print(", Time: "); Serial.print(NowScheduledTask.sec_timeMillis);
      Serial.print("ms, Repeat: "); Serial.print(Execute.repeat_counter + 1);
      Serial.print("/"); Serial.print(NowScheduledTask.sec_repeat);
      Serial.print(", Rotate: "); Serial.println(NowScheduledTask.sec_rotation);
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
