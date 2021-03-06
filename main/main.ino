////////////////////
//POMODORO MACHINE//
//  VERSION 1.0   //
//   TOMAS MACH   //
////////////////////

#include <LiquidCrystal_I2C.h> // Knihovna pro LCD
#include <SD.h>                // Knihovna pro čtečku SD karet
#include <SPI.h>               //Knihovna užívaná společně s SD.h
#include <EEPROM.h>            //Knihovna pro ukládání nastavovaných dat na Arduino

#define CLK 3
#define DT 4
#define SW 5
#define SAMP 50
#define buzzer 8

// EEPROM adresy pro ukládání
#define idPom 0
#define srepeat 2

#define pomHours 3
#define pomMinutes 4
#define pomSeconds 5
#define pauHours 6
#define pauMinutes 7
#define pauSeconds 8

File fp_stats; //Proměnná pro ukládání statistik na SD kartu

void encoder();         // Čtení rotačního enkodéru
void sampling();        // Sampling na 50 millis
void reset();           // Resetování ovládání
void menu();            // Vypsání menu na LCD
void countdown();       // Odpočet
void pomodoro();        // Vyvolání pomodora
void pause();           // Vyvolání pauzy
void about();           // Vypsání informací
void printTime();       // Vypisování času na LCD
void writeStats();      // Zapisování dat na SD kartu
int end();              // Konečná obrazovka
int stateCheck();       // Kontrola kolik pomodor zbývá + jestli je další pauza nebo pomodoro
int timeSettings();     // Nastavování délky pomodora a přestávky
int settings();         // Nastavování délky pomodora a přestávky
int pomodoroSettings(); // Nastavovení počtu opakování pomodora

LiquidCrystal_I2C lcd(0x27, 20, 4); //Inicializace LCD 20×04

// Pomocné proměné, poté vyřešit
byte stav = false;
int pointer = 1;
unsigned long cas;
unsigned long timeNow;
int nastav;
byte idPomodoro;
int cislo;
int newTarget;
int prevTarget;
//unsigned long myTime;
byte success;

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
byte hours, minutes, seconds;
long counter, interval;
long pomodoroTime;
long pauseTime;
byte state;
byte target;
int numPomodoro = 0;
unsigned long mytime;

void setup()
{
    Serial.begin(9600); //Spuštění serialu
    while (!Serial) //Vyčkat dokud se nezapne Seriová komunikace
    {
    }
    cas = millis(); //Uložení millis do proměnné cas

    //Čtení uložených hodnot z EEPROMu
    EEPROM.get(idPom, idPomodoro); 
    EEPROM.get(srepeat, target);

    EEPROM.get(pomHours, hours);
    EEPROM.get(pomMinutes, minutes);
    EEPROM.get(pomSeconds, seconds);
    pomodoroTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);

    EEPROM.get(pauHours, hours);
    EEPROM.get(pauMinutes, minutes);
    EEPROM.get(pauSeconds, seconds);
    pauseTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);

    //Přiřazení pinů pinů na rotačním enkóderu + bzučáku
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT_PULLUP);
    pinMode(DT, INPUT);
    pinMode(buzzer, OUTPUT);

    //Uložení polohy enkóderu
    prevEnc = digitalRead(CLK);

    //Inicializace LCD + Startovní nápis + Vypsání menu
    lcd.init(); //Inicializace LCD
    lcd.backlight(); //Zapnutí podsvícení LCD
    lcd.setCursor(2, 1); 
    lcd.print(F("POMODORO MACHINE"));
    lcd.setCursor(3,2);
    lcd.print(F("By Tomas Mach"));
    delay(5000); //Úvodní strana na 5 vteřin
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print(F("Main menu"));
    lcd.setCursor(0, 1);
    lcd.print(F(">Pomodoro"));
    lcd.setCursor(1, 2);
    lcd.print(F("Settings"));
    lcd.setCursor(1, 3);
    lcd.print(F("About"));

    SD.begin(10); //Zapnutí čtečky SD karet
    if (!SD.exists("STATS.CSV")) //Zkontroluji, zda-li (ne)existuje soubor stats.csv
    {
        fp_stats = SD.open("STATS.CSV", FILE_WRITE); // Pokud neexistuje, soubor vytvořím
        if (fp_stats)
        {
            fp_stats.println("Pomodoro,Hours,Minutes,Seconds,Repeats,Success"); //Zapsání prvního řádku do nového CSV souboru
            fp_stats.close(); //Zavřu soubor kvůli výkonu a paměti
        }
    }
}

