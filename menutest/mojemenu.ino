#include <LiquidCrystal_I2C.h>

// Piny enkoderu
#define CLK 2
#define DT 3
#define SW 4

// Proměnné
int counter = 0;
int aktualStavCLK;
int posledniStavCLK;
int posledniStavDT;
String aktualDir = "";
unsigned long posledniZmacTlac = 0;

// Piny LCDčka
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
    Serial.begin(9600);
    // Příprava LCDčka
    lcd.init();
    lcd.backlight();
    // Uvítací zpráva
    lcd.setCursor(0, 0);
    lcd.print("Test menu");
    lcd.setCursor(0, 1);
    lcd.print("Tomas Mach");
    lcd.setCursor(0, 2);
    lcd.print("4EB");
    delay(5000);

    lcd.clear();

    vypisLCD();
    // Encoder piny jako input
    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);

    posledniStavCLK = digitalRead(CLK);
    posledniStavDT = digitalRead(DT);
}

void loop()
{
    // Aktualni stav CLK
    aktualStavCLK = digitalRead(CLK);
}

void vypisLCD()
{
    // These are the values which are not changing the operation
    lcd.setCursor(1, 0); // 1st line, 2nd block
    lcd.print("Menu 1"); // text
    //----------------------
    lcd.setCursor(1, 1); // 2nd line, 2nd block
    lcd.print("Menu 2"); // text
    //----------------------
    lcd.setCursor(1, 2); // 3rd line, 2nd block
    lcd.print("Menu 3"); // text
    //----------------------
    lcd.setCursor(1, 3); // 4th line, 2nd block
    lcd.print("Menu 4"); // text
    //----------------------
    lcd.setCursor(13, 0); // 1st line, 14th block
    lcd.print("cnt: ");   // counts - text
}