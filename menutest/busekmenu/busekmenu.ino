#define PIN_BTN_PREV 2
#define PIN_BTN_ENTER 3
#define PIN_BTN_NEXT 4
#define SAMP 100

int id = 0;
int prev_id = -1;
unsigned long prev_millis;
byte tick = LOW;
byte prev_btn_prev = LOW;
byte prev_btn_enter = LOW;
byte prev_btn_next = LOW;
byte btn_prev_click = LOW;
byte btn_enter_click = LOW;
byte btn_next_click = LOW;

void setup()
{
    pinMode(PIN_BTN_PREV, INPUT);
    pinMode(PIN_BTN_ENTER, INPUT);
    pinMode(PIN_BTN_NEXT, INPUT);
    Serial.begin(9600);
    prev_millis = millis();
}

void loop()
{
    // sampling

    sampling();

    // buttons

    button_prev();
    button_enter();
    button_next();
    switch (id)
    {
    case 0:
        message("> main menu");
        if (btn_next_click)
            id = 1;
        if (btn_prev_click)
            id = 3;
        break;
    case 1:
        message("> menu 1");
        if (btn_next_click)
            id = 2;
        if (btn_prev_click)
            id = 0;
        break;
    case 2:
        message("> menu 2");
        if (btn_next_click)
            id = 3;
        if (btn_enter_click)
            id = 20;
        if (btn_prev_click)
            id = 1;
        break;
    case 20:
        message("> menu 20");
        if (btn_next_click)
            id = 21;
        if (btn_prev_click)
            id = 29;
        break;
    case 21:
        message("> menu 21");
        if (btn_next_click)
            id = 22;
        if (btn_prev_click)
            id = 20;
        break;
    case 22:
        message("> menu 22");
        if (btn_next_click)
            id = 29;
        if (btn_prev_click)
            id = 21;
        break;
    case 29:
        message("Exit");
        if (btn_next_click)
            id = 22;
        if (btn_enter_click)
            id = 2;
        if (btn_prev_click)
            id = 20;
        break;
    case 3:
        message("> menu 3");
        if (btn_next_click)
            id = 0;
        if (btn_prev_click)
            id = 2;
        break;
    }
}

void message(char *msg)
{
    if (id != prev_id) //Aby to neblikalo
    {
        Serial.println(msg);
        prev_id = id;
    }
}

void sampling()
{
    unsigned long curr_millis = millis();

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

void button_prev()
{

    byte curr_btn_prev = digitalRead(PIN_BTN_PREV);

    if (curr_btn_prev != prev_btn_prev && curr_btn_prev == HIGH)
    {
        btn_prev_click = HIGH;
    }
    else
    {
        btn_prev_click = LOW;
    }

    prev_btn_prev = curr_btn_prev;
}

void button_enter()
{

    byte curr_btn_enter = digitalRead(PIN_BTN_ENTER);

    if (curr_btn_enter != prev_btn_enter && curr_btn_enter == HIGH)
    {
        btn_enter_click = HIGH;
    }
    else
    {
        btn_enter_click = LOW;
    }

    prev_btn_enter = curr_btn_enter;
}

void button_next()
{

    byte curr_btn_next = digitalRead(PIN_BTN_NEXT);

    if (curr_btn_next != prev_btn_next && curr_btn_next == HIGH)
    {
        btn_next_click = HIGH;
    }
    else
    {
        btn_next_click = LOW;
    }

    prev_btn_next = curr_btn_next;
}