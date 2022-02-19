#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
//#include <time.h>

int hours, minutes, seconds;
long counter, interval = 5000;
bool stav;
int target = 3;
int numPomodoro = 0;
long mytime;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  Serial.begin(9600);
  //Priprava LCDcka
  lcd.init();
  lcd.backlight();
}

void loop()
{
    checkStavu();
}

void odpocet()
{
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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Kolik zbyva: ");
  numPomodoro++;
  odpocet();
}

void prestavka()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Prestavka: ");
  lcd.print("Prestavka: ");
  odpocet();
}

int konec()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.println("Autak je idiot");
  lcd.print("Autak je idiot");
  delay(1000);
  target = numPomodoro + 3;
  stav = !stav;
}

int checkStavu()
{
  if (numPomodoro >= target)
  {
    konec();
  }
  else
  {
    mytime = millis() + interval + 1000;
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