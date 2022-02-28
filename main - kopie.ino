#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

#define CLK 2
#define DT 3
#define SW 4
#define SAMP 100
File pm_stats;

void encoder(); //Reading rotary encoder
void sampling(); //1 millis sampling
void reset(); //Reseting controls
void menu(); //Menu overwrite
void odpocet(); //Countdown
void pomodoro(); //Pomodoro countdown
void pause(); //Pause countdown
int end(); //End screen + writing stats to SD card
int checkStavu(); //Checking how many pomodoros left
int timeSettings(); //Setting pomodoro and pause time
int settings(); //Actuall setting
int pomodoroSettings(); //Setting number of settings
void printTime(); //Time printing on LCD
void writeData(); //Writing data to SD card

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Pomocné proměné, poté vyřešit
byte stav;
int pointer = 1;
unsigned long cas;
unsigned long timeNow;
int nastav;
int idPomodoro;
int cislo;

// Menu
byte id;
byte prev_id = -1;
byte tick = LOW;

// Encoder
unsigned long prevButton;
int actuEnc;
int prevEnc;
byte stateButton = HIGH;

// menu pohyb
byte encUp = LOW;
byte encDown = LOW;
byte enter = LOW;
byte press;

// Pomodoro
int hours, minutes, seconds;
long counter, interval = 5000;
byte state;
int target = 3;
int numPomodoro = 0;
long mytime;

void setup()
{
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT_PULLUP);
    pinMode(DT, INPUT);

    Serial.begin(9600);
    cas = millis();

    prevEnc = digitalRead(CLK);

    lcd.init();
    lcd.backlight();
    /*lcd.setCursor(0, 0);
            lcd.print("POMODORO");
            delay(5000);
            lcd.clear();*/
    SD.begin(10);
    if (!SD.exists("STATS.CSV"))
    {
        pm_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (pm_stats)
        {
            pm_stats.println("Pomodoro,Hours,Minutes,Seconds,Repeats,Success");
            pm_stats.close();
        }
    }
}

void loop()
{

    /*cislo++;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(cislo);
    writeData(cislo, 0, 35, 0, 5, 1);*/

    sampling();
    encoder();

    switch (id)
    {
    case 0:
        menu("> Main Menu");
        if (encUp)
            id = 1;
        if (encDown)
            id = 4;
        break;
    case 1:
        menu("> Pomodoro");
        if (encUp)
            id = 2;
        if (encDown)
            id = 0;
        if (enter)
            checkStavu();
        break;
    case 2:
        menu("> Nastaveni");
        if (encUp)
            id = 3;
        if (encDown)
            id = 1;
        if (enter)
        {
            id = 20;
        }
        break;
    case 3:
        menu("> Menu 3");
        if (encUp)
            id = 4;
        if (encDown)
            id = 2;
        if (enter)
            id = 30;
        break;
    case 4:
        menu("> Menu 4");
        if (encUp)
            id = 0;
        if (encDown)
            id = 3;
        if (enter)
            id = 40;
        break;
    case 20:
        menu("> Pocet pomodor");
        if (encUp)
            id = 21;
        if (encDown)
            id = 23;
        if (enter)
            id = 201;
        break;
    case 21:
        menu("> Delka pomodora");
        if (encUp)
            id = 22;
        if (encDown)
            id = 20;
        if (enter)
            timeSettings();
        break;
    case 22:
        menu("> Delka prestavky");
        if (encUp)
            id = 23;
        if (encDown)
            id = 21;
        if (enter)
            id = 101;
        break;
    case 23:
        menu("> Exit");
        if (encUp)
            id = 20;
        if (encDown)
            id = 22;
        if (enter)
            id = 1;
        break;
    case 201:
        pomodoroSettings();
    }
}

void menu(String text)
{
    if (id != prev_id)
    {
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(text);
        //Serial.println(id);
        prev_id = id;
    }
}

void sampling()
{
    unsigned long curr_millis = micros();

    if (curr_millis - cas >= SAMP)
    {
        tick = HIGH;
        cas = curr_millis;
    }
    else
    {
        tick = LOW;
    }
}

void reset()
{
    encUp = LOW;
    encDown = LOW;
    enter = LOW;
    stateButton = HIGH;
    press = HIGH;
}

