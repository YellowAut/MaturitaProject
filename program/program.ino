#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

unsigned long cas;
int hodiny, minuty, vteriny;
long poct;
unsigned long casSpust;
unsigned long casAktual;
int nasDelka = 60000;
int prestav = 30000;
bool stav;

LiquidCrystal_I2C lcd(0x27,20,4);

void setup() 
{
  //Priprava LCDcka
  lcd.init();
  lcd.backlight();
  lcd.begin(16, 2);                          // put your LCD parameters here
  lcd.print("Krmeni Lukase: ");
}

void loop() 
{
  
}

int odpocetCasu ()
{
  //Zobrazuj na LCD uběhnuté millis
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(4, 1);
  // print the number of seconds since reset:
  poct = (nasDelka - millis()) / 1000;
  hodiny = poct / 3600;
  poct -= (hodiny * 3600);
  minuty = poct / 60;
  poct -= (minuty * 60);
  vteriny = poct;
  lcd.print(hodiny);
  lcd.print(":");
  if (minuty < 10) 
  {
    lcd.print("0");
    lcd.print(minuty);
  }
  else
    lcd.print(minuty);
  lcd.print(":");
  if (vteriny < 10) 
  {
    lcd.print("0");
    lcd.print(vteriny);
  }
  else
    lcd.print(vteriny);

  if (poct <= millis())
  {
    
  }
}

int Prestavka()
{

}