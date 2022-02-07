#include <LiquidCrystal_I2C.h>

#define CLK 2
#define SW 3
#define DT 4
#define SAMP 100

LiquidCrystal_I2C lcd(0x27, 20, 4);

int id;
int prev_id = -1;
unsigned long prev_millis;
byte tick = LOW;

int aktualEncoder;
int predchEncoder;

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
    pinMode(SW, INPUT);
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
    sampling();

    encoder();

    switch (id)
    {
    case 0:
        text("Main Menu");
        if (menu_nahoru)
            id = 1;
        if (menu_dolu)
            id = 3;
        break;
    case 1:
        text("Pomodoro");
        if (menu_nahoru)
            id = 2;
        if (menu_dolu)
            id = 0;
        if (enter)
            id = 10;
        break;
    case 2:
        text("Menu 2");
        if (menu_nahoru)
            id = 3;
        if (menu_dolu)
            id = 1;
        if (enter)
            id = 20;
        break;
    case 3:
        text("Menu 3");
        if (menu_nahoru)
            id = 4;
        if (menu_dolu)
            id = 2;
        if (enter)
            id = 30;
        break;
    case 4:
        text("Menu 4");
        if (menu_nahoru)
            id = 0;
        if (menu_dolu)
            id = 3;
        if (enter)
            id = 40;
        break;
    case 10:
        text("Menu 10");
        if (menu_nahoru)
            id = 11;
        if (menu_dolu)
            id = 12;
        break;
    case 11:
        text("Menu 11");
        if (menu_nahoru)
            id = 12;
        if (menu_dolu)
            id = 10;
        break;
    case 12:
        text("Menu 12");
        if (menu_nahoru)
            id = 13;
        if (menu_dolu)
            id = 11;
        if (enter)
            id = 1;
        break;
    case 13:
        text("Exit");
        if (menu_nahoru)
            id = 14;
        if (menu_dolu)
            id = 12;
        if (enter)
            id = 1;
        break;
    }
}

void text(String text)
{
    if (id != prev_id)
    {
        Serial.println(text);
        prev_id = id;
    }
}

void sampling()
{
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
    aktualEncoder = digitalRead(CLK);

    if (aktualEncoder == 1 && aktualEncoder != predchEncoder)
    {
        if (digitalRead(DT) != aktualEncoder)
        {
            menu_nahoru = HIGH;
        }
        else
        {
            menu_dolu = HIGH;
        }
    }
    else
    {
        menu_dolu = LOW;
        menu_nahoru = LOW;
    }
    predchEncoder = aktualEncoder;

    int stavTlac = digitalRead(SW);

    if (stavTlac == LOW)
    {
        enter = HIGH;
    }
    else
    {
        enter = LOW;
    }
}

void odpocet()
{
    while (mytime >= millis())
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
        if (seconds < 10)
        {
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
    pocetPomodor++;
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
    cil = pocetPomodor + 3;
    stav = !stav;
}

int checkStavu()
{
    if (pocetPomodor >= cil)
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