#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PINA 2
#define PINB 3
#define PUSHB 12

volatile boolean turned; // rotary was turned
volatile boolean fired;  // knob was pushed
volatile boolean up;     // true when turned cw

int CursorLine = 0;
int DisplayFirstLine = 0;

char *MenueLine[] = {" Option 1", " Option 2", " Option 3", " Option 4", " Option 5"};
char *One[] = {" One-1", " One-2", " One-3", " One-4", " One-5"};
char *Two[] = {" Two-1", " Two-2", " Two-3", " Two-4", " Two-5"};
char *Three[] = {" Three-1", " Three-2", " Three-3", " Three-4", " Three-5"};

int MenueItems;

// Variable for the button's current state.
// button_mode = 1 when the button is up, and 0 when the button is pressed.
// This variable is 'static' and persists because it is declared outside of a function.
int button_mode = 1;

void setup()
{
    digitalWrite(PINA, HIGH); // enable pull-ups
    digitalWrite(PINB, HIGH);
    digitalWrite(PUSHB, HIGH);

    attachInterrupt(3, isr, CHANGE);

    lcd.begin(20, 4);

} // end of setup

void loop()
{

    if (turned)
    {
        if (up)
        {
            move_up();
        }
        else
        {
            move_down();
        }
        turned = false;
    }

    clickPin();

} // end of loop

// Interrupt Service Routine for a change to encoder pin A
void isr()
{
    if (digitalRead(PINA))
    {
        up = digitalRead(PINB);
    }
    else
    {
        up = !digitalRead(PINB);
    }
    turned = true;
} // end of isr

void clickPin()
{

    if ((digitalRead(PUSHB) == LOW) && (button_mode == 1))
    {
        // Button was up before, but is pressed now. Set the button to pressed
        button_mode = 0; // Button is pressed.
        selection();
        fired = false;
    }
    else if ((digitalRead(PUSHB) == HIGH) && (button_mode == 0))
    {
        // Button was down before, but is released now. Set the button to released.
        button_mode = 1;
    }
}

void print_menu()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   Main Menu  ");
    for (int i = 1; i < 4; i++)
    {
        lcd.setCursor(0, i);
        lcd.print(MenueLine[DisplayFirstLine + i]);
    }
    lcd.setCursor(0, (CursorLine - DisplayFirstLine) + 1);
}

void print_menu1()
{
    lcd.clear();
    for (int i = 1; i < 4; i++)
    {
        lcd.setCursor(0, i);
        lcd.print(One[DisplayFirstLine + i]);
    }
    lcd.setCursor(0, (CursorLine - DisplayFirstLine));
}

void print_menu2()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menu option 2");
    for (int i = 1; i < 4; i++)
    {
        lcd.setCursor(0, i);
        lcd.print(Two[DisplayFirstLine + i]);
    }
    lcd.setCursor(0, (CursorLine - DisplayFirstLine));
}

void print_menu3()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Menu option 3");
    for (int i = 1; i < 4; i++)
    {
        lcd.setCursor(0, i);
        lcd.print(Three[DisplayFirstLine + i]);
    }
    lcd.setCursor(0, (CursorLine - DisplayFirstLine));
}

void move_down()
{
    if (CursorLine == (DisplayFirstLine + 4 - 1))
    {
        DisplayFirstLine++;
    }
    if (CursorLine == (MenueItems - 1))
    {
        CursorLine = 0;
        DisplayFirstLine = 0;
    }
    else
    {
        CursorLine = CursorLine + 1;
    }
    print_menu();
}

void move_up()
{
    if ((DisplayFirstLine == 0) & (CursorLine == 0))
    {
        DisplayFirstLine = MenueItems - 4;
    }
    else if (DisplayFirstLine == CursorLine)
    {
        DisplayFirstLine--;
    }
    if (CursorLine == 0)
    {
        CursorLine = MenueItems - 1;
    }
    else
    {
        CursorLine = CursorLine - 1;
    }
    print_menu();
}

void selection()
{

    lcd.clear();
    lcd.print("MY MENU:");
    lcd.setCursor(0, 1);
    lcd.print("You selected:");
    lcd.setCursor(0, 2);
    lcd.print(MenueLine[CursorLine]);
    delay(2000);
    lcd.clear();

    switch (CursorLine)
    {
    case 0:
        First();
        break;
    case 1:
        Second();
        break;
    case 2:
        Third();
        break;
    case 3:
        break;
        default:
        break;
    }
}

void First()
{
    lcd.setCursor(0, 0);
    lcd.print("You are in menu:");
    lcd.setCursor(0, 1);
    lcd.print("ONE");
    print_menu1();
}

void Second()
{
    lcd.setCursor(0, 0);
    lcd.print("You are in menu:");
    lcd.setCursor(0, 1);
    lcd.print("TWO");
    print_menu2();
}

void Third()
{
    lcd.setCursor(0, 0);
    lcd.print("You are in menu:");
    lcd.setCursor(0, 1);
    lcd.print("THREE");
    print_menu3();
}