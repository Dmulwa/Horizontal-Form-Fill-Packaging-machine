//#include <stdio.h>
//#include "LCD.h" // For LCD
#include "LiquidCrystal_I2C.h" // Added library*
#include <Wire.h>
#include <itoa.h>

#define disk1 0x54    //Address of 24LC256 eeprom chip
//
//char newStr[10] ;

//Global variables
int screen, Length, itemSelected, sub, x, y, subMenuSelected;
int menu_Pointer = 0;
String text = "Hewbe";
//Main Menu
//String menu[3] = {"Pack:", "Setup:", "Data:"};
String menu[1] = {"Pack:"};
//sub menu
String subMenu0[3] = {"JOG:", "SPEED:", "Length:"};

String subMenu1[3] = {"pieces:", "Paper Length:", "Reset:"};



//LCD definitions
//--------------LCD DEFINITIONS-----------------
#define COLUMNS          16
#define ROWS             2

#define LCD_SPACE_SYMBOL 0x20  //space symbol from the lcd ROM, see p.9 of GDM2004D datasheet

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);

int text_width = 3;
int lcd_column = 2;
int lcd_row = 1;
//keypad
const byte rows = 5;
const byte cols = 4;

char keys[rows][cols] = {
  {'R', 'G', 'Y'}
};

//byte rowPins[rows] = {2, 3, 4, 5, 6};
//byte colPins[cols] = {10, 9, 8, 7};

byte rowPins[rows] = {PA12, PA1, PA11};
byte colPins[cols] = {PA0};

//functional variables
float pieces, p_length;
int capacity; //speed



//Define Pins
#define conv PB9

//stepper
#define stepsPerRevolution 1600
#define seal_dir PA8
#define seal_stp PA7

#define feed PB15
#define paper PB1
#define roll PB8

//inputs
#define temp PB8
#define prox PB0
#define eye PB3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //pmp
  pinMode(conv, OUTPUT);
  digitalWrite(conv, LOW);
  //keypad
  for (int r = 0; r < rows; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); //rowpins as input
    // digitalWrite(rowPins[r],HIGH);

  }
  for (int c = 0; c < cols; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH);
  }

  //stepper
  pinMode(paper, OUTPUT);
  pinMode(feed, OUTPUT);
  pinMode(roll, OUTPUT);
  pinMode(PB12, OUTPUT);
  digitalWrite(PB12, HIGH);
  pinMode(seal_dir, OUTPUT);
  pinMode(seal_stp, OUTPUT);

  //input
  pinMode(temp, INPUT_PULLUP);
  pinMode(prox, INPUT_PULLUP);
  pinMode(eye, INPUT_PULLUP);
  //initialise LCD
  lcd.begin(COLUMNS, ROWS, LCD_5x8DOTS);
  lcd.clear();
  Wire.begin();
  /* prints static text */
  lcd.setCursor(1, 1);
  lcd.print(F("Hewbe Automation"));
  delay(2000);
  // lcd.clear();
  /* prints static text */
  // int out1 = out;



  // read data addresses
  pieces = readData(disk1, 20);
  p_length = readData(disk1, 60);
  capacity = readData(disk1, 80);
  lcd.clear();
}

void loop() {

  topMenu();
}

void topMenu() {


  while (1) {
    text = ">>Hewbe<<";
    screen = 3;
    x = 1;
    y = 1;
    text_width = 3;
    lcd_column = 1;
    lcd_row = 1;
    Length = 4;

    itemSelected = displayMenu(menu, Length);

    switch (itemSelected) {

      case 0: {
          //sell code
          pack();
          break;

        }
      case 1: {
          //user submen
          sub0();
          break;

        }

      case 2: {
          sub1();
          break;

        }



    }



    //Serial.println(itemSelected);

  }
}
void sub0() {
  //Length = 4;
  text = ">>Setup<<";
  screen = 3;
  x = 1;
  y = 1;
  text_width = 3;
  lcd_column = 1;
  lcd_row = 1;
  Length = 4;
  bool user = true;

  while (user) {
    text = ">>User Menu<<";
    center_text(text);
    itemSelected = displayMenu(subMenu0, Length);
    switch (itemSelected) {
      case 0: {
          jog();
          break;

        }

      case 1: {
          Speed();
          break;
        }
      case 2: {
          product_length();
          break;
        }


    }
    //user = false;
  }

}


void sub1() {
  Length = 4;
  text = ">>Info<<";
  screen = 3;


  //while (!passwrd(admin_pass));

  x = 1;
  y = 1;
  text_width = 3;
  lcd_column = 1;
  lcd_row = 1;
  bool admin = true;
  while (admin) {

    itemSelected = displayMenu(subMenu1, Length);

    switch (itemSelected) {
      case 0: {

          P_pieces();
          break;

        }
      case 1: {
          paper_Used();
          break;
        }



      case 2: {
          Reset();
          break;
        }

    }
  }

}

int displayMenu(String menuInput[], int menuLength) {
  bool display = true;
  char valKey = 0;


  while (display) {
    for (int i = 0; i < menuLength; i++) {
      lcd.clear();
      center_text(text);
      int x = lcd_column;
      int y = lcd_row;

      int state = i;
      menu_Pointer = state;


      //Arrow
      lcd.setCursor(0, 1);
      lcd.print((char) 126);

      //y list
      String k = menuInput[i];
      Serial.println(k);
      lcd.setCursor(x, y);
      lcd.print(k);

      //done printing
      bool input = true;
      uint32_t instant_t0 = millis();
      //input
      while (input) {
        valKey = get_Key();
        if (millis() - instant_t0 > 10000) {
          input = false;
          display = false;
          topMenu();
        }

        switch (valKey) {
          case 'E': {
              input = false;
              display = false;
              state = i;
              break;

            }

          case 'B': {
              if (state > 0) {
                input = false;
                state = 0;
                state--;
              }
              else {
                topMenu();

              }
              break;

            }
          case 'U': {
              if (state - 1 < 0) {
                state = menuLength - 2;
              }
              else {
                state = state - 2;
              }
              input = false;
              break;
            }
          case 'D': {
              input = false;
              break;
            }

        }
      }


      i = state;
      Serial.println(i);
      Serial.println("\n");
      Serial.println("\n");
      //delay(5000);

      //keypad


    }

  }

  return menu_Pointer;
}


//keypad input
//keypress function
char get_Key() {
  char k = 0;
  bool button = true;

  for (int c = 0; c < cols; c++) {
    digitalWrite(colPins[c], LOW);

    for (int r = 0; r < rows; r++) {
      int p = digitalRead(rowPins[r]);
      if (p == LOW) {

        while (button) {

          k = keys[r][c];
          button = false;

        }
      }

    }
    digitalWrite(colPins[c], HIGH);

  }
  //Serial.println("escaping:");
  delay(300);
  return k;

}
//Handle multiple matrix keypad input

int myInput(int c, int r, int width) {

  char keyVal = 0;
  char buff[width];

  //memset(buff; /0; width);
  int i = 0;
  bool inputState = true;
  //width = width-1;
  center_text(text);
  lcd.setCursor(c, r);
  //null terminator
  for (int i = 0; i < width + 1; i++ ) {
    buff[i] = 0;
  }
  while (inputState) {
    keyVal = get_Key();
    //lcd.clear();
    if (c + i < c + width) {
      lcd.setCursor(c + i, r);
      lcd.cursor();
    }
    else {
      lcd.setCursor(c + width, r);
      lcd.cursor();
    }

    switch (keyVal) {
      case 'E':
        {
          Serial.println("Enter");
          inputState = false;
          break;
        }

      case 'B':
        {
          for (int y = 0; y < width; y++) {
            buff[y] = 0;
          }
          Serial.println("Back");
          inputState = false;
          break;

        }

      case 'L':
        {
          Serial.println("LEFT");
          if (i > 0) {
            i--;
            //  i--;
            //  c--;




          }
          break;
        }

      case 'R':
        {
          Serial.println("RIGHT");
          if (i > 0 ) {
            i++;
            //c++;

          }
          break;
        }

      default:
        {
          if (keyVal >= '0' && keyVal <= '9') {
            if (i < width) {
              //setCursor
              buff[i] = +keyVal;
              //Serial.println(n);
              i++;
              //display here
              Serial.println(buff);
              lcd.setCursor(c, r);
              lcd.print("                 ");
              lcd.setCursor(c, r);

              lcd.print(buff);


              //Serial.println("in myInput");
              //c++;
              delay(200);

            }
            else {

              i--;
              //setCursor
              buff[i] = 0;
              buff[i] = +keyVal;
              i++;

              //display here
              lcd.setCursor(c, r);
              lcd.print("                 ");
              lcd.setCursor(c, r);
              Serial.println(buff);
              lcd.print(buff);
              //c++;
              //Serial.println("in myInput");
              delay(200);



            }

          }
          break;
        }




    }
  }
  int nmber = atoi(buff);
  lcd.noCursor();
  return nmber;
}



//FUNCTIONS

void pack() {
  lcd.clear();
  center_text("PACK");
  //Home seal jaws
  lcd.setCursor(0, 1);
  lcd.print("Homing...");
  bool L = false;
  digitalWrite(seal_dir, HIGH);
  while (L) {
    digitalWrite(seal_stp, LOW);
    delayMicroseconds(1000);
    digitalWrite(seal_stp, LOW);
    delayMicroseconds(1000);
    if (digitalRead(prox) == LOW) {
      L = false;
    }
    if (digitalRead(PA1) == LOW) {
      L = false;
    }

  }
  //temperature
  lcd.clear();
  center_text("PACK");
  lcd.setCursor(0, 1);
  lcd.print("Heating...");
  //while (digitalRead(temp) != 0);
  lcd.clear();
  center_text("PACK");
  lcd.setCursor(0, 1);
  lcd.print("Done: Press G");
  L = true;
  lcd.clear();
  center_text("PACK");
  int c = 0;
  lcd.setCursor(0, 1);
  lcd.print("Pieces:");
  lcd.setCursor(9, 1);
  lcd.print(c);
  //DELAYS ratio of diameters
  uint32_t feed_delay = 800;
  uint32_t roll_delay = 500;
  uint32_t paper_delay = 400;

  uint32_t instant_t0 = micros();
  uint32_t instant_t1 = micros();
  uint32_t instant_t2 = micros();

  while (L) {
    //start packing
    /* if (micros() - instant_t0 > feed_delay) {
       digitalWrite(feed, HIGH);
       instant_t0 = micros();
      }
      if (micros() - instant_t1 > roll_delay) {
       digitalWrite(roll, HIGH);
       instant_t1 = micros();
      }
      if (micros() - instant_t2 > paper_delay) {
       digitalWrite(paper, HIGH);
       instant_t2 = micros();
      }*/
    digitalWrite(paper, HIGH);
    digitalWrite(feed, HIGH);
    digitalWrite(roll, HIGH);
    delayMicroseconds(600);
    digitalWrite(paper, LOW);
    digitalWrite(feed, LOW);
    digitalWrite(PB12, LOW);
    digitalWrite(roll, LOW);
    delayMicroseconds(400);
    //abort
    if (digitalRead(PA12) == LOW) {
      bool A = true;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("R = exit");
      lcd.setCursor(0, 1);
      lcd.print("Y = Continue");

      while (A) {
        if (get_Key() == 'R') {
          //abort
          L = false;
          A = false;
          center_text("PACK");
          lcd.setCursor(0, 1);
          lcd.print("Exiting....");
          delay(2000);
        }
        if (get_Key() == 'Y') {
          //CONTINUE
          center_text("PACK");
          lcd.setCursor(0, 1);
          lcd.print("Pieces:");
          lcd.setCursor(9, 1);
          lcd.print(c);
          A = false;
        }
      }
    }
    /*
      //seal and cut
      if (digitalRead(eye) == LOW) {
      digitalWrite(seal_dir, LOW);
      for (int i = 0; i < 800; i++) {
        digitalWrite(seal_stp, LOW);
        delayMicroseconds(500);
        digitalWrite(seal_stp, LOW);
        delayMicroseconds(500);
      }
      delay(400);
      digitalWrite(seal_dir, HIGH);
      for (int i = 0; i < 800; i++) {
        digitalWrite(seal_stp, LOW);
        delayMicroseconds(500);
        digitalWrite(seal_stp, LOW);
        delayMicroseconds(500);
      }
      //add pieces
      c++;
      center_text("PACK");
      lcd.setCursor(0, 1);
      lcd.print("Pieces:");
      lcd.setCursor(9, 1);
      lcd.print(c);
      }*/

  }



}

void jog() {
  lcd.clear();
  bool L = true;
  center_text("JOG");
  //p_Length = readData(disk1, 60);
  lcd.setCursor(2, 1);
  lcd.print("press Y");
  while (L) {
    if (get_Key() == 'Y') {
      for (int i = 0; i < 800; i++) {
        digitalWrite(roll, HIGH);
        delayMicroseconds(1000);
        digitalWrite(roll, LOW);
        digitalWrite(paper, HIGH);
        delayMicroseconds(1000);
        digitalWrite(paper, LOW);
      }
    }
    if (get_Key() == 'G' ) {

      L = false;
    }

  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("set success");
  while (get_Key() != 'R');
  center_text("Setup");

}
void Speed() {


}

void product_length() {
  lcd.clear();
  bool L = true;
  center_text("Paper Length");
  p_length = readData(disk1, 60);
  lcd.setCursor(2, 1);
  lcd.print(p_length);
  while (L) {
    if (get_Key() == 'Y') {
      p_length = p_length + 10;
      lcd.print(p_length);
    }
    if (get_Key() == 'R' && p_length > 10) {
      p_length = p_length - 10;
      lcd.print(p_length);
    }
    if (get_Key() == 'G') {
      L = false;

    }
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("set success");
  while (get_Key() != 'R');
  center_text("Setup");

}

void P_pieces() {
  lcd.clear();
  center_text("PIECES");
  pieces = readData(disk1, 20);
  lcd.setCursor(2, 1);
  lcd.print(pieces);
  while (get_Key() != 'R');
  center_text("Info");
}

void  paper_Used() {
  lcd.clear();
  center_text("Used Paper: mtrs");
  float bulk = (pieces * p_length) / 1000;
  lcd.setCursor(0, 1);
  lcd.print("Length:");
  lcd.setCursor(9, 1);
  lcd.print(bulk);
  while (get_Key() != 'R');
  center_text("Info");
}
//reset
void Reset() {
  lcd.clear();
  center_text("RESET");
  //reset data to 0
  lcd.setCursor(0, 1);
  center_text("Green to cont.");
  while (get_Key() != 'G');
  lcd.clear();
  center_text("RESETTING...");

  writeData(disk1, 20, 0.00);

  writeData(disk1, 60, 0.00);

  lcd.setCursor(0, 1);
  // delay(2000);
  lcd.setCursor(2, 2);
  lcd.print(F("RESET COMPLETE"));
  while (get_Key() != 'R');
  center_text("Info");
  delay(2000);
}


//Heading display
void center_text(String dis) {

  //int disp= atoi(dis.c_str());
  int string_length = dis.length();

  int x = (COLUMNS - string_length) / 2;
  lcd.setCursor(x, 0);
  lcd.print(dis);
}




//External EEProm functions


byte readEEPROM(int deviceaddress, unsigned int eeaddress )
{
  byte rdata = 0xFF;

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(deviceaddress, 1);

  if (Wire.available()) rdata = Wire.read();

  return rdata;
}

void writeEEPROM(int deviceaddress, unsigned int eeaddress, byte data )
{
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB

  Wire.write(data);
  Wire.endTransmission();

  delay(5);
}

float readData(int devAddr, unsigned int eeaddress) {
  //read
  char chrFloat[] = "";
  char newStr[20];
  for (byte i = 0; i <= 10; i++) {
    newStr[i] = readEEPROM(disk1, eeaddress + i);
    newStr[i + 1] = 0;
    //convert to float

  }
  float out = atof(newStr);
  return out;

}

void writeData(int deviceaddress, unsigned int eeaddress, float data) {
  float t = data;
  char chrFloat[] = "";
  char chBuff[10];
  dtostrf(t, 6, 4, chBuff);
  strcat(chrFloat, chBuff);

  //write
  for (byte i = 0; i < 10; i++) {
    writeEEPROM(disk1, eeaddress + i, chrFloat[i]);


  }
}

void writeLong(int deviceaddress, unsigned int eeaddress, char* data) {
  ///char t = data;
  // char chrFloat[] = "";
  // char chBuff[10];
  //dtostrf(t, 6, 4, chBuff);
  // strcat(chrFloat, chBuff);

  //write
  for (byte i = 0; i < 10; i++) {
    writeEEPROM(disk1, eeaddress + i, data[i]);


  }
}

long int readLong(int devAddr, unsigned int eeaddress) {
  //read
  char chrFloat[] = "";
  char newStr[20];
  for (byte i = 0; i <= 10; i++) {
    newStr[i] = readEEPROM(disk1, eeaddress + i);
    newStr[i + 1] = 0;
    //convert to float

  }
  long int out = atoi(newStr);
  return out;

}
