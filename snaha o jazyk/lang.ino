#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>

#define CLK 2
#define DT 3
#define SW 4
#define SAMP 50
#define buzzer 8
File fp_stats;

void encoder();         // Reading rotary encoder
void sampling();        // 1 millis sampling
void reset();           // Reseting controls
void menu();            // Menu overwrite
void odpocet();         // Countdown
void pomodoro();        // Pomodoro countdown
void pause();           // Pause countdown
int end();              // End screen + writing stats to SD card
int checkStavu();       // Checking how many pomodoros left
int timeSettings();     // Setting pomodoro and pause time
int settings();         // Actuall setting
int pomodoroSettings(); // Setting number of settings
void printTime();       // Time printing on LCD
void writeStats();       // Writing data to SD card

LiquidCrystal_I2C lcd(0x27, 20, 4);

//language
String opt0, opt1,opt2, opt3, opt20, opt21, opt22, opt23;
String rem, paus, ende, success, to_start_pause, to_start_pomodoro;
String Press_Button, time_settings, out;
String num_before, num_now, classroom, version;
byte lang = true;

// Pomocné proměné, poté vyřešit
byte stav = false;
int pointer = 1;
unsigned long cas;
unsigned long timeNow;
int nastav;
int idPomodoro;
int cislo;
int newTarget;
int prevTarget;
unsigned long myTime;

// Menu
byte id = 1;
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
long counter, interval;
long pomodoroTime = 5000;
long pauseTime = 10000;
byte state;
int target = 3;
int numPomodoro = 0;
long mytime;

void setup()
{
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT_PULLUP);
    pinMode(DT, INPUT);
    pinMode(buzzer, OUTPUT);

    Serial.begin(9600);
    while (!Serial)
    {
    }
    cas = millis();

    prevEnc = digitalRead(CLK);
    language();
    lcd.init();
    lcd.backlight();
    /*lcd.setCursor(0, 0);
            lcd.print("POMODORO");
            delay(5000);
            lcd.clear();*/
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(opt0);
    lcd.setCursor(0, 1);
    lcd.print(F(">"));
    lcd.print(opt1);
    lcd.setCursor(1, 2);
    lcd.print(opt2);
    lcd.setCursor(1, 3);
    lcd.print(opt3);
    SD.begin(10);
    if (!SD.exists("STATS.CSV"))
    {
        fp_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (fp_stats)
        {
            fp_stats.println("Pomodoro,Hours,Minutes,Seconds,Repeats,Success");
            fp_stats.close();
        }
    }
}

