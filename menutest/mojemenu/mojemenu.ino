#define CLK 2
#define SW 3
#define DT 4
#define SAMP 100

int id;
int prev_id = -1;
unsigned long prev_millis;
byte tick = LOW;

int aktualEncoder;
int predchEncoder;

byte menu_nahoru = LOW;
byte menu_dolu = LOW;
byte enter = LOW;

void setup()
{
    pinMode(CLK, INPUT);
    pinMode(SW, INPUT);
    pinMode(DT, INPUT);

    Serial.begin(9600);
    prev_millis = millis();

    predchEncoder = digitalRead(CLK);
}

void loop()
{
    sampling();

    encoder();

    switch(id)
    {
    case 0:
        text("Main Menu");
        if (menu_nahoru)
            id = 1;
        if (menu_dolu)
            id = 3;
        break;
    case 1:
        text("Menu 1");
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

void text(char *msg)
{
    if ( id != prev_id)
    {
        Serial.println(msg);
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

    if( aktualEncoder == 1 && aktualEncoder != predchEncoder)
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

    if(stavTlac == LOW)
    {
        enter = HIGH;
    }
    else
    {
        enter = LOW;
    }
}