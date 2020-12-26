/*
   2020/09/19 - Creacion de la libreria.


*/

class libRTC {
  private:
    RTC_DS3231 rtc;
    String buf1 = "YYYY/MM/DD hh:mm:ss";
    char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    int vYear;
    int vMonth;
    int vDay;
    int vHour;
    int vMinute;
    int vSecond;
    int vDayOfTheWeek;

  public:
    enum eSetDatetime { SET_ALL, SET_DATE, SET_TIME };
    enum eDate { DATE, YEAR, MONTH, DAY };
    enum eTime { TIME, HOUR, MINUTE, SECONDS };

    void begin();
    void readRTC();
    String datetimeToString(DateTime dt);
    String datetimeToString_ISO(DateTime);
    DateTime stringToDatetime(String DatetimeString);
    boolean setDatetime(String vDate, String vTime);
    void setDatetime(String nSeccion[], int parameterSize);
    String getDatetime();
    DateTime Now();
    int getDate(eDate seccion);
    int getTime(eTime seccion);
    float getTemp();
    void help();
    void menu(String command, String parameters);
    String toStringTimeSpan(TimeSpan tmDiff);
    long duration(DateTime dtIni, DateTime dtEnd);

    boolean isTimeExecute(DateTime iniDt, DateTime endDt, DateTime nowDt);
    boolean isCompare(DateTime iniDt, DateTime endDt);
};

void libRTC::begin()
{
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));

    // When time needs to be re-set on a previously configured device, the
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

/*
   Carga almacen temporal de tiempo.
   OK - 20/11/16
*/
void libRTC::readRTC() {
  DateTime now = rtc.now();
  vYear = now.year();
  vMonth = now.month();
  vDay = now.day();
  vHour = now.hour();
  vMinute = now.minute();
  vSecond = now.second();
  vDayOfTheWeek = now.dayOfTheWeek();
}

/*
   Tipo Fecha a String.
   OK - 20/11/16
*/
String libRTC::datetimeToString(DateTime dt) {
  String text = (String)dt.year(); text.concat("/");
  text.concat(dt.month()); text.concat("/");
  text.concat(dt.day()); text.concat(" ");
  text.concat(dt.hour()); text.concat(":");
  text.concat(dt.minute()); text.concat(":");
  text.concat(dt.second());
  return text;
}

/*
   Tipo Fecha a String.
   OK - 20/11/16
*/
String libRTC::datetimeToString_ISO(DateTime dt) {
  String text = (String)dt.year(); text.concat("/");
  text.concat(dt.month()); text.concat("/");
  text.concat(dt.day()); text.concat("T");
  text.concat(dt.hour()); text.concat(":");
  text.concat(dt.minute()); text.concat(":");
  text.concat(dt.second());
  return text;
}

/*
   Tipo String a Fecha.
   OK - 20/11/16
*/
DateTime libRTC::stringToDatetime(String DatetimeString) {
  String year = DatetimeString.substring(0, DatetimeString.indexOf("-"));
  String month = DatetimeString.substring(DatetimeString.indexOf("-") + 1, DatetimeString.lastIndexOf("-"));
  String day = DatetimeString.substring(DatetimeString.lastIndexOf("-") + 1, DatetimeString.indexOf("T"));
  String hour = DatetimeString.substring(DatetimeString.indexOf("T") + 1, DatetimeString.indexOf(":"));
  String minute = DatetimeString.substring(DatetimeString.indexOf(":") + 1, DatetimeString.lastIndexOf(":"));
  String second = DatetimeString.substring(DatetimeString.lastIndexOf(":") + 1);
  //  Serial.println("---------------@@@@");
  //  Serial.print("Year= "); Serial.println(year);
  //  Serial.print("month= "); Serial.println(month);
  //  Serial.print("day= "); Serial.println(day);
  //  Serial.print("hour= "); Serial.println(hour);
  //  Serial.print("minute= "); Serial.println(minute);
  //  Serial.print("second= "); Serial.println(second);
  //  Serial.println("---------------@@@@");
  return DateTime (year.toInt(), month.toInt(), day.toInt(),
                   hour.toInt(), minute.toInt(), second.toInt());
}