void loop()
{
    sampling();
    if (tick == HIGH)
    {
        encoder();
        menu();
        switch (id)
        {
            /*    case 0: // Main menu
                    if (encUp)
                    {
                        lcd.setCursor(0, 0);
                        lcd.print(F(" "));
                        lcd.setCursor(0, 1);
                        lcd.print(F(">"));
                        id = 1;
                    }
                    if (encDown)
                    {
                        lcd.setCursor(0, 0);
                        lcd.print(F(" "));
                        lcd.setCursor(0, 3);
                        lcd.print(F(">"));
                        id = 3;
                    }
                    break;*/
        case 1: // Pomodoro
            if (encUp)
            {
                lcd.setCursor(0, 1);
                lcd.print(F(" "));
                lcd.setCursor(0, 2);
                lcd.print(F(">"));
                id = 2;
            }
            if (encDown)
            {
                lcd.setCursor(0, 1);
                lcd.print(F(" "));
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
                id = 3;
            }
            if (enter)
            {
                pomodoro();
            }
            break;
        case 2: // Settings
            if (encUp)
            {
                lcd.setCursor(0, 2);
                lcd.print(F(" "));
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
                id = 3;
            }
            if (encDown)
            {
                lcd.setCursor(0, 2);
                lcd.print(F(" "));
                lcd.setCursor(0, 1);
                lcd.print(F(">"));
                id = 1;
            }
            if (enter)
            {
                stav = true;
                id = 20;
            }
            break;
        case 3: // About
            if (encUp)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(" "));
                lcd.setCursor(0, 1);
                lcd.print(F(">"));
                id = 1;
            }
            if (encDown)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(" "));
                lcd.setCursor(0, 2);
                lcd.print(F(">"));
                id = 2;
            }
            if (enter)
            {
                about();
            }
            break;
        case 20: // Pomodoro settings
            if (encUp)
            {
                lcd.setCursor(0, 0);
                lcd.print(F(" "));
                lcd.setCursor(0, 1);
                lcd.print(F(">"));
                id = 21;
            }
            if (encDown)
            {
                lcd.setCursor(0, 0);
                lcd.print(F(" "));
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
                id = 23;
            }
            if (enter)
            {
                pomodoroSettings();
            }
            break;
        case 21: // Pomodoro length
            if (encUp)
            {
                lcd.setCursor(0, 1);
                lcd.print(F(" "));
                lcd.setCursor(0, 2);
                lcd.print(F(">"));
                id = 22;
            }
            if (encDown)
            {
                lcd.setCursor(0, 1);
                lcd.print(F(" "));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));
                id = 20;
            }
            if (enter)
            {
                timeSettings();
            }
            break;
        case 22: // Pause length
            if (encUp)
            {
                lcd.setCursor(0, 2);
                lcd.print(F(" "));
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
                id = 23;
            }
            if (encDown)
            {
                lcd.setCursor(0, 2);
                lcd.print(F(" "));
                lcd.setCursor(0, 1);
                lcd.print(F(">"));
                id = 21;
            }
            if (enter)
            {
                timeSettings();
            }
            break;
        case 23: // Exit
            if (encUp)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(" "));
                lcd.setCursor(0, 0);
                lcd.print(F(">"));
                id = 20;
            }
            if (encDown)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(" "));
                lcd.setCursor(0, 2);
                lcd.print(F(">"));
                id = 22;
            }
            if (enter)
            {
                lcd.setCursor(0, 3);
                lcd.print(F(" "));
                lcd.setCursor(0, 1);
                lcd.print(F(">"));
                stav = true;
                id = 1;
            }
            break;
        }
    }
}

void menu()
{

    //Serial.println(id);
    //Serial.println(stav);
    if (id >= 0 && id <= 3 && stav == true)
    {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(opt0);
        lcd.setCursor(0, 1);
        lcd.print(opt1);
        lcd.setCursor(1, 2);
        lcd.print(opt2);
        lcd.setCursor(1, 3);
        lcd.print(opt3);
        stav = false;
    }
    else if (id >= 20 && id <= 23 && stav == true)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(">"));
        lcd.print(opt20);
        lcd.setCursor(1, 1);
        lcd.print(opt21);
        lcd.setCursor(1, 2);
        lcd.print(opt22);
        lcd.setCursor(1, 3);
        lcd.print(opt23);
        stav = false;
    }
}

void sampling()
{
    unsigned long curr_millis = millis();

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
            // Serial.println("Tlacitko zmacknuto");
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
    mytime = millis() + pomodoroTime + 1000;
    numPomodoro++;
    state = false;
    odpocet();
}

void pause()
{
    // Serial.print("Pause");
    lcd.clear();
    lcd.setCursor(0, 0);
    mytime = millis() + pauseTime + 1000;
    // Serial.println("Prestavka: ");
    lcd.print(F("Prestavka: "));
    state = true;
    odpocet();
}

int end()
{
    // erial.println("End");
    lcd.clear();
    lcd.setCursor(0, 0);
    // Serial.println("Ende");
    lcd.print(F("Ende"));
    delay(1000);
    idPomodoro++;
    // writeData(idPomodoro, hours, minutes, seconds, numPomodoro,true);
    writeData(idPomodoro, 0, 45, 0, 5, 1);
    numPomodoro = 0;
    state = !state;
    id = 1;
    stav = true;
    // Serial.println(id);
}