void loop()
{
    sampling(); //Zpomalení programu pro lepší funkčnost
    if (tick == HIGH) //Uběhlo 50 millis
    {
        encoder(); //Ovládání menu
        menu(); //Vypisování menu pokud se potřebuji dostat na jiné menu
        // Menu ID
        switch (id) //Šipka na ukázání polohy na menu
        {
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
                success = true;
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
                stav = true; //Stav na true pro přepsání LCD
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
                stav = true; //Stav na true pro přepsání LCD
                id = 1;
            }
            break;
        }
    }
}

void menu()
{
    if (id >= 0 && id <= 3 && stav == true) //Pokud je id 0 až 3 a stav true, přepiš LCD.
    {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print(F("Main menu"));
        lcd.setCursor(0, 1);
        lcd.print(F(">Pomodoro"));
        lcd.setCursor(1, 2);
        lcd.print(F("Settings"));
        lcd.setCursor(1, 3);
        lcd.print(F("About"));
        stav = false; //Opět hodím stav na false
    }
    else if (id >= 20 && id <= 23 && stav == true) //Pokud je id 20 až 23 a stav true, přepiš LCD.
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(F(">Number of pomodoros"));
        lcd.setCursor(1, 1);
        lcd.print(F("Pomodoro length"));
        lcd.setCursor(1, 2);
        lcd.print(F("Pause length"));
        lcd.setCursor(1, 3);
        lcd.print(F("Back"));
        stav = false; //Opět hodím stav na false
    }
}


void sampling()
{
    unsigned long curr_millis = millis(); //zapíšu aktuální millis do curr_millis

    if (curr_millis - cas >= SAMP) //Zkontroluji, jestli uběhl už SAMP
    {
        tick = HIGH; //Nastavím tick na HIGH a proběhne menu
        cas = curr_millis; //Přepíšu millis
    }
    else
    {
        tick = LOW; //tick na LOW a čeká se se millis opět nenaplní
    }
}

void reset()
{
    //Resetuji všechny hodnoty na 0, aby se mi mé volby neopakovali
    encUp = LOW;
    encDown = LOW;
    enter = LOW;
    stateButton = HIGH;
    press = HIGH;
}

void encoder()
{
    reset(); 

    press = digitalRead(SW); //Čtu, zda-li se nezmáčklo tlačíko
    if (press == LOW) //Pokud je zmáčknuté tlačítko
    {
        stateButton = LOW; //Zapíšu stateButton na LOW. Kontroluji v pozdější podmínce
    }

    actuEnc = digitalRead(CLK); //Aktuální pozice enkóderu
    if (actuEnc == 1 && actuEnc != prevEnc) //Pokud se změnila pozice enkóderu
    {
        if (digitalRead(DT) != actuEnc) //Pokud se otočil enkóder doprava
        {
            encUp = HIGH;
        }
        else //Pokud se otočil enkóder doleva
        {
            encDown = HIGH;
        }
    }
    else if (stateButton == LOW) //Pokud bylo tlačítko předtím zmáčknuto
    {
        if (millis() - prevButton > 250) //A uběhlo 250 millis (Pro lepší funkčnost)
        {
            enter = HIGH; //Nastavím enter na high
        }
        prevButton = millis(); //Zapíšu čas posledního zmáčknutí tlačítka
    }
    prevEnc = actuEnc; //Zapíšu aktuální pozici enkóderu do předchozí pozice enkóderu
}

void countdown()
{
    while (mytime >= millis()) //Dokud neuběhne potřebný čas, budu odpočítávat a vypisovat čas na LCD
    {
        encoder();
        if (enter == HIGH) //Pokud zmáčknu enter, zruším pomodoro a zapíšu statistiky s failem na microSD kartu
        {
            success = false;
            id = 1;
            numPomodoro = target; //Nastavím počet pomodor na Target a díky tomu vyskočím z celého pomodora
            break;
        }
        lcd.setCursor(1, 1);
        printTime(); //Printování času na LCD
    }
    stateCheck(); //Pokud uběhl požadovaný interval času, skočím do checkování stavu
}

void pomodoro()
{
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print(F("Time remaining: "));
    lcd.setCursor(0, 3);
    lcd.print(F(">Exit pomodoro"));
    numPomodoro++; //Přidám jedničku do numPomodoro, které potom porovnávám s cílem
    state = false; //Nastavím state na false, aby byl další odpočet false
    mytime = millis() + pomodoroTime + 1000; //Přepíšu mytime na požadovaný interval pomodora. + 1 vteřina aby to začlo na požadovaném času
    countdown(); //Zapnu odpočet
}

