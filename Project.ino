// === Pin Configuration ===
const int AIN1 = 13;
const int AIN2 = 12;
const int PWMA = 11;
const int PWMB = 10;
const int BIN2 = 9;
const int BIN1 = 8;
const int trigPin = 6;
const int echoPin = 5;
const int switchPin = 7;

// === Motion & Scan Settings ===
const int scanStepAngle = 30;
const int scanStepTime = 300;
const int driveSpeed = 200;
const int turnSpeed = 200;
const float closeThreshold = 10.0;  // Consider "arrived" when closer than this

// === Mapping Memory Settings ===
const int numAngles = 360 / scanStepAngle;
int blockedDirection[numAngles] = {0};  // Penalty score per direction

// === Stuck Detection Settings ===
const unsigned long checkInterval = 500;
const int noProgressLimit = 3;
const float progressThreshold = 3.0;

// === State Variables ===
float distance = 0;
float targetDistance = 0;
bool reachedTarget = true;

/********************************************************************************/
void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);

  pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT); pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT); pinMode(PWMB, OUTPUT);

  Serial.begin(9600);
  Serial.println("SmartNavigatorBot Ready.");
}

/********************************************************************************/
void loop() {
  if (digitalRead(switchPin) == LOW) {

    distance = getDistance();
    Serial.print("Current Distance: ");
    Serial.println(distance);

    if (reachedTarget) {
      scanForFurthestDirection();
      reachedTarget = false;
    }

    if (distance < closeThreshold) {
      Serial.println("Close to target. Re-evaluating...");
      rotateBriefly();
      reachedTarget = true;
    } else {
      tryMovingForwardWithMonitoring();
    }

    decayBlockedMemory();  // Gradually forget old collisions

  } else {
    stopMotors();
  }

  delay(100);
}

/********************************************************************************/
void scanForFurthestDirection() {
  float maxScore = -9999;
  int bestAngleIndex = 0;

  Serial.println("Scanning for best direction...");

  for (int i = 0; i < numAngles; i++) {
    rightMotor(turnSpeed);
    leftMotor(-turnSpeed);
    delay(scanStepTime);
    stopMotors();
    delay(100);

    float d = getDistance();
    float score = d - blockedDirection[i] * 10;

    Serial.print("Angle ");
    Serial.print(i * scanStepAngle);
    Serial.print(" | Dist: ");
    Serial.print(d);
    Serial.print(" | Penalty: ");
    Serial.print(blockedDirection[i]);
    Serial.print(" | Score: ");
    Serial.println(score);

    if (score > maxScore) {
      maxScore = score;
      bestAngleIndex = i;
    }
  }

  Serial.print("Best direction: ");
  Serial.print(bestAngleIndex * scanStepAngle);
  Serial.print("°, Score: ");
  Serial.println(maxScore);

  rightMotor(turnSpeed);
  leftMotor(-turnSpeed);
  delay(bestAngleIndex * scanStepTime);
  stopMotors();

  targetDistance = getDistance();
}

/********************************************************************************/
void tryMovingForwardWithMonitoring() {
  int noProgressCount = 0;
  float previousDistance = getDistance();

  rightMotor(driveSpeed);
  leftMotor(driveSpeed);

  unsigned long startTime = millis();
  while (millis() - startTime < 3000) {
    delay(checkInterval);
    float currentDistance = getDistance();
    float progress = previousDistance - currentDistance;

    Serial.print("Progress: ");
    Serial.println(progress);

    if (progress < progressThreshold) {
      noProgressCount++;
    } else {
      noProgressCount = 0;
    }

    if (noProgressCount >= noProgressLimit) {
      Serial.println("Stuck! Handling...");
      handleStuckSituation();
      reachedTarget = true;
      return;
    }

    previousDistance = currentDistance;
  }

  stopMotors();
}

/********************************************************************************/
void handleStuckSituation() {
  stopMotors();
  delay(200);

  // Penalize current direction
  int angleIndex = (int)(targetDistance / 30.0) % numAngles;
  blockedDirection[angleIndex]++;
  Serial.print("Blocked angle index ");
  Serial.print(angleIndex);
  Serial.print(" now has penalty ");
  Serial.println(blockedDirection[angleIndex]);

  // Back up
  rightMotor(-driveSpeed);
  leftMotor(-driveSpeed);
  delay(600);

  // Turn slightly
  rightMotor(turnSpeed);
  leftMotor(-turnSpeed);
  delay(500);

  stopMotors();
  delay(200);
}

/********************************************************************************/
void rotateBriefly() {
  rightMotor(-turnSpeed);
  leftMotor(turnSpeed);
  delay(500);
  stopMotors();
  delay(200);
}

/********************************************************************************/
void decayBlockedMemory() {
  for (int i = 0; i < numAngles; i++) {
    if (blockedDirection[i] > 0) blockedDirection[i]--;
  }
}

/********************************************************************************/
void stopMotors() {
  rightMotor(0);
  leftMotor(0);
}

/********************************************************************************/
void rightMotor(int motorSpeed) {
  digitalWrite(AIN1, motorSpeed > 0);
  digitalWrite(AIN2, motorSpeed < 0);
  analogWrite(PWMA, abs(motorSpeed));
}

void leftMotor(int motorSpeed) {
  digitalWrite(BIN1, motorSpeed > 0);
  digitalWrite(BIN2, motorSpeed < 0);
  analogWrite(PWMB, abs(motorSpeed));
}

/********************************************************************************/
float getDistance() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float echoTime = pulseIn(echoPin, HIGH, 20000); // 20ms timeout
  if (echoTime == 0) return 0;

  return echoTime / 148.0;  // inches
}
