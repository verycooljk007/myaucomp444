#include <Servo.h>

Servo myServo;

int potPin = A0;       // Potentiometer connected to analog pin A0
int servoPin = 9;      // Servo signal connected to pin 9

int angle = 0;         // Current servo angle

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  // Read the potentiometer (range: 0 to 1023)
  int potValue = analogRead(potPin);

  // Map potentiometer value to speed delay (slow = 20ms, fast = 1ms)
  int speedDelay = map(potValue, 0, 1023, 20, 1);

  // Move to the next angle
  angle++;

  // Wrap around after 180°
  if (angle > 180) {
    angle = 0;
  }

  // Move the servo
  myServo.write(angle);

  // Wait before the next step, based on speed
  delay(speedDelay);
}