void pause()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Pause remaining: "));
    lcd.setCursor(0, 3);
    lcd.print(F(">Exit pause"));
    state = true; //Nastavím state na true, aby byl další odpočet pomodoro
    mytime = millis() + pauseTime + 1000; //Přepíšu mytime na požadovaný interval pauzy. + 1 vteřina aby to začlo na požadovaném čase
    countdown(); //zapnu odpočet
}

int end()
{
    //Vyvolám tuto funkci pokud bylo pomodoro ukončeno
    lcd.clear();
    lcd.setCursor(0, 0);
    idPomodoro++; //Přidám 1 do idPomodoro. Lepší orientace v statistikách
    writeStats(); //Zapsat statistiky
    EEPROM.write(idPom, idPomodoro); //Zapíšu idPomodora do EEPROM, abych pokračoval od původního i po restartu Arduina
    numPomodoro = 0; //Nastavím numPomodoro na 0, ať mohu opět spustit pomodoro
    state = !state; //Otočení state, aby další bylo pomodoro
    id = 1; //Skočím zpátky do menu na pozici č.1 (Pomodoro)
    stav = true; //Stav na true aby se mi opět vypsalo menu
}

int stateCheck()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    if (numPomodoro == target && success == true) //Pokud ukončím pomodoro úspěšně
    {
        lcd.print(F("SUCCESS!"));
        success = 1; //Nastavím success na 1 pro zapsání neúspěchu na SD kartu
    }
    else if (numPomodoro == target && success == false) //Pokud ukončím pomodoro neúspěšně
    {
        lcd.print(F("FAIL!"));
        success = 0; //Nastavím success na 0 pro zapsání neúspěchu na SD kartu
    }
    else if (state == false) //Pokud je state false, další odpočet je pauza
    {
        lcd.print(F("To start pause"));
    }
    else if (state == true) //Pokud je state na true, další odpočet je pomodoro
    {
        lcd.print(F("To start pomodoro"));
    }
    lcd.setCursor(0, 1);
    lcd.print(F("PRESS BUTTON!"));
    interval = millis(); 
    while (true) //Čekám na potvrzení uživatele, že chce začít další odpočet
    {
        encoder();
        mytime = millis();
        if (mytime - interval >= 250) //Každou 1/4 vteřiny pípá bzučák
        {
            if (stav == false)
            {
                tone(buzzer, 1000); //Zapnutí bzučení po 250 milisekundách
            }
            else if (stav == true)
            {
                noTone(buzzer); //Vypnutí bzučení po 250 milisekundách
            }
            stav = !stav;
            interval = mytime; //Přepsání millis
        }
        if (enter == HIGH)
        {
            noTone(buzzer); //Vypnutí bzučáku
            break; 
        }
    }
    if (numPomodoro >= target) //Pokud je počet pomodor splněný (toto se sepne i pokud jsem ukončil předčasně), skočím na end
    {
        end();
    }
    else
    {
        if (state == true) //Pokud je state true, začnu odpočet pomodora
        {
            pomodoro(); //Počátek pomodora
        }
        else if (state == false) //Pokud je state false, začnu odpočet pauzy
        {
            pause(); //Počátek pauzy
        }
    }
}

