#include <LiquidCrystal_I2C.h>

#define CLK 2
#define DT 3
#define SW 4

int counter = 0;
int aktualStavCLK;
int posledniStavCLK;
String aktualDir = "";
unsigned long posledniZmacTlac = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
    Serial.begin(9600);

    lcd.init();
    lcd.backlight();

    lcd.setCursor(0,0);
    lcd.print("Test menu");
    lcd.setCursor(0,1);
    lcd.print("Tomas Mach");
    lcd.setCursor(0,2);
    lcd.print("4EB");
    delay(5000);

    lcd.clear();

    pinMode(CLK, INPUT);
    pinMode(DT, INPUT);
    pinMode(SW, INPUT_PULLUP);

    posledniStavCLK = digitalRead(CLK);
}

void loop()
{
    aktualStavCLK = digitalRead(CLK);
}