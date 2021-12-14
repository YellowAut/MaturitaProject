#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <time.h>

int hours, minutes, seconds;
long counter, mytime, interval = 5000;
bool stav = 1;
int cil = 3;
int pocetPomodor;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  //Priprava LCDcka
  lcd.init();
  lcd.backlight();
  mytime = millis() + interval;
}

void loop()
{
  checkStavu();
}

void odpocet()
{
  lcd.setCursor(1, 1);
  counter = (mytime - millis()) / 1000;
  hours = counter / 3600;
  counter -= (hours * 3600);
  minutes = counter / 60;
  counter -= (minutes * 60);
  seconds = counter;
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10)
  {
    lcd.print("0");
    lcd.print(minutes);
  }
  else
    lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) {
    lcd.print("0");
    lcd.print(seconds);
  }
  else
    lcd.print(seconds);
}

void pomodoro()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kolik zbyva: ");
  pocetPomodor++;
  odpocet();
}

void prestavka()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Prestavka: ");
  odpocet();
}

void konec()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kusy je kokot");
}

void checkStavu()
{
  if (pocetPomodor >= cil)
  {
    konec();
  }
  else
  {
    if (millis() > mytime)
    {
      mytime = millis() + interval;
      stav = !stav;
      if (stav == true)
      {
        pomodoro();
      }
      else
      {
        prestavka();
      }
    }
  }
}