int timeSettings()
{
    byte choice; //Pozice stříšky
    int pointerPrev = 1; //Předchozí pozice stříšky
    lcd.clear();
    lcd.setCursor(1, 0);
    if (id == 21) //Pokud jsem zvolil nastavování pomodora
    {
        lcd.print(F("Pomodoro time"));
    }
    else if (id == 22) //Pokud jsem zvolil nastavování pauzy
    {
        lcd.print(F("Pause time"));
    }
    lcd.setCursor(1, 3);
    lcd.print(F("Save and Exit")); //Možnost save and exit na spodku LCD
    lcd.setCursor(1, 1);
    if (id == 21) //Pokud jsem zvolil nastavení pomodora, vypíšu na LCD předchozí čas pomodora
    {
        mytime = millis() + pomodoroTime + 1000;
    }
    else if (id == 22) //Pokud jsem zvolil nastavení pauzy, vypíšu na LCD předchozí čas pauzy
    {
        mytime = millis() + pauseTime + 1000;
    }
    printTime(); //Vytisknutí času
    lcd.setCursor(1, 2);
    lcd.print(F("^^")); //Vytisknu stříšky na první pozici (nastavení hodin)

    while (true) //While loop dokud nezmáčknu tlačítko na exit
    {
        encoder();
        if (encUp == HIGH) //Pokud otočím enkoderem doprava
        {
            pointer++; //Přidám 1 k pointeru
        }
        else if (encDown == HIGH) //Pokud otočím enkoderem doleva
        {
            pointer--; //Odečtu 1 od pointeru
        }
        else if (enter == HIGH)
        {
            if (pointer == 9) //Jestli ukazuji pointerem na exit
            {
                // interval = (setHours*25) + ((setTenMin * 10 + setMin) * 60000) + ((setTenSec * 10 + setSec) * 1000);
                stav = true; //nastavím stav na true pro vytisknutí menu
                id = 20; 
                break; //Opustím while loop
            }
            else
            {
                settings(); //Funkce pro samotné nastavování času
            }
        }

        if (pointer > 9) //Pokud je pointer větší než 9, nastavím opět na 1
        {
            pointer = 1;
        }
        else if (pointer < 1) //Pokud je pointer menší než 1, nastavím opět na 9
        {
            pointer = 9;
        }
        // 00:00:00
        //Potřebuji přeskakovat dvojtečky u času a jelikož ukazuji na 2 znaky zároveň, musí zde být více podmínek
        else if (pointer == 2 || pointer == 3 || pointer == 5 || pointer == 6 || pointer == 8) 
        {
            pointer++;
        }
        else if (pointer == 2 && encDown == HIGH || pointer == 5 && encDown == HIGH) //Proč jsem to tady napsal jinak?
        {
            pointer--;
        }

        if (pointer != pointerPrev) //Pokud se pointer změnil
        {
            //Automaticky přepisuji místo před Exitem, vyhnu se tím if podmínce
            lcd.setCursor(0, 3);
            lcd.print(F(" "));
            //Vymažu předchozí šipky
            lcd.setCursor(pointerPrev, 2);
            lcd.print(F("  "));
            if (pointer == 9) //Pokud je pointer roven 9, vypíšu šipku k exitu
            {
                lcd.setCursor(0, 3);
                lcd.print(F(">"));
            }
            else //Jinak vypisuji stříšky k požadovanému času
            {
                lcd.setCursor(pointer, 2);
                lcd.print(F("^^"));
            }
            pointerPrev = pointer; //Zapíšu aktuální místo pointeru do proměnné
        }
    }
}

int settings()
{
    int prevNastav = nastav; //Proměnná využívaná pro přepisování hodnoty nastavení
    cas = timeNow; //Přepsání millis
    switch (pointer) //Nastavujeme hodiny, minuty a nebo vteřiny?
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
        unsigned long timeNow = millis(); //Přepsání millis
        encoder(); //Ovládání enkóderu
        if (timeNow - cas >= 1000)
        {
            //Problikávání šipek každou vteřinu
            if (stav == false)
            {
                lcd.setCursor(pointer, 2);
                lcd.print(F("  "));
                stav = !stav;
            }
            else if (stav == true)
            {
                lcd.setCursor(pointer, 2);
                lcd.print(F("^^"));
                stav = !stav;
            }
            cas = timeNow; //Přepsání millis
        }

        if (encUp)
        {
            nastav++; //Hodiny/minuty/vteřiny +
        }
        else if (encDown)
        {
            nastav--; //Hodiny/minuty/vteřiny -
        }
        else if (enter)
        {
            //Uložení nastavené hodnoty na správné místo
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
            if (id == 21) //Uložení nastavené hodnoty do pomodora
            {
                EEPROM.write(pomHours, hours);
                EEPROM.write(pomMinutes, minutes);
                EEPROM.write(pomSeconds, seconds);
                pomodoroTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);
            }
            else if (id == 22) //Uložení nastavené hodnoty do pazy
            {
                EEPROM.write(pauHours, hours);
                EEPROM.write(pauMinutes, minutes);
                EEPROM.write(pauSeconds, seconds);
                pauseTime = (hours * 3600000) + (minutes * 60000) + (seconds * 1000);
            }
            lcd.setCursor(pointer, 2);
            lcd.print(F("^^")); //Vytisknutí šipek pokud zrovna nebyly zobrazené
            break;
        }
        if (pointer == 1) //Pokud nastavuji hodiny, mohu nastavovat od 0 do 99
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
        else //Pokud nastavuji minuty/vteřiny, mohu nastavovat od 0 do 59
        {
            if (nastav >= 60)
            {
                nastav = 0;
            }
            else if (nastav <= -1)
            {
                nastav = 59;
            }
        }

        if (nastav != prevNastav) //Pokud jsem otočil enkoderem a změnila se hodnota, přepiš starou
        {
            if (nastav < 10) //Pokud přesáhnu hodnotu 10, napíšu upravím desítky
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
            prevNastav = nastav; //Přepsání předešlé hodnoty na stav
        }
    }
}

