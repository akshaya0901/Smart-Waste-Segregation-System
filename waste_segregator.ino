#include <CheapStepper.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define IR_SENSOR 5
#define PROX_SENSOR 6
#define BUZZER 12
#define MOISTURE_PIN A0
#define TRIG_PIN 4
#define ECHO_PIN 3
Servo servoMotor;
CheapStepper stepper(8, 9, 10, 11);
LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27
long getDistance() {
 long total = 0;
 for (int i = 0; i < 5; i++) {
 digitalWrite(TRIG_PIN, LOW);
 delayMicroseconds(2);
 digitalWrite(TRIG_PIN, HIGH);
 delayMicroseconds(10);
 digitalWrite(TRIG_PIN, LOW);

 long duration = pulseIn(ECHO_PIN, HIGH);
 total += (duration * 0.0344) / 2;
 delay(50);
 }
 return total / 5;
}
void setup() {
 Serial.begin(9600); // Serial monitor + Bluetooth
 pinMode(IR_SENSOR, INPUT);
 pinMode(PROX_SENSOR, INPUT_PULLUP);
 pinMode(BUZZER, OUTPUT);
 pinMode(TRIG_PIN, OUTPUT);
 pinMode(ECHO_PIN, INPUT);

 servoMotor.attach(7);
 servoMotor.write(70); // Closed position
 stepper.setRpm(17);
 lcd.init();
 lcd.backlight();
 lcd.setCursor(0, 0);
 lcd.print("Smart Dustbin");
 delay(2000);
 lcd.clear();
}
void loop() {
 checkDustbinLevel();
 checkProximity();
 checkIRSensor();
 delay(300);
}
void checkDustbinLevel() {
 long distance = getDistance();
 lcd.clear();
 lcd.setCursor(0, 0);
 if (distance < 6) {
 lcd.print("Dustbin Full");
 tone(BUZZER, 1000, 1000);
 Serial.println("Dustbin Full"); // Bluetooth alert
 } else {
 lcd.print("Dustbin OK");
 noTone(BUZZER);
 }
 delay(2000);
}
void checkProximity() {
 if (digitalRead(PROX_SENSOR) == LOW) {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Metal Detected");
 tone(BUZZER, 1000, 1000);
 Serial.println("Metal Detected");
 stepper.moveDegreesCW(240);
 delay(1000);
 openAndCloseServo();
 stepper.moveDegreesCCW(240);
 delay(1000);
 }
}
void checkIRSensor() {
 if (digitalRead(IR_SENSOR) == LOW) {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Scanning...");
 tone(BUZZER, 1000, 500);
 delay(1000);
 int moisture = readMoisture();
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Moisture: ");
 lcd.print(moisture);
 lcd.print("%");
 delay(2000);
 if (moisture > 30) {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Wet Waste");
 Serial.println("Wet Waste Detected");
 stepper.moveDegreesCW(120);
 delay(1000);
 openAndCloseServo();
 stepper.moveDegreesCCW(120);
 delay(1000);
 } else {
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Dry Waste");
 Serial.println("Dry Waste Detected");
 tone(BUZZER, 1000, 500);
 delay(1000);
 openAndCloseServo();
 }
 }
}
int readMoisture() {
 int total = 0;
 for (int i = 0; i < 3; i++) {
 int val = analogRead(MOISTURE_PIN);
 val = constrain(val, 380, 825);
 total += map(val, 380, 825, 100, 0);
 delay(75);
 }
 return total / 3;
}
void openAndCloseServo() {
 servoMotor.write(180); delay(1000);
 servoMotor.write(70); delay(1000);
}
