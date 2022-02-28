#include <LiquidCrystal_I2C.h>
//#include <SD.h>
//#include <SPI.h>

//File fp_stats;

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
        fp_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (fp_stats)
        {
            fp_stats.println("Pomodoro,Hours,Minutes,Seconds,Repeats,Success");
            fp_stats.close();
        }
    }*/

}

/*void writeData(int f_pomodoro, int f_hours, int f_minutes, int f_seconds, int f_numPomodoro, int success)
{
    Serial.println("writeData");
    if (SD.exists("STATS.CSV"))
    {
        Serial.println("if sd exists");
        fp_stats = SD.open("STATS.CSV", FILE_WRITE);
        if (fp_stats)
        {
            Serial.println("if fp_stats");
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