int pomodoroSettings()
{
    int newTarget = 1; //Proměnná pro změnu počtu
    int prevTarget; //Proměnná pro přepisování hodnoty
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(F("Repeats before: "));
    lcd.print(target); //Vypíšu předchozí počet opakování
    lcd.setCursor(0, 2);
    lcd.print(F("Repeats now: "));
    lcd.print(target); //Vypíšu aktuální počet opakování
    while (true)
    {
        encoder();

        if (encUp == HIGH) //Pokud otočím doprava, přidám 1 opakování
        {
            newTarget++;
        }
        else if (encDown == HIGH) //Pokud otočím doleva, uberu 1 opakování
        {
            newTarget--;
        }
        else if (enter == HIGH) //Pokud stisknu enter
        {
            target = newTarget; //Uložím počet opakování
            EEPROM.write(srepeat, target); //Uložím počet opakování do EEPROM paměti
            id = 20; //Vrátím se na menu nastavení
            stav = true; //Vypíšu menu nastavení
            break;
        }

        if (newTarget != prevTarget) //Pokud se změnila hodnota nastavení, smažu starou a vypíšu novou
        {
            prevTarget = newTarget;
            lcd.setCursor(13, 2);
            lcd.print(F("   "));
            lcd.setCursor(13, 2);
            lcd.print(newTarget);
        }
    }
}

void printTime() //Tato část kódu je odsud: https://forum.arduino.cc/t/how-to-display-hh-mm-ss-on-lcd/279405/3
{
    lcd.setCursor(1, 1);
    counter = (mytime - millis()) / 1000; //Vydělím si počet millis 1000
    hours = counter / 3600; //Pokud chceme dosáhnout hodin, dělíme millis hodnotou 3.600.000. Uložím do hodin
    counter -= (hours * 3600); //Vynásobím hodnotou 3600
    minutes = counter / 60; //A poté vydělím 60 pro získání minut
    counter -= (minutes * 60); //Minuty vynásobím 60 a získám vteřiny
    seconds = counter; //Uložím vteřiny
    if (hours < 10) //Pokud je hodin méně než 10, vypíšu pouze jednotky hodin
    {
        lcd.print(F("0"));
        lcd.print(hours);
    }
    else //Pokud je víc jak 10, vypíšu i desítky hodin. To stejné se opakuje u minut i vteřin
    {
        lcd.print(hours);
    }
    lcd.print(F(":")); //Mezi hodinami, minutami a vteřinami vypíšu dvojtečku
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

void writeStats() //Zapsání statistik na SD kartu
{
    if (SD.exists("STATS.CSV")) //Pokud existuje stats.csv na SD kartě
    {
        fp_stats = SD.open("STATS.CSV", FILE_WRITE); //Otevřu soubor v režimu zapisování
        if (fp_stats) //Pokud je soubor otevřen
        {
            //Přepočtu millis na hodiny, vteřiny a minuty
            mytime = millis() + pomodoroTime;
            counter = (mytime - millis()) / 1000;
            hours = counter / 3600;
            counter -= (hours * 3600);
            minutes = counter / 60;
            counter -= (minutes * 60);
            seconds = counter;

            //Následně uložím id Pomodora, hodiny, minuty, vteřiny, počet opakování a jestli bylo pomodoro úspěšné
            fp_stats.print(idPomodoro);
            fp_stats.print(","); //Mezi každou hodnotou napíšu čárku
            fp_stats.print(hours);
            fp_stats.print(",");
            fp_stats.print(minutes);
            fp_stats.print(",");
            fp_stats.print(seconds);
            fp_stats.print(",");
            fp_stats.print(numPomodoro);
            fp_stats.print(",");
            fp_stats.println(success);
            fp_stats.close();
        }
        fp_stats.close(); //Zavřu soubor z důvodu šetření paměti a výkonu
    }
}

void about() //Vypsání informací o projektu
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

    while (true) //Čekám na zmáčknutí tlačítka. Poté vypíšu hlavní menu
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