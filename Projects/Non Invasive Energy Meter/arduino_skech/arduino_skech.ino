#include "EmonLib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

EnergyMonitor emon1;
LiquidCrystal_I2C lcd(0x27, 16, 2); 

const float assumedVoltage = 230.0; 
const int buzzerPin = 8; 
const float powerLimit = 1500.0; // Trigger alarm at 1500W

void setup() {
  Serial.begin(9600);
  
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Energy Monitor");
  delay(2000);
  lcd.clear();

  emon1.current(1, 30.0); // Pin A1, Calibration 30
}

void loop() {
  double Irms = emon1.calcIrms(1480);
  double apparentPower = Irms * assumedVoltage;

  if (apparentPower > powerLimit) {
    digitalWrite(buzzerPin, HIGH); 
    lcd.setCursor(0, 0);
    lcd.print("! OVERLOAD !    "); 
    lcd.setCursor(0, 1);
    lcd.print("Pwr: ");
    lcd.print(apparentPower, 1);
    lcd.print(" W   ");
  } else {
    digitalWrite(buzzerPin, LOW); 
    lcd.setCursor(0, 0);
    lcd.print("Irms: ");
    lcd.print(Irms, 2);
    lcd.print(" A    "); 
    lcd.setCursor(0, 1);
    lcd.print("Pwr : ");
    lcd.print(apparentPower, 1);
    lcd.print(" W    ");
  }
  delay(1000); 
}
