/*
   2020/09/17 - Creacion de la libreria.


*/
#include <FS.h>

class libFile {
  private:

  public:
    void begin();
    void info();
    void format();
    void checkFiles(String nfile[], int nfile_size);
    void dirFiles();
    boolean rm(String dir);
    void help();
    void menu(String command, String parameters);
};

void libFile::begin() {
  SPIFFS.begin();
}

void libFile::info() {
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  Serial.println("SPIFFS - Info");
  Serial.print("Total Bytes= "); Serial.println(fs_info.totalBytes); // Tamaño total utilizable en el sistema de archivos en bytes
  Serial.print("Used Bytes= "); Serial.println(fs_info.usedBytes); // Bytes usados en el sistema de archivos
  Serial.print("Block Size= "); Serial.println(fs_info.blockSize); // Tamaño del bloque, en bytes
  Serial.print("Page Size= "); Serial.println(fs_info.pageSize); // Tamaño de la página, en bytes
  Serial.print("Max Open Files= "); Serial.println(fs_info.maxOpenFiles); // Num. máximo de archivos simultáneamente abiertos.
  Serial.print("Max Path Length= "); Serial.println(fs_info.maxPathLength); // Tamaño máximo del path, incluyendo el ‘\0’ final. Normalmente debería ser 32.
}

void libFile::format() {
  Serial.println("SPIFFS - Format");
  Serial.print("Run format. Wait!!");
  SPIFFS.format();
  Serial.print("End format");
}

void libFile::checkFiles(String nfile[], int nfile_size) {
  for (int i = 0; i < nfile_size; i++) {
    Serial.print("File= "); Serial.print(nfile[i]);
    Serial.print(" Exist= "); Serial.println(SPIFFS.exists(nfile[i]) == 0 ? "No" : "Yes");
  }
}

void libFile::dirFiles() {
  boolean Empity = true;
  Serial.println("SPIFFS - Directory tree");
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    Empity = false;
    Serial.print("Namefile= "); Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.print(" Size= "); Serial.println(f.size());
  }
  if (Empity) {
    Serial.println("Empity");
  }
}

boolean libFile::rm(String dir) {
  Serial.print("WS rm Task");
  if (SPIFFS.exists(dir)) {
    SPIFFS.remove(dir);
    Serial.print("OK: ");
  }
  else
    Serial.println("ERROR: ");
  Serial.println(dir);

  return !SPIFFS.exists(dir);
}

void libFile::help() {
  Serial.println("SPIFFS - Help");
  Serial.println("format\t - Fomatea el espacio de almacenamiento.");
  Serial.println("info  \t - Informa el estado del espacio de alamacenamiento.");
  Serial.println("check \t - Verifica se existe/n un/los archivo/s.");
  Serial.println("tree  \t - Devuelve la estructura de archivos.");
  Serial.println("help  \t - Manual de intrucciones.");
  Serial.println();
}

void libFile::menu(String command, String parameters) {
  command.toLowerCase();
  if (command == "format") {
    format();
  }
  else if (command == "info") {
    info();
  }
  else if (command == "rm") {
    rm(parameters);
  }
  else if (command == "check" & parameters.length() > 0) {
    int nameFilesSize = 1;
    for (int nchar = 0; nchar < parameters.length(); nchar++) {
      if (parameters[nchar] == ' ')
        nameFilesSize++;
    }
    int nFile = 0;
    String nameFiles[nameFilesSize];
    for (int nchar = 0; nchar < parameters.length(); nchar++) {
      if (parameters[nchar] == ' ')
        nFile++;
      else
        nameFiles[nFile] += parameters[nchar];
    }
    checkFiles(nameFiles, nameFilesSize);
  }
  else if (command == "tree")
    dirFiles();
  else if (command == "help")
    help();
  else
    Serial.println("File - nvalid command. Type help");
}

/*
  void SPIFFS_Read(String nfile) {
  // Siempre usa esto para "montar" el sistema de archivos
  bool result = SPIFFS.begin();
  Serial.println("SPIFFS opened: " + result);

  // esto abre el archivo "prueba.txt" en modo lectura
  File f = SPIFFS.open(nfile, "r");

  if (!f) {
    Serial.println("File doesn't exist yet. Creating it");

    // abre el archivo en modo de escritura
    File f = SPIFFS.open(nfile, "w");
    if (!f) {
      Serial.println("file creation failed");
    }
    // ahora escribe dos líneas en estilo de clave/valor con caracteres de fin de línea
    f.println("ssid=abc");
    f.println("password=123455secret");
  } else {
    // podríamos abrir el archivo.
    while (f.available()) {
      // Permite leer línea por línea desde el archivo
      String line = f.readStringUntil('\n');
      Serial.println(line);
    }

  }
  f.close();
  }
*/