/*
   Setea la fecha y hora del RTC.
   OK - 20/11/16
*/
boolean libRTC::setDatetime(String vDate, String vTime) {

  if (vDate == "" & vTime == "")
    return false;
  else {
    libRTC::readRTC();    
    Serial.print("SetDatetime= Date:"); Serial.print(vDate);
    Serial.print(" Time: "); Serial.println(vTime);

    if (vDate != "") {
      vDate.trim();
      int first_bar = vDate.indexOf("/");
      int seconds_bar = vDate.lastIndexOf("/");
      vDay = vDate.substring(0, first_bar).toInt();
      vMonth = vDate.substring(first_bar + 1, seconds_bar).toInt();
      vYear = vDate.substring(seconds_bar + 1, vDate.length()).toInt();
    }

    if (vTime != "") {
      vTime.trim();
      int first_point = vTime.indexOf(":");
      int seconds_point = vTime.lastIndexOf(":");
      vHour = vTime.substring(0, first_point).toInt();
      vMinute = vTime.substring(first_point + 1, seconds_point).toInt();
      vSecond = vTime.substring(seconds_point + 1, vTime.length()).toInt();
    }

    if (DEBUG) {
      Serial.print("Day: "); Serial.println(vDay);
      Serial.print("Month: "); Serial.println(vMonth);
      Serial.print("Year: "); Serial.println(vYear);
      Serial.print("Hour: "); Serial.println(vHour);
      Serial.print("Minute: "); Serial.println(vMinute);
      Serial.print("Seconds: "); Serial.println(vSecond);
    }

    DateTime newDt = DateTime(vYear, vMonth, vDay, vHour, vMinute, vSecond);
    rtc.adjust(newDt);
    
    libRTC::readRTC();
    DateTime RtcDt = DateTime(vYear, vMonth, vDay, vHour, vMinute, vSecond);
    TimeSpan A = RtcDt - newDt;
    return A.totalseconds() >= 0;
  }
}

// VER muy probable de eliminar
void libRTC::setDatetime(String nSeccion[], int parameterSize) {
  Serial.print("setDatetime ");

  if (parameterSize == 4 & nSeccion[3] == "SET_DATE") {
    Serial.println("SET_DATE");
    libRTC::readRTC();
    vYear = nSeccion[0].toInt();
    vMonth = nSeccion[1].toInt();
    vDay = nSeccion[2].toInt();
  }
  else if (parameterSize == 4 & nSeccion[3] == "SET_TIME") {
    Serial.println("SET_TIME");
    libRTC::readRTC();
    vHour = nSeccion[0].toInt();
    vMinute = nSeccion[1].toInt();
    vSecond = nSeccion[2].toInt();
  }
  else {//(parameterSize == 7 & nSeccion[6] == "SET_ALL") {
    Serial.println("SET_ALL");
    vYear = nSeccion[0].toInt();
    vMonth = nSeccion[1].toInt();
    vDay = nSeccion[2].toInt();
    vHour = nSeccion[3].toInt();
    vMinute = nSeccion[4].toInt();
    vSecond = nSeccion[5].toInt();
  }

  rtc.adjust(DateTime(vYear, vMonth, vDay, vHour, vMinute, vSecond));
}

String libRTC::getDatetime() {
  libRTC::readRTC();
  String text = "";
  text.concat(vYear); text.concat("/");
  text.concat(vMonth); text.concat("/");
  text.concat(vDay); text.concat(" ");
  text.concat(vHour); text.concat(":");
  text.concat(vMinute); text.concat(":");
  text.concat(vSecond);
  return text;
}

DateTime libRTC::Now() {
  return rtc.now();
}

int libRTC::getDate(eDate seccion) {
  libRTC::readRTC();
  String text = "";
  text.concat(vYear); text.concat("/");
  text.concat(vMonth); text.concat("/");
  text.concat(vDay); text.concat("\n");

  int value = 0;
  switch (seccion) {
    case YEAR: {
        text.concat("YEAR= ");
        value = vYear;
        break;
      }
    case MONTH: {
        text.concat("MONTH= ");
        value = vMonth;
        break;
      }
    case DAY: {
        text.concat("DAY= ");
        value = vDay;
        break;
      }
    default: {
        text.concat("DATE= ");
        break;
      }
  }
  text.concat(value);
  Serial.println(text);
  return value;
}

