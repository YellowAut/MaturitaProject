#include <LiquidCrystal_I2C.h>

#define CLK 2
#define DT 3
#define SW 4
#define SAMP 100

LiquidCrystal_I2C lcd(0x27, 20, 4);

int id;
int prev_id = -1;
unsigned long prev_millis;
byte tick = LOW;

unsigned long prev_tlac;
int aktualEncoder;
int predchEncoder;
int stavTlac;

byte menu_nahoru = LOW;
byte menu_dolu = LOW;
byte enter = LOW;

int hours, minutes, seconds;
long counter, interval = 5000;
bool stav;
int cil = 3;
int pocetPomodor = 0;
long mytime;

void setup()
{
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT_PULLUP);
    pinMode(DT, INPUT);

    Serial.begin(9600);
    prev_millis = millis();

    predchEncoder = digitalRead(CLK);

    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("POMODORO");
    delay(5000);
    lcd.clear();
}

void loop()
{
    Serial.println("Zacatek loopu");
    sampling();

    encoder();

    switch (id)
    {
    case 0:
        text("> Main Menu");
        if (menu_nahoru)
            id = 1;
        if (menu_dolu)
            id = 4;
        break;
    case 1:
        text("> Pomodoro");
        if (menu_nahoru)
            id = 2;
        if (menu_dolu)
            id = 0;
        //if (enter)
        //    checkStavu();
        break;
    case 2:
        text("> Nastaveni");
        if (menu_nahoru)
            id = 3;
        if (menu_dolu)
            id = 1;
        if (enter)
            id = 20;
        break;
    case 3:
        text("> Menu 3");
        if (menu_nahoru)
            id = 4;
        if (menu_dolu)
            id = 2;
        if (enter)
            id = 30;
        break;
    case 4:
        text("> Menu 4");
        if (menu_nahoru)
            id = 0;
        if (menu_dolu)
            id = 3;
        if (enter)
            id = 40;
        break;
    case 10:
        text("> Menu 10");
        if (menu_nahoru)
            id = 11;
        if (menu_dolu)
            id = 12;
        break;
    case 11:
        text("> Menu 11");
        if (menu_nahoru)
            id = 12;
        if (menu_dolu)
            id = 10;
        break;
    case 12:
        text("> Menu 12");
        if (menu_nahoru)
            id = 13;
        if (menu_dolu)
            id = 11;
        if (enter)
            id = 1;
        break;
    case 13:
        text("> Exit");
        if (menu_nahoru)
            id = 14;
        if (menu_dolu)
            id = 12;
        if (enter)
            id = 1;
        break;
    }
    Serial.println("Konec loopu");
}

void text(String text)
{
    if (id != prev_id)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(text);
        Serial.println("zmena textu");
        prev_id = id;

        Serial.println(id);
    }
}

void sampling()
{
    Serial.println("Sampling");
    unsigned long curr_millis = micros();

    if (curr_millis - prev_millis >= SAMP)
    {
        tick = HIGH;
        prev_millis = curr_millis;
    }
    else
    {
        tick = LOW;
    }
}

void encoder()
{
    Serial.println("Kontrola stavu encoderu");
    aktualEncoder = digitalRead(CLK);

    if (aktualEncoder == 1 && aktualEncoder != predchEncoder)
    {
        Serial.println("Jakym smerem se encoder pohnul");
        if (digitalRead(DT) != aktualEncoder)
        {
            menu_nahoru = HIGH;
            Serial.println("Menu nahoru");
        }
        else
        {
            menu_dolu = HIGH;
            Serial.println("Menu dolu");
        }
    }
    else if (stavTlac == LOW)
    {
        Serial.println("Tlacitko zmacknuto...");
        if (millis() - prev_tlac > 50)
        {
            enter = HIGH;
            Serial.println("Tlacitko zmacknuto");
        }
        prev_tlac = millis();
    }
    else
    {
        Serial.println("Vsechno low");
        menu_dolu = LOW;
        menu_nahoru = LOW;
        enter = LOW;
    }
    predchEncoder = aktualEncoder;
}