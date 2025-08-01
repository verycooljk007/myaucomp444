#include <LiquidCrystal.h>  // include the LCD library

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // define LCD pin connections

void setup() {
  lcd.begin(16, 2);     // initialize 16x2 LCD
  lcd.clear();          // clear any previous text
}

void loop() {
  lcd.setCursor(0, 0);              // top row
  lcd.print("How heavy are you");

  int fakeWeight = (millis() / 1000) % 300;  // simulate a weight up to 299 lbs

  lcd.setCursor(0, 1);              // bottom row
  lcd.print("(");
  lcd.print(fakeWeight);
  lcd.print(") lbs   ");  // extra spaces to clear leftover characters
}
