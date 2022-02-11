#include <LiquidCrystal_I2C.h>

// Rotary Encoder Inputs
#define CLK 2
#define DT 3
#define SW 4

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="";
unsigned long lastButtonPress = 0;
unsigned long prev_millis;
byte tick = LOW;

LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {

  lcd.init();
  lcd.backlight();

  pinMode(CLK,INPUT);
  pinMode(DT,INPUT);
  pinMode(SW, INPUT_PULLUP);

  Serial.begin(9600);

  // Read the initial state of CLK
  lastStateCLK = digitalRead(CLK);
}

void loop() {
  
  // Čti aktuální stav CLK
  currentStateCLK = digitalRead(CLK);

  if (currentStateCLK != lastStateCLK  && currentStateCLK == 1 && tick){

    // Enkoder se otáčí protisměru hodinových ručiček
    if (digitalRead(DT) != currentStateCLK) {
      counter --;
      currentDir ="CCW";
    } else {
      // Encoder se otáčí po směru hod. ručiček
      counter ++;
      currentDir ="CW";
    }
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Direction: ");
    lcd.print(currentDir);
    lcd.setCursor(0,2);
    lcd.print(counter);
  }

  // Zapamatuj si poslední směr
  lastStateCLK = currentStateCLK;

  // Čti stav tlačítka
  int btnState = digitalRead(SW);

  //Když je tlačítko zmáčknuto
  if (btnState == LOW) {
    if (millis() - lastButtonPress > 50) {
      lcd.setCursor(0,3);
      lcd.print("Button pressed!");
      Serial.println("Button pressed!");
    }

    // Zapamatovat si kdy bylo tlačítko naposledy stisknuto
    lastButtonPress = millis();
  }

  delay(1);
}
