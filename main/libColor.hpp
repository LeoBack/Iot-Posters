/*
   2020/09/17 - Creacion de la libreria.


*/
#include <ColorConverterLib.h>

class libColor {
  private:
    int pRed;
    int pGreen;
    int pBlue;
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

  public:
    String Rgb = "000000";
    void begin(int pin_red, int pin_green, int pin_blue);
    void rgbToHex();
    void hexToRgb();
    void print();
    void println();
    void getColor();
    void help();
    void menu(String command, String parameters);
};

void libColor::begin(int pin_red, int pin_green, int pin_blue) {
  pRed = pin_red;
  pGreen = pin_green;
  pBlue = pin_blue;
  pinMode(pRed, OUTPUT);
  pinMode(pGreen, OUTPUT);
  pinMode(pBlue, OUTPUT);
  print();
}

void libColor::rgbToHex() {
  ColorConverter::RgbToHex(red, green, blue, Rgb);
}

void libColor::hexToRgb() {
  ColorConverter::HexToRgb(Rgb, red, green, blue);
}

void libColor::print() {
  libColor::hexToRgb();
  analogWrite(pRed, red);
  analogWrite(pGreen, green);
  analogWrite(pBlue, blue);
}

/*
  Imprime salida RGB.
  OK - 20/11/17
*/
void libColor::println() {
  libColor::hexToRgb();
  
  if (COLOR_INV) {
    analogWrite(pRed, map(red, 0, 254, 1024, 0));
    analogWrite(pGreen, map(green, 0, 254, 1024, 0));
    analogWrite(pBlue, map(blue, 0, 254, 1024, 0));
  }
  else {
    analogWrite(pRed, map(red, 0, 254, 0, 1024));
    analogWrite(pGreen, map(green, 0, 254, 0, 1024));
    analogWrite(pBlue, map(blue, 0, 254, 0, 1024));
  }

  if (DEBUG) {
    Serial.print("#2 RGB: "); Serial.println(Rgb);
  }
}

void libColor::getColor() {
  Serial.println("Color - get");
  Serial.print("Dec= "); Serial.print(red, DEC); Serial.print(green, DEC); Serial.println(blue, DEC);
  Serial.print("Hex= "); Serial.print(red, HEX); Serial.print(green, HEX); Serial.println(blue, HEX);
}

void libColor::help() {
  Serial.println("Color - Help");
  Serial.println("set \t - Establece color. RGB Ej: 0,0,0 o 255,100,100");
  Serial.println("get \t - Devuelve el color actual.");
  Serial.println("help\t - Manual de intrucciones.");
  Serial.println();
}

void libColor::menu(String command, String parameters) {
  command.toLowerCase();
  if (command == "set" & parameters.length() > 0) {
    int nColor = 0;
    String valueColor = "";
    for (int nchar = 0; nchar < parameters.length() + 1; nchar++) {
      if (parameters[nchar] != ',' & nchar < parameters.length()) {
        valueColor += parameters[nchar];
      }
      else {
        if (nColor == 0) {
          Serial.print("> R= "); Serial.println(valueColor);
          red  = atoi(valueColor.c_str());
          nColor++;
          valueColor = "";
        }
        else if (nColor == 1) {
          Serial.print("> G= "); Serial.println(valueColor);
          green  = atoi(valueColor.c_str());
          nColor++;
          valueColor = "";
        }
        else {
          Serial.print("> B= "); Serial.println(valueColor);
          blue  = atoi(valueColor.c_str());
        }
        libColor::println();
      }
    }
  }
  else if (command == "get") {
    getColor();
  }
  else if (command == "help") {
    help();
  }
  else {
    Serial.println("Command no valid");
  }
}
