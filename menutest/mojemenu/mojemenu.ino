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

unsigned long prevButton;
int actuEnc;
int prevEnc;

byte encUp = LOW;
byte encDown = LOW;
byte enter = LOW;

int hours, minutes, seconds;
long counter, interval = 5000;
bool stav;
int target = 3;
int numPomodoro = 0;
long mytime;

void setup()
{
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT);
    pinMode(DT, INPUT);

    Serial.begin(9600);
    prev_millis = millis();

    prevEnc = digitalRead(CLK);

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
        if (encUp)
            id = 1;
        if (encDown)
            id = 4;
        break;
    case 1:
        text("Pomodoro");
        if (encUp)
            id = 2;
        if (encDown)
            id = 0;
        //if (enter)
        //    checkStavu();
        break;
    case 2:
        text("Nastaveni");
        if (encUp)
            id = 3;
        if (encDown)
            id = 1;
        if (enter)
            id = 20;
        break;
    case 3:
        text("Menu 3");
        if (encUp)
            id = 4;
        if (encDown)
            id = 2;
        if (enter)
            id = 30;
        break;
    case 4:
        text("Menu 4");
        if (encUp)
            id = 0;
        if (encDown)
            id = 3;
        if (enter)
            id = 40;
        break;
    case 10:
        text("Menu 10");
        if (encUp)
            id = 11;
        if (encDown)
            id = 12;
        break;
    case 11:
        text("Menu 11");
        if (encUp)
            id = 12;
        if (encDown)
            id = 10;
        break;
    case 12:
        text("Menu 12");
        if (encUp)
            id = 13;
        if (encDown)
            id = 11;
        if (enter)
            id = 1;
        break;
    case 13:
        text("Exit");
        if (encUp)
            id = 14;
        if (encDown)
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
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(text);
        Serial.println(text);
        prev_id = id;
        Serial.println(id);
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
    actuEnc = digitalRead(CLK);

    if (actuEnc == 1 && actuEnc != prevEnc)
    {
        if (digitalRead(DT) != actuEnc)
        {
            encUp = HIGH;
            Serial.println("Menu nahoru");
        }
        else
        {
            encDown = HIGH;
            Serial.println("Menu dolu");
        }
    }
    else
    {
        encDown = LOW;
        encUp = LOW;
    }
    prevEnc = actuEnc;

    int stavTlac = digitalRead(SW);

    if (stavTlac == LOW)
    {
        if (millis() - prevButton > 50)
        {
            enter = HIGH;
            Serial.println("Tlacitko zmacknuto");
        }
        prevButton = millis();
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
    Serial.println("Konec pomodora");
    lcd.print("Konec pomodora");
    delay(1000);
    target = numPomodoro + 3;
    stav = !stav;
    id = 0;
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