int checkStavu()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    if (numPomodoro == target)
    {
        lcd.print(F("SUCCESS!"));
    }
    else if (state == false)
    {
        lcd.print(F("To start pause"));
    }
    else if (state == true)
    {
        lcd.print(F("To start pomodoro"));
    }
    lcd.setCursor(0, 1);
    lcd.print(F("PRESS BUTTON!"));
    interval = millis();
    while (true)
    {
        encoder();
        mytime = millis();
        if (mytime - interval >= 250)
        {
            if (stav == false)
            {
                tone(buzzer, 1000);
            }
            else if (stav == true)
            {
                noTone(buzzer);
            }
            stav = !stav;
            interval = mytime;
        }
        if (enter == HIGH)
        {
            noTone(buzzer);
            break;
        }
    }
    if (numPomodoro >= target)
    {
        end();
    }
    else
    {
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
    // Serial.print("time settings");
    byte choice; // Ukazování stříšky
    int pointerPrev = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Nastaveni casu"));
    lcd.setCursor(1, 3);
    lcd.print(F("Exit"));
    lcd.setCursor(1, 1);
    if (id == 21)
    {
        mytime = millis() + pomodoroTime + 1000;
    }
    else if (id == 22)
    {
        mytime = millis() + pauseTime + 1000;
    }
    printTime();
    lcd.setCursor(1, 2);
    lcd.print(F("^^"));

    while (true)
    {
        encoder();
        if (encUp == HIGH)
        {
            pointer++;
            // Serial.println("pointer +");
        }
        else if (encDown == HIGH)
        {
            pointer--;
            // Serial.println("pointer -");
        }
        else if (enter == HIGH)
        {
            if (pointer == 9)
            {
                // interval = (setHours*25) + ((setTenMin * 10 + setMin) * 60000) + ((setTenSec * 10 + setSec) * 1000);
                stav = true;
                id = 20;
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
            // Serial.println("Pointer 1");
        }
        else if (pointer < 1)
        {
            pointer = 9;
            // Serial.println("Pointer 9");
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
            // Serial.println("Pohnuti");
            // Serial.println(pointer);
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
    // Serial.println("Za loopem");
}

int settings()
{
    // Serial.println("Settings");
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
            if (id == 21)
            {
                pomodoroTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);
            }
            else if (id == 22)
            {
                pauseTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);
            }
            // Serial.println(interval);
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
    // Serial.println("pomodoroSettings");
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
            // Serial.println("Target +");
        }
        else if (encDown == HIGH)
        {
            newTarget--;
            // Serial.println("Target -");
        }
        else if (enter == HIGH)
        {
            // Serial.println("enter");
            target = newTarget;
            id = 20;
            stav = true;
            // Serial.println(id);
            break;
        }

        if (newTarget != prevTarget)
        {
            prevTarget = newTarget;
            // Serial.println("Prepis pocet na lcd");
            lcd.setCursor(16, 2);
            lcd.print(F("   "));
            lcd.setCursor(16, 2);
            lcd.print(newTarget);
        }
    }
    // Serial.println("Pryc z while loopu");
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
    // Serial.println("writeData");
    if (SD.exists("STATS.CSV"))
    {
        // Serial.println("if sd exists");
        fp_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (fp_stats)
        {
            //    Serial.println("if fp_stats");
            fp_stats.print(f_pomodoro);
            fp_stats.print(",");
            fp_stats.print(f_hours);
            fp_stats.print(",");
            fp_stats.print(f_minutes);
            fp_stats.print(",");
            fp_stats.print(f_seconds);
            fp_stats.print(",");
            fp_stats.print(f_numPomodoro);
            fp_stats.print(",");
            fp_stats.println(success);
            fp_stats.close();
        }
    }
    // Serial.println("Konec");
}

void about()
{
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(F("Pomodoro Machine"));
    lcd.setCursor(1, 1);
    lcd.print(F("Author: Tomas Mach"));
    lcd.setCursor(5, 2);
    lcd.print(F("Class: 4EB"));
    lcd.setCursor(4, 3);
    lcd.print(F("Version: 1.0"));

    while (true)
    {
        encoder();

        if (enter)
        {
            stav = true;
            id = 1;
            break;
        }
    }
}

String language()
{
    if (lang == true)
    {
        opt0 = "Main menu";
        opt1 = "Pomodoro";
        opt2 = "Settings";
        opt3 = "About";
        opt20 = "Pomodoro settings";
        opt21 = "Pomodoro length";
        opt22 = "Pause length";
        opt23 = "Back";
    }
    else if (lang == false)
    {
        opt0 = "Hlavni menu";
        opt1 = "Pomodoro";
        opt2 = "Nastaveni";
        opt3 = "Informace";
        opt20 = "Nastaveni pomodora";
        opt21 = "Delka pomodora";
        opt22 = "Delka pauzy";
        opt23 = "Zpatky";
    }
}