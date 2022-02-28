#include <LiquidCrystal_I2C.h>
//#include <SD.h>
//#include <SPI.h>

//File pm_stats;

LiquidCrystal_I2C lcd(0x27, 20, 4);

int cislo;

void setup()
{
    Serial.begin(9600);

        lcd.init();
    lcd.backlight();
//    SD.begin(10);
/*    if (!SD.exists("STATS.CSV"))
    {
        pm_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (pm_stats)
        {
            pm_stats.println("Pomodoro,Hours,Minutes,Seconds,Repeats,Success");
            pm_stats.close();
        }
    }*/

}

/*void writeData(int f_pomodoro, int f_hours, int f_minutes, int f_seconds, int f_numPomodoro, int success)
{
    Serial.println("writeData");
    if (SD.exists("STATS.CSV"))
    {
        Serial.println("if sd exists");
        pm_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (pm_stats)
        {
            Serial.println("if pm_stats");
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
    Serial.println("Konec");
}*/

void loop()
{
cislo++;
lcd.clear();
lcd.setCursor(0,0);
lcd.print(cislo);
//writeData(cislo,0,35,0,5,1);
}