void encoder()
{
    reset();

    press = digitalRead(SW);
    if (press == LOW)
    {
        stateButton = LOW;
    }

    actuEnc = digitalRead(CLK);
    if (actuEnc == 1 && actuEnc != prevEnc)
    {
        if (digitalRead(DT) != actuEnc)
        {
            encUp = HIGH;
        }
        else
        {
            encDown = HIGH;
        }
    }
    else if (stateButton == LOW)
    {
        if (millis() - prevButton > 1000)
        {
            enter = HIGH;
            //Serial.println("Tlacitko zmacknuto");
        }
        prevButton = millis();
    }
    prevEnc = actuEnc;
}

void odpocet()
{
    while (mytime >= millis())
    {
        lcd.setCursor(1, 1);
        printTime();
    }
    checkStavu();
}

void pomodoro()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Kolik zbyva: "));
    numPomodoro++;
    odpocet();
}

void pause()
{
    //Serial.print("Pause");
    lcd.clear();
    lcd.setCursor(0, 0);
    //Serial.println("Prestavka: ");
    lcd.print(F("Prestavka: "));
    odpocet();
}

int end()
{
    //erial.println("End");
    lcd.clear();
    lcd.setCursor(0, 0);
    //Serial.println("Ende");
    lcd.print(F("Ende"));
    delay(1000);
    idPomodoro++;
    // writeData(idPomodoro, hours, minutes, seconds, numPomodoro,true);
    writeData(idPomodoro, 0, 45, 0, 5, 1);
    numPomodoro = 0;
    state = !state;
    id = 0;
    //Serial.println(id);
}

int checkStavu()
{
    if (numPomodoro >= target)
    {
        end();
    }
    else
    {
        mytime = millis() + interval + 1000;
        state = !state;
        if (state == true)
        {
            pomodoro();
        }
        else if (state == false)
        {
            pause();
        }
    }
}

int timeSettings()
{
    //Serial.print("time settings");
    int newTarget;
    int prevTarget;
    byte choice; // Ukazování stříšky
    int pointerPrev = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Nastaveni casu"));
    lcd.setCursor(1, 3);
    lcd.print(F("Exit"));
    lcd.setCursor(1, 1);
    mytime = millis() + interval + 1000;
    printTime();
    lcd.setCursor(1, 2);
    lcd.print(F("^^"));

    while (true)
    {
        encoder();
        if (encUp == HIGH)
        {
            pointer++;
            //Serial.println("pointer +");
        }
        else if (encDown == HIGH)
        {
            pointer--;
            //Serial.println("pointer -");
        }
        else if (enter == HIGH)
        {
            if (pointer == 9)
            {
                // interval = (setHours*25) + ((setTenMin * 10 + setMin) * 60000) + ((setTenSec * 10 + setSec) * 1000);
                id = 0;
                // Serial.println(interval);
                break;
            }
            else
            {
                settings();
            }
        }

        if (pointer > 9)
        {
            pointer = 1;
            //Serial.println("Pointer 1");
        }
        else if (pointer < 1)
        {
            pointer = 9;
            //Serial.println("Pointer 9");
        }
        // 00:00:00
        else if (pointer == 2 || pointer == 3 || pointer == 5 || pointer == 6 || pointer == 8)
        {
            pointer++;
        }
        else if (pointer == 2 && encDown == HIGH || pointer == 5 && encDown == HIGH)
        {
            pointer--;
        }

        if (pointer != pointerPrev)
        {
            //Serial.println("Pohnuti");
            //Serial.println(pointer);
            lcd.setCursor(0, 3);
            lcd.print(F(" "));
            lcd.setCursor(pointerPrev, 2);
            lcd.print(F("  "));
            if (pointer == 9)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
            }
            else
            {
                lcd.setCursor(pointer, 2);
                lcd.print(F("^^"));
            }
            pointerPrev = pointer;
        }
    }
    //Serial.println("Za loopem");
}

int settings()
{
    //Serial.println("Settings");
    int prevNastav = nastav;
    cas = timeNow;
    switch (pointer)
    {
    case 1:
        nastav = hours;
        break;
    case 4:
        nastav = minutes;
        break;
    case 7:
        nastav = seconds;
        break;
    }
    while (true)
    {
        unsigned long timeNow = millis();
        //        Serial.println("Settings");
        encoder();
        //        Serial.println(cas);
        //        Serial.println(timeNow);
        if (timeNow - cas >= 1000)
        {
            //            Serial.println("Millis");
            if (stav == false)
            {
                //                Serial.println("Blank");
                lcd.setCursor(pointer, 2);
                lcd.print(F("  "));
                stav = !stav;
            }
            else if (stav == true)
            {
                //                Serial.println("Point");
                lcd.setCursor(pointer, 2);
                lcd.print(F("^^"));
                stav = !stav;
            }
            cas = timeNow;
        }

        if (encUp)
        {
            nastav++;
        }
        else if (encDown)
        {
            nastav--;
        }
        else if (enter)
        {
            switch (pointer)
            {
            case 1:
                hours = nastav;
                break;
            case 4:
                minutes = nastav;
                break;
            case 7:
                seconds = nastav;
                break;
            }

            /*            Serial.println("Break");
                        Serial.print(setHours);
                        Serial.print(":");
                        Serial.print(setTenMin); 
                        Serial.print(setMin);
                        Serial.print(":");
                        Serial.print(setTenSec);
                        Serial.println(setSec);*/
            interval = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);
            //Serial.println(interval);
            lcd.setCursor(pointer, 2);
            lcd.print(F("^^"));
            break;
        }
        if (pointer == 1)
        {
            if (nastav > 99)
            {
                nastav = 0;
            }
            else if (nastav <= -1)
            {
                nastav = 99;
            }
        }
        else
        {
            if (nastav >= 61)
            {
                nastav = 0;
            }
            else if (nastav <= -1)
            {
                nastav = 60;
            }
        }

        if (nastav != prevNastav)
        {
            if (nastav < 10)
            {
                lcd.setCursor(pointer, 1);
                lcd.print(F("0"));
                lcd.print(nastav);
            }
            else if (nastav >= 10)
            {
                lcd.setCursor(pointer, 1);
                lcd.print(nastav);
            }
            prevNastav = nastav;
        }
    }
}

int pomodoroSettings()
{
    //Serial.println("pomodoroSettings");
    int newTarget = 1;
    int prevTarget;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(F("Puvodni pocet: "));
    lcd.print(target);
    lcd.setCursor(0, 2);
    lcd.print(F("Aktualni pocet: "));
    while (true)
    {
        encoder();

        if (encUp == HIGH)
        {
            newTarget++;
            //Serial.println("Target +");
        }
        else if (encDown == HIGH)
        {
            newTarget--;
            //Serial.println("Target -");
        }
        else if (enter == HIGH)
        {
            //Serial.println("enter");
            target = newTarget;
            id = 0;
            //Serial.println(id);
            break;
        }

        if (newTarget != prevTarget)
        {
            prevTarget = newTarget;
            //Serial.println("Prepis pocet na lcd");
            lcd.setCursor(16, 2);
            lcd.print(F("   "));
            lcd.setCursor(16, 2);
            lcd.print(newTarget);
        }
    }
    //Serial.println("Pryc z while loopu");
}

void printTime()
{
    lcd.setCursor(1, 1);
    counter = (mytime - millis()) / 1000;
    hours = counter / 3600;
    counter -= (hours * 3600);
    minutes = counter / 60;
    counter -= (minutes * 60);
    seconds = counter;
    if (hours < 10)
    {
        lcd.print(F("0"));
        lcd.print(hours);
    }
    else
    {
        lcd.print(hours);
    }
    lcd.print(F(":"));
    if (minutes < 10)
    {

        lcd.print(F("0"));
        lcd.print(minutes);
    }
    else
    {
        lcd.print(minutes);
    }
    lcd.print(F(":"));
    if (seconds < 10)
    {
        lcd.print(F("0"));
        lcd.print(seconds);
    }
    else
    {
        lcd.print(seconds);
    }
}

void writeData(int f_pomodoro, int f_hours, int f_minutes, int f_seconds, int f_numPomodoro, int success)
{
    //Serial.println("writeData");
    if (SD.exists("STATS.CSV"))
    {
        //Serial.println("if sd exists");
        pm_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (pm_stats)
        {
        //    Serial.println("if pm_stats");
            pm_stats.print(f_pomodoro);
            pm_stats.print(",");
            pm_stats.print(f_hours);
            pm_stats.print(",");
            pm_stats.print(f_minutes);
            pm_stats.print(",");
            pm_stats.print(f_seconds);
            pm_stats.print(",");
            pm_stats.print(f_numPomodoro);
            pm_stats.print(",");
            pm_stats.println(success);
            pm_stats.close();
        }
    }
    //Serial.println("Konec");
}