// PIN VARIABLES
const int AIN1 = 13;    // Motor direction control pin 1
const int AIN2 = 12;    // Motor direction control pin 2
const int PWMA = 11;    // Motor speed control (PWM)
const int switchPin = 7; // Switch input

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP); // Use internal pull-up resistor
}

void loop() {
  if (digitalRead(switchPin) == LOW) {
    // Switch is ON (pulled LOW), run motor forward at full speed
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 255);
  } else {
    // Switch is OFF, stop motor
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);
  }
}