int libRTC::getTime(eTime seccion) {
  libRTC::readRTC();
  String text = "";
  text.concat(vHour); text.concat(":");
  text.concat(vMinute); text.concat(":");
  text.concat(vSecond); text.concat("\n");

  int value = 0;
  switch (seccion) {
    case YEAR: {
        text.concat("HOUR= ");
        value = vHour;
        break;
      }
    case MONTH: {
        text.concat("MINUTE= ");
        value = vMinute;
        break;
      }
    case DAY: {
        text.concat("SECOND= ");
        value = vSecond;
        break;
      }
    default: {
        text.concat("TIME= ");
        break;
      }
  }
  text.concat(value);
  Serial.println(text);
  return value;
}

float libRTC::getTemp() {
  float temp = rtc.getTemperature();
  Serial.print("Temperatura: ");
  Serial.print(temp);
  Serial.println(" C");
  return temp;
}

void libRTC::help() {
  Serial.printf("RTC - Help\nsetDatetime\t - Establece fecha y hora.\n");
  Serial.printf("\t\t - Formato: SET_ALL,AAAA,MM,DD,hh,mm,ss\n");
  Serial.printf("\t\t - Formato: SET_DATE,AAAA,MM,DD\n");
  Serial.printf("\t\t - Formato: SET_TIME,hh,mm,ss\n");
  Serial.printf("getDatetime\t - Devuelve fecha y hora.\n");
  Serial.printf("getDate    \t - Devuelve fecha\n");
  Serial.printf("getTime    \t - Devuelve hora\n");
  Serial.printf("getTemp    \t - Devuelve temperatura\n");
  Serial.printf("help       \t - Manual de intrucciones.\n");
}

void libRTC::menu(String command, String parameters) {
  command.toLowerCase();
  if (command == "setdatetime" & parameters.length() > 0) {
    int parameterSize = 1;
    for (int nchar = 0; nchar < parameters.length(); nchar++) {
      if (parameters[nchar] == ',') {
        parameterSize++;
      }
    }
    int nParameters = 0;
    String nameParameters[parameterSize];
    for (int nchar = 0; nchar < parameters.length(); nchar++) {
      if (parameters[nchar] == ',')
        nParameters++;
      else
        nameParameters[nParameters] += parameters[nchar];
    }
    libRTC::setDatetime(nameParameters, parameterSize);
  }
  else if (command == "getdatetime") {
    libRTC::getDatetime();
  }
  else if (command == "getdate") {
    libRTC::getDate(DATE);
  }
  else if (command == "gettime") {
    libRTC::getTime(TIME);
  }
  else if (command == "gettemp") {
    libRTC::getTemp();
  }
  else if (command == "help") {
    libRTC::help();
  }
  else {
    Serial.println("RTC - Invalid command. Type help");
  }
}

String libRTC::toStringTimeSpan(TimeSpan tmDiff) {
  return String(tmDiff.days()) + String("d ") +
         String(tmDiff.hours()) + String("h ") +
         String(tmDiff.minutes()) + String("min ") +
         String(tmDiff.seconds() + String("s"));
}

long libRTC::duration(DateTime dtIni, DateTime dtEnd) {
  TimeSpan tmDiff = dtEnd - dtIni;
  return tmDiff.totalseconds();
}

boolean libRTC::isTimeExecute(DateTime iniDt, DateTime endDt, DateTime nowDt) {
  boolean Enable = false;
  boolean Disable = false;

  //  // Metodo 1 ----------------------
  //  DateTime fN = DateTime(nowDt.year(), nowDt.month(), nowDt.day(), 0, 0, 0);
  //  DateTime fI = DateTime(iniDt.year(), iniDt.month(), iniDt.day(), 0, 0, 0);
  //  DateTime fE = DateTime(endDt.year(), endDt.month(), endDt.day(), 0, 0, 0);
  //
  //  TimeSpan fTsDuracion = fE - fI;  // Duracion de la programacion
  //  TimeSpan fTsEnable = fI - fN;    // Activar
  //  TimeSpan fTsDisable = fE - fN;   // Desactivar
  //
  //  boolean fEnable = fTsEnable.totalseconds() <= 0;
  //  boolean fDisable = fTsDisable.totalseconds() >= 0;
  //
  //  Serial.println("");
  //  Serial.print("fE: "); Serial.print(datetimeToString(fE));
  //  Serial.print("- fI: "); Serial.print(datetimeToString(fI));
  //  Serial.print(" = TsDuracion: "); Serial.println(fTsDuracion.totalseconds());
  //  //
  //  Serial.print("fI: "); Serial.print(datetimeToString(fI));
  //  Serial.print(" - fN: "); Serial.print(datetimeToString(fN));
  //  Serial.print(" = TsEnable: "); Serial.println(fTsEnable.totalseconds());
  //  //
  //  Serial.print("fE: "); Serial.print(datetimeToString(fE));
  //  Serial.print(" - fN: "); Serial.print(datetimeToString(fN));
  //  Serial.print(" = TsDisable: "); Serial.println(fTsDisable.totalseconds());
  //  //
  //  Serial.print("fEnable= "); Serial.print(fEnable);
  //  Serial.print(" fDisable= "); Serial.println(fDisable);
  //
  //  if (fEnable & fDisable) {
  //    DateTime tN = DateTime(1970, 0, 0, nowDt.hour(), nowDt.minute(), nowDt.second());
  //    DateTime tI = DateTime(1970, 0, 0, iniDt.hour(), iniDt.minute(), iniDt.second());
  //    DateTime tE = DateTime(1970, 0, 0, endDt.hour(), endDt.minute(), endDt.second());
  //
  //    TimeSpan tTsDuracion = tE - tI;  // Duracion de la programacion
  //    TimeSpan tTsEnable = tI - tN;    // Activar
  //    TimeSpan tTsDisable = tE - tN;   // Desactivar
  //
  //    Enable = tTsEnable.totalseconds() <= 0;
  //    Disable = tTsDisable.totalseconds() >= 0;
  //
  //    Serial.print("tE: "); Serial.print(datetimeToString(tE));
  //    Serial.print("- tI: "); Serial.print(datetimeToString(tI));
  //    Serial.print(" = tTsDuracion: "); Serial.println(tTsDuracion.totalseconds());
  //    //
  //    Serial.print("tI: "); Serial.print(datetimeToString(tI));
  //    Serial.print(" - tN: "); Serial.print(datetimeToString(tN));
  //    Serial.print(" = tTsEnable: "); Serial.println(tTsEnable.totalseconds());
  //    //
  //    Serial.print("tE: "); Serial.print(datetimeToString(tE));
  //    Serial.print(" - tN: "); Serial.print(datetimeToString(tN));
  //    Serial.print(" = tTsDisable: "); Serial.println(tTsDisable.totalseconds());
  //  }
  //
  //  Serial.print("Enable= "); Serial.print(Enable);
  //  Serial.print(" Disable= "); Serial.println(Disable);

  // Metodo 2 ----------------------
  //  TimeSpan tsDuracion = endDt - iniDt;  // Duracion de la programacion
  //  TimeSpan tsEnable = iniDt - nowDt;    // Activar
  //  TimeSpan tsDisable = endDt - nowDt;   // Desactivar
  //
  //  Serial.println("");
  //  //  Serial.print(tsDuracion.days()); Serial.print(" ");
  //  //  Serial.print(tsDuracion.hours()); Serial.print(":");
  //  //  Serial.print(tsDuracion.minutes()); Serial.print(":");
  //  //  Serial.print(tsDuracion.seconds()); Serial.print(" ");
  //  Serial.print("Duracion.totalseconds= "); Serial.println(tsDuracion.totalseconds());
  //  //  Serial.print(tsEnable.days()); Serial.print(" ");
  //  //  Serial.print(tsEnable.hours()); Serial.print(":");
  //  //  Serial.print(tsEnable.minutes()); Serial.print(":");
  //  //  Serial.print(tsEnable.seconds()); Serial.print(" ");
  //  Serial.print("Enable.totalseconds= "); Serial.println(tsEnable.totalseconds());
  //  //  Serial.print(tsDisable.days()); Serial.print(" ");
  //  //  Serial.print(tsDisable.hours()); Serial.print(":");
  //  //  Serial.print(tsDisable.minutes()); Serial.print(":");
  //  //  Serial.print(tsDisable.seconds()); Serial.print(" ");
  //  Serial.print("Disable.totalseconds= "); Serial.println(tsDisable.totalseconds());
  //
  //  bool Enable = tsEnable.totalseconds() <= 0;
  //  bool Disable = tsDisable.totalseconds() >= 0;
  //  Serial.print("Enable= "); Serial.println(Enable);
  //  Serial.print("Disable= "); Serial.println(Disable);

  // Metodo 3 ----------------------
  //  TimeSpan tsDuracion = endDt - iniDt;  // Duracion de la programacion
  //  TimeSpan tsEnable = iniDt - nowDt;    // Activar
  //  TimeSpan tsDisable = endDt - nowDt;   // Desactivar
  //
  //  Enable = tsEnable.totalseconds() <= 0;
  //  Disable = tsDisable.totalseconds() >= 0;
  //
  //  Serial.println("");
  //  Serial.print(tsDuracion.days()); Serial.print("Days ");
  //  Serial.print(tsDuracion.minutes()); Serial.print("Minutes ");
  //  Serial.print(tsDuracion.seconds()); Serial.println("Seconds ");
  //  //
  //  Serial.print("iniDt: "); Serial.print(datetimeToString(iniDt));
  //  Serial.print(" - nowDt: "); Serial.print(datetimeToString(nowDt));
  //  Serial.print(" = tTsEnable: "); Serial.println(tsEnable.totalseconds());
  //  //
  //  Serial.print("endDt: "); Serial.print(datetimeToString(endDt));
  //  Serial.print(" - nowDt: "); Serial.print(datetimeToString(nowDt));
  //  Serial.print(" = tTsDisable: "); Serial.println(tsDisable.totalseconds());
  //  //
  //  Serial.print("Enable= "); Serial.print(Enable);
  //  Serial.print(" Disable= "); Serial.println(Disable);

  // Metodo 4 ----------------------
  Serial.println("");

  // La fecha INICIO es igual o mayor que AHORA = SI
  if (iniDt <= nowDt) {

    // Nueva variable fecha con A/M/D de (AHORA) con la H/M/S de (INICIO)
    DateTime Dt = DateTime(nowDt.year(), nowDt.month(), nowDt.day(),
                           iniDt.hour(), iniDt.minute(), iniDt.second());
    Serial.print("Stage1 - Dt: "); Serial.println(datetimeToString(Dt));

    // Comparo HORAS
    if (Dt < nowDt) {
      DateTime Dt = DateTime(nowDt.year(), nowDt.month(), nowDt.day() - 1,
                             iniDt.hour(), iniDt.minute(), iniDt.second());
      Serial.print("Stage1/2 - Dt: "); Serial.println(datetimeToString(Dt));
      //    Enable = Dt <= nowDt;
    }
    Enable = Dt <= nowDt;
  }
  else {
    // La fecha INICIO es igual o mayor que AHORA = NO
    Enable = false;
  }

  //Enable = iniDt <= nowDt;
  Disable = endDt >= nowDt;

  Serial.print("Enable= "); Serial.print(Enable);
  Serial.print(" Disable= "); Serial.println(Disable);

  return Enable & Disable;
}

boolean libRTC::isCompare(DateTime iniDt, DateTime endDt) {
  TimeSpan A = iniDt - endDt;
  return A.totalseconds() > 0;
}

/*
    void read()
    {
      DateTime now = rtc.now();

      // calculate a date which is 7 days and 30 seconds into the future
      DateTime future (now + TimeSpan(7, 12, 30, 6));

      Serial.print(" now + 7d + 30s: ");
      Serial.print(future.year(), DEC);
      Serial.print('/');
      Serial.print(future.month(), DEC);
      Serial.print('/');
      Serial.print(future.day(), DEC);
      Serial.print(' ');
      Serial.print(future.hour(), DEC);
      Serial.print(':');
      Serial.print(future.minute(), DEC);
      Serial.print(':');
      Serial.print(future.second(), DEC);
      Serial.println();
    }
*/
