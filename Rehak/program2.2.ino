#include <ClickEncoder.h>
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "HX711.h"

int oldEncPos, encPos;
int buttonState, oldButtonState;

int enA_V = 6;   //24 voda rychlost
int in1_V = 47;  //24 voda +
int in2_V = 45; //24 voda -
int enB_N = 7; //24 napousteci ventil
int in3_N = 43; //24 naposteci ventil +
int in4_N = 41; //24 napousteci ventil -
int enA_1 = 2; //12 prichut 1 rychlost
int in1_1 = 31; //12 prichut 1 +
int in2_1 = 29; //12 prichut 1 -
int in3_2 = 27; // 12 prichut 2 +
int in4_2 = 25; // 12 prichut 2 -
int enB_2 = 3; // 12 prichut 2 rychlost
int enA_3 = 4; // 12 prichut 3 rychlost
int in1_3 = 39; // 12 prichut 3 +
int in2_3 = 37; // 12 prichut 3 -
int enB_4 = 5; // 12 prichut 4 rychlost
int in3_4 = 35; // 12 prichut 4 +
int in4_4 = 33; // 12 prichut 4 -

int Prichut1; //plovak prichut
int Prichut2;
int Prichut3;
int Prichut4;

int volba = 0;
int prichut = 0;
int intenzita = 0;
int vynuluj = 0;
int natoceno = 0;
int obsah = 0;
int objem = 0;

HX711 scale(A1, A0); //DT A1, SCK A0

#define CLK 53
#define DT 51
#define SW 49 //switch
#define STEPS 4

#define Plovak1 46
#define Plovak2 48
#define Plovak3 50
#define Plovak4 52

byte Whole[] {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte esko[] {
  B10001,
  B01110,
  B01111,
  B10001,
  B11110,
  B01110,
  B10001,
  B11111,
};
byte emko[] {
  B01110,
  B00100,
  B01010,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
};
byte elko[] {
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B00000,
};

LiquidCrystal_I2C lcd(0x27, 16, 2);
ClickEncoder encoder(CLK, DT, SW, STEPS);

const unsigned long kontrola = 5000;

const unsigned long vypis = 1000;
unsigned long oldCas = 0;

void setup() {

  pinMode (enA_V, OUTPUT);
  pinMode (in1_V, OUTPUT);
  pinMode (in2_V, OUTPUT);
  pinMode (enB_N, OUTPUT);
  pinMode (in3_N, OUTPUT);
  pinMode (in4_N, OUTPUT);
  pinMode (enA_1, OUTPUT);
  pinMode (in1_1, OUTPUT);
  pinMode (in2_1, OUTPUT);
  pinMode (in3_2, OUTPUT);
  pinMode (in4_2, OUTPUT);
  pinMode (enB_2, OUTPUT);
  pinMode (enA_3, OUTPUT);
  pinMode (in1_3, OUTPUT);
  pinMode (in2_3, OUTPUT);
  pinMode (enB_4, OUTPUT);
  pinMode (in3_4, OUTPUT);
  pinMode (in4_4, OUTPUT);

  pinMode (Plovak1, INPUT_PULLUP);
  pinMode (Plovak2, INPUT_PULLUP);
  pinMode (Plovak3, INPUT_PULLUP);
  pinMode (Plovak4, INPUT_PULLUP);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, Whole);
  lcd.createChar(1, esko);
  lcd.createChar(2, emko);
  lcd.createChar(3, elko);

  Timer1.initialize(1000); //casovac v tomto pripade pro encoder
  Timer1.attachInterrupt(timerIsr);

  encoder.setAccelerationEnabled(true); //zapnuti encoderu
  oldEncPos = -1;
  oldButtonState = -1;

  scale.set_scale(325.f); // 325 parametr - zjisteno po kalibraci, aby hodnoty odpovidaly realite s co nejmensi odchylkou
  scale.tare(); //vynulovani vahy


}

void loop() {
  int zasoba = analogRead (A3);

  unsigned long casTed = millis();

  if (casTed - oldCas >= vypis) {
    Serial.print("ENC:");
    Serial.println(encPos);
    Serial.print("prichut:");
    Serial.println(prichut);
    Serial.print("INTENZITA:");
    Serial.println(intenzita);
    Serial.print("volba:");
    Serial.println(volba);
    Serial.print("VAHA");
    Serial.println(scale.get_units());
    Serial.print("zasoba");
    Serial.println(zasoba);
    oldCas = casTed;
  }


  encPos -= encoder.getValue();
  if (encPos != oldEncPos or buttonState != oldButtonState) {   //PRIDANO OK
    oldEncPos = encPos;
    oldButtonState = buttonState;   //PRIDANO OK

    if (encPos == 0 and volba == 0 and Prichut1 == 0) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Zvolte napoj:");
      lcd.setCursor(5, 1);
      lcd.print("Jablko");
    }

    if (encPos == 0 and volba == 0 and Prichut1 == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Doplnte prichut:");
      lcd.setCursor(4, 1);
      lcd.print("!Jablko!");
    }

    if (encPos == 1 and volba == 0 and Prichut2 == 0) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Zvolte napoj:");
      lcd.setCursor(5, 1);
      lcd.print("Mango");
    }

    if (encPos == 1 and volba == 0 and Prichut2 == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Doplnte prichut:");
      lcd.setCursor(4, 1);
      lcd.print("!Mango!");
    }

    if (encPos == 2 and volba == 0 and Prichut3 == 0) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Zvolte napoj:");
      lcd.setCursor(5, 1);
      lcd.print("Visen");
    }

    if (encPos == 2 and volba == 0 and Prichut3 == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Doplnte prichut:");
      lcd.setCursor(4, 1);
      lcd.print("!Visen!");
    }

    if (encPos == 3 and volba == 0 and Prichut4 == 0) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Zvolte napoj:");
      lcd.setCursor(4, 1);
      lcd.print("Pomeranc");
    }

    if (encPos == 3 and volba == 0 and Prichut4 == 1) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Doplnte prichut:");
      lcd.setCursor(3, 1);
      lcd.print("!Pomeranc!");
    }

    if (encPos == 0 and volba == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte intenzitu");
      lcd.setCursor(3, 1);
      lcd.write(0);
      lcd.setCursor(4, 1);
      lcd.write(0);
      lcd.setCursor(5, 1);
      lcd.write(0);
    }

    if (encPos == 1 and volba == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte intenzitu");
      lcd.setCursor(3, 1);
      lcd.write(0);
      lcd.setCursor(4, 1);
      lcd.write(0);
      lcd.setCursor(5, 1); //prvni
      lcd.write(0);
      lcd.setCursor(6, 1);
      lcd.write(0);
      lcd.setCursor(7, 1);
      lcd.write(0);
      lcd.setCursor(8, 1);
      lcd.write(0);
      lcd.setCursor(9, 1); //druha
      lcd.write(0);
    }

    if (encPos == 2 and volba == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte intenzitu");
      lcd.setCursor(3, 1);
      lcd.write(0);
      lcd.setCursor(4, 1);
      lcd.write(0);
      lcd.setCursor(5, 1); //prvni
      lcd.write(0);
      lcd.setCursor(6, 1);
      lcd.write(0);
      lcd.setCursor(7, 1);
      lcd.write(0);
      lcd.setCursor(8, 1);
      lcd.write(0);
      lcd.setCursor(9, 1); //druha
      lcd.write(0);
      lcd.setCursor(10, 1);
      lcd.write(0);
      lcd.setCursor(11, 1);
      lcd.write(0);
      lcd.setCursor(12, 1); //treti
      lcd.write(0);
    }
    if (encPos == 0 and volba == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte velikost:");
      lcd.setCursor(5, 1);
      lcd.print(">S<M L");
    }
    if (encPos == 1 and volba == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte velikost:");
      lcd.setCursor(6, 1);
      lcd.print("S>M<L");
    }
    if (encPos == 2 and volba == 2) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Zvolte velikost:");
      lcd.setCursor(6, 1);
      lcd.print("S M>L<");
    }

  }

  if (buttonState == 5) {
    volba++;
  }

  if (buttonState == 6) {
    volba--;
  }

  if (volba == 0) {
    if (encPos <= -1) {
      encPos = 0;
    }
    if (encPos >= 4) {
      encPos = 3;
    }
  }
  if (volba == 1 and intenzita == 0) {
    if (encPos <= -1) {
      encPos = 0;
    }
    if (encPos > 3) {
      encPos = 2;
    }
  }
  if (volba == 2 and objem == 0) {
    if (encPos <= -1) {
      encPos = 0;
    }
    if (encPos > 3) {
      encPos = 2;
    }
  }

  if (volba == 1 and encPos == 0 and buttonState == 5 and Prichut1 == 0) {    //normal stav
    prichut = 1;
    encPos = 0;
  }
  if (volba == 1 and encPos == 1 and buttonState == 5 and Prichut2 == 0) {
    prichut = 2;
    encPos = 0;
  }
  if (volba == 1 and encPos == 2 and buttonState == 5 and Prichut3 == 0) {
    prichut = 3;
    encPos = 0;
  }
  if (volba == 1 and encPos == 3 and buttonState == 5 and Prichut4 == 0) {    //normal stav
    prichut = 4;
    encPos = 0;
  }

  if (volba == 1 and encPos == 0 and buttonState == 5 and Prichut1 == 1) {    //chybovy stav
    prichut = 0;
    encPos = 0;
    volba = 0;
  }
  if (volba == 1 and encPos == 1 and buttonState == 5 and Prichut2 == 1) {
    prichut = 0;
    encPos = 1;
    volba = 0;
  }
  if (volba == 1 and encPos == 2 and buttonState == 5 and Prichut3 == 1) {
    prichut = 0;
    encPos = 2;
    volba = 0;
  }
  if (volba == 1 and encPos == 3 and buttonState == 5 and Prichut4 == 1) {    //chybivy stav
    prichut = 0;
    encPos = 1;
    volba = 0;
  }

  if (volba == 2 and encPos == 0 and buttonState == 5) {
    intenzita = 1;
    encPos = 0;
  }
  if (volba == 2 and encPos == 1 and buttonState == 5) {
    intenzita = 2;
    encPos = 0;
  }
  if (volba == 2 and encPos == 2 and buttonState == 5) {
    intenzita = 3;
    encPos = 0;
  }

  if (volba == 3 and encPos == 0 and buttonState == 5) {
    objem = 150;
  }
  if (volba == 3 and encPos == 1 and buttonState == 5) {
    objem = 250;
  }
  if (volba == 3 and encPos == 2 and buttonState == 5) {
    objem = 300;
  }

  if (prichut == 1 and intenzita == 1 and natoceno == 0 and objem != 0) { //NAPOJ 1 INTENZITA 1
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_1, HIGH);
    digitalWrite(in2_1, LOW);
    analogWrite(enA_1, 255);
    if (scale.get_units(5) >= 21) {
      digitalWrite(in1_1, LOW);
      digitalWrite(in2_1, LOW);
      natoceno = 21;
    }
  }

  if (prichut == 1 and intenzita == 2 and natoceno == 0 and objem != 0) {  //NAPOJ 1 INTENZITA 2
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_1, HIGH);
    digitalWrite(in2_1, LOW);
    analogWrite(enA_1, 255);
    if (scale.get_units(5) >= 31) {
      digitalWrite(in1_1, LOW);
      digitalWrite(in2_1, LOW);
      natoceno = 31;
    }
  }

  if (prichut == 1 and intenzita == 3 and natoceno == 0 and objem != 0) { //NAPOJ 1 INTENZITA 3
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_1, HIGH);
    digitalWrite(in2_1, LOW);
    analogWrite(enA_1, 255);
    if (scale.get_units(5) >= 21) {
      digitalWrite(in1_1, LOW);
      digitalWrite(in2_1, LOW);
      natoceno = 41;
    }
  }

  if (prichut == 2 and intenzita == 1 and natoceno == 0 and objem != 0) { //NAPOJ 2 INTENZITA 1
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_2, HIGH);
    digitalWrite(in4_2, LOW);
    analogWrite(enB_2, 255);
    if (scale.get_units(5) >= 21) {
      digitalWrite(in3_2, LOW);
      digitalWrite(in4_2, LOW);
      natoceno = 21;
    }
  }

  if (prichut == 2 and intenzita == 2 and natoceno == 0 and objem != 0) {  ///NAPOJ 2 INTENZITA 2
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_2, HIGH);
    digitalWrite(in4_2, LOW);
    analogWrite(enB_2, 255);
    if (scale.get_units(5) >= 31) {
      digitalWrite(in3_2, LOW);
      digitalWrite(in4_2, LOW);
      natoceno = 31;
    }
  }

  if (prichut == 2 and intenzita == 3 and natoceno == 0 and objem != 0) {   ///NAPOJ 2 INTENZITA 3
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_2, HIGH);
    digitalWrite(in4_2, LOW);
    analogWrite(enB_2, 255);
    if (scale.get_units(5) >= 41) {
      digitalWrite(in3_2, LOW);
      digitalWrite(in4_2, LOW);
      natoceno = 41;
    }
  }

  if (prichut == 3 and intenzita == 1 and natoceno == 0 and objem != 0) {   ///NAPOJ 3 INTENZITA 1
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_3, HIGH);
    digitalWrite(in2_3, LOW);
    analogWrite(enA_3, 255);
    if (scale.get_units(5) >= 21) {
      digitalWrite(in1_3, LOW);
      digitalWrite(in2_3, LOW);
      natoceno = 21;
    }
  }

  if (prichut == 3 and intenzita == 2 and natoceno == 0 and objem != 0) {   ///NAPOJ 3 INTENZITA 2
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_3, HIGH);
    digitalWrite(in2_3, LOW);
    analogWrite(enA_3, 255);
    if (scale.get_units(5) >= 31) {
      digitalWrite(in1_3, LOW);
      digitalWrite(in2_3, LOW);
      natoceno = 31;
    }
  }

  if (prichut == 3 and intenzita == 3 and natoceno == 0 and objem != 0) {   ///NAPOJ 3 INTENZITA 3
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in1_3, HIGH);
    digitalWrite(in2_3, LOW);
    analogWrite(enA_3, 255);
    if (scale.get_units(5) >= 41) {
      digitalWrite(in1_3, LOW);
      digitalWrite(in2_3, LOW);
      natoceno = 41;
    }
  }

  if (prichut == 4 and intenzita == 1 and natoceno == 0 and objem != 0) {   ///NAPOJ 4 INTENZITA 1
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_4, HIGH);
    digitalWrite(in4_4, LOW);
    analogWrite(enB_4, 255);
    if (scale.get_units(5) >= 21) {
      digitalWrite(in3_4, LOW);
      digitalWrite(in4_4, LOW);
      natoceno = 21;
    }
  }

  if (prichut == 4 and intenzita == 2 and natoceno == 0 and objem != 0) {   ///NAPOJ 4 INTENZITA 2
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_4, HIGH);
    digitalWrite(in4_4, LOW);
    analogWrite(enB_4, 255);
    if (scale.get_units(5) >= 31) {
      digitalWrite(in3_4, LOW);
      digitalWrite(in4_4, LOW);
      natoceno = 31;
    }
  }

  if (prichut == 4 and intenzita == 3 and natoceno == 0 and objem != 0) {   ///NAPOJ 4 INTENZITA 3
    vynuluj++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Probiha priprava");
    lcd.setCursor(5, 1);
    lcd.print("napoje");
    digitalWrite(in3_4, HIGH);
    digitalWrite(in4_4, LOW);
    analogWrite(enB_4, 255);
    if (scale.get_units(5) >= 41) {
      digitalWrite(in3_4, LOW);
      digitalWrite(in4_4, LOW);
      natoceno = 41;
    }
  }

  if (natoceno != 0) {              //NATACENI VODY
    digitalWrite(in1_V, HIGH);
    digitalWrite(in2_V, LOW);
    analogWrite(enA_V, 255);
    if (scale.get_units(5) >= objem) {
      digitalWrite(in1_V, LOW);
      digitalWrite(in2_V, LOW);
      volba = 0;
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Odeberte napoj");
      delay(2000);
      prichut = 0;
      vynuluj = 0;
      natoceno = 0;
      encPos = 0;
      intenzita = 0;
      objem = 0;
    }
  }
  if (zasoba < 580) {           //DOPLNOVANI VODY hodnoty???
    digitalWrite(in3_N, HIGH);
    digitalWrite(in4_N, LOW);
    analogWrite(enB_N, 255);
  }
  if (zasoba > 620) {
    digitalWrite(in3_N, LOW);
    digitalWrite(in4_N, LOW);
  }


  Encoder();

  if (digitalRead(Plovak1) == LOW) { //pokud je plovak nahore, dava nulu, tudiz je prichute dost
    Prichut1 = 1;
  }
  else {
    Prichut1 = 0;
  }

  if (digitalRead(Plovak2) == LOW) {
    Prichut2 = 1;
  }
  else {
    Prichut2 = 0;
  }

  if (digitalRead(Plovak3) == LOW) {
    Prichut3 = 1;
  }
  else {
    Prichut3 = 0;
  }

  if (digitalRead(Plovak4) == LOW) {
    Prichut4 = 1;
  }
  else {
    Prichut4 = 0;
  }

  if (vynuluj == 1) {
    scale.tare();
  }

}


void timerIsr() {
  encoder.service();
}
