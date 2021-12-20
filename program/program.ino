#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
//#include <time.h>

int hours, minutes, seconds;
long counter, mytime = 5000;
bool stav;
int cil = 3;
int pocetPomodor;
int interval;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  //Serial.println("setup");
  Serial.begin(9600);
  //Priprava LCDcka
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,1);
  Serial.println("Zadej pocet vterin: ");
  lcd.print("Zadej pocet vterin: ");
  //mytime = millis() + interval;
  //checkStavu();
}

void loop()
{
/*  //Serial.println("Loop");
  //inputUzivatel();
  if (Serial.available() > 0)
  {
    //Serial.println("Check inputu");
    interval = Serial.read()*10000;
    Serial.flush();*/
    interval = Serial.read()*10000;
    pocetPomodor = 0;
    checkStavu();
  //}
}

void odpocet()
{
  //Serial.print("Zacatek odpoctu");
  while ( mytime >= millis() )
  {
    lcd.setCursor(1, 1);
    counter = (mytime - millis()) / 1000;
    hours = counter / 3600;
    counter -= (hours * 3600);
    minutes = counter / 60;
    counter -= (minutes * 60);
    seconds = counter;
    Serial.print(hours);
    lcd.print(hours);
    Serial.print(":");
    lcd.print(":");
    if (minutes < 10)
    {
      Serial.print("0");
      Serial.print(minutes);
      lcd.print("0");
      lcd.print(minutes);
    }
    else
    {
      Serial.print(minutes);
      lcd.print(minutes);
    }
    Serial.print(":");
    lcd.print(":");
    if (seconds < 10) {
      Serial.print("0");
      Serial.println(seconds);
      lcd.print("0");
      lcd.print(seconds);
    }
    else
    {
      Serial.println(seconds);
      lcd.print(seconds);
    }
  }
  checkStavu();
}

void pomodoro()
{
  //Serial.println("pomodoro");
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Kolik zbyva: ");
  lcd.print("Kolik zbyva: ");
  pocetPomodor++;
  odpocet();
}

void prestavka()
{
  //Serial.println("prestavka");
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Prestavka: ");
  lcd.print("Prestavka: ");
  odpocet();
}

void konec()
{
  //Serial.println("Konec");
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Autak je idiot");
  lcd.print("Autak je idiot");
  cil = pocetPomodor + 3;
}

void checkStavu()
{
  //Serial.println("checkStavu");
  if (pocetPomodor >= cil)
  {
    konec();
  }
  else
  {
    mytime = millis() + interval;
    stav = !stav;
    if (stav == true)
    {
      pomodoro();
    }
    else if (stav == false)
    {
      prestavka();
    }
  }
}
