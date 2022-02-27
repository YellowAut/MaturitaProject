#include <LiquidCrystal_I2C.h>

#define CLK 2
#define DT 3
#define SW 4
#define SAMP 100

LiquidCrystal_I2C lcd(0x27, 20, 4);

// Pomocné proměné, poté vyřešit
byte stav;
int pointer = 1;
unsigned long cas;
unsigned long timeNow;
int nastav;
int setHours, setMin, setSec;
int nevim;

// Menu
int id;
int prev_id = -1;
unsigned long timePrev;
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
    timePrev = millis();

    prevEnc = digitalRead(CLK);

    lcd.init();
    lcd.backlight();
    /*    lcd.setCursor(0, 0);
        lcd.print("POMODORO");
        delay(5000);
        lcd.clear();*/
}

void loop()
{
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
            id = 20;
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
        Serial.println(id);
        prev_id = id;
    }
}

void sampling()
{
    unsigned long curr_millis = micros();

    if (curr_millis - timePrev >= SAMP)
    {
        tick = HIGH;
        timePrev = curr_millis;
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
            Serial.println("Tlacitko zmacknuto");
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
    lcd.print("Kolik zbyva: ");
    numPomodoro++;
    odpocet();
}

void pause()
{
    Serial.print("Pause");
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println("Prestavka: ");
    lcd.print("Prestavka: ");
    odpocet();
}

int end()
{
    Serial.println("End");
    lcd.clear();
    lcd.setCursor(0, 0);
    Serial.println("Ende");
    lcd.print("Ende");
    delay(1000);

    numPomodoro = 0;
    state = !state;
    id = 0;
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
    Serial.print("time settings");
    int newTarget;
    int prevTarget;
    byte choice; // Ukazování stříšky
    int pointerPrev = 1;
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Nastaveni casu");
    lcd.setCursor(1, 3);
    lcd.print("Exit");
    lcd.setCursor(1, 1);
    mytime = millis() + interval + 1000;
    printTime();
    lcd.setCursor(1, 2);
    lcd.print("^^");

    while (true)
    {
        encoder();
        if (encUp == HIGH)
        {
            pointer++;
            Serial.println("pointer +");
        }
        else if (encDown == HIGH)
        {
            pointer--;
            Serial.println("pointer -");
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
            Serial.println("Pointer 1");
        }
        else if (pointer < 1)
        {
            pointer = 9;
            Serial.println("Pointer 9");
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
            Serial.println("Pohnuti");
            Serial.println(pointer);
            lcd.setCursor(0, 3);
            lcd.print(" ");
            lcd.setCursor(pointerPrev, 2);
            lcd.print("  ");
            if (pointer == 9)
            {
                lcd.setCursor(0, 3);
                lcd.print(">");
            }
            else
            {
                lcd.setCursor(pointer, 2);
                lcd.print("^^");
            }
            pointerPrev = pointer;
        }
    }
    Serial.println("Za loopem");
}

int settings()
{
    Serial.println("Settings");
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
                lcd.print("  ");
                stav = !stav;
            }
            else if (stav == true)
            {
                //                Serial.println("Point");
                lcd.setCursor(pointer, 2);
                lcd.print("^^");
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
                setHours = nastav;
                break;
            case 4:
                setMin = nastav;
                break;
            case 7:
                setSec = nastav;
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
            interval = (setHours * 3600000) + (setMin * 60000) + (setSec * 1000);
            Serial.println(interval);
            lcd.setCursor(pointer, 2);
            lcd.print("^^");
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
                lcd.print("0");
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
    Serial.println("pomodoroSettings");
    int newTarget = 1;
    int prevTarget;
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Puvodni pocet: ");
    lcd.print(target);
    lcd.setCursor(0, 2);
    lcd.print("Aktualni pocet: ");
    while (true)
    {
        encoder();

        if (encUp == HIGH)
        {
            newTarget++;
            Serial.println("Target +");
        }
        else if (encDown == HIGH)
        {
            newTarget--;
            Serial.println("Target -");
        }
        else if (enter == HIGH)
        {
            Serial.println("enter");
            target = newTarget;
            id = 20;
            Serial.println(id);
            break;
        }

        if (newTarget != prevTarget)
        {
            prevTarget = newTarget;
            Serial.println("Prepis pocet na lcd");
            lcd.setCursor(16, 2);
            lcd.print("   ");
            lcd.setCursor(16, 2);
            lcd.print(newTarget);
        }
    }
    Serial.println("Pryc z while loopu");
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
        lcd.print("0");
        lcd.print(hours);
    }
    else
    {
        lcd.print(hours);
    }
    lcd.print(":");
    if (minutes < 10)
    {

        lcd.print("0");
        lcd.print(minutes);
    }
    else
    {
        lcd.print(minutes);
    }
    lcd.print(":");
    if (seconds < 10)
    {
        lcd.print("0");
        lcd.print(seconds);
    }
    else
    {
        lcd.print(seconds);
    }
}