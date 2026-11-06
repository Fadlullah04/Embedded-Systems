#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define Hardware Pins
const int BUZZER_PIN = 8;
const int GREEN_LED = 6;
const int RED_LED = 7;

void setup() {
  Serial.begin(9600); 
  
  // Initialize output pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  lcd.init(); 
  lcd.backlight(); 
  lcd.print("Scan ID Please.."); 
  //lcd.blink();
}

void loop() {
  if (Serial.available()) { 
    String incomingData = Serial.readStringUntil('\n'); 
    incomingData.trim(); 
    
    lcd.noBlink(); 
    
    // ==========================================
    // SUCCESS: VALID CBT TOKEN
    // ==========================================
    if (incomingData.startsWith("1")) { 
       int firstPipe = incomingData.indexOf('|');
       int secondPipe = incomingData.indexOf('|', firstPipe + 1);
       
       String studentName = "Unknown";
       String courseCode = ""; // Replaced Track with Course Code
       
       if (firstPipe != -1 && secondPipe != -1) {
           studentName = incomingData.substring(firstPipe + 1, secondPipe);
           courseCode = incomingData.substring(secondPipe + 1);
       }

       // --- Screen 1: Biodata ---
       lcd.clear();
       lcd.setCursor(0, 1);
       lcd.print(courseCode); // Prints Course Code on the bottom row

       // Handle long names with scrolling on the top row
       if (studentName.length() <= 16) {
         lcd.setCursor(0, 0); 
         lcd.print(studentName);
         delay(5000); 
       } else {
         String paddedName = studentName + "   "; 
         unsigned long startTime = millis();
         int scrollPos = 0;
         
         while (millis() - startTime < 5000) {
           lcd.setCursor(0, 0); 
           lcd.print((paddedName + paddedName).substring(scrollPos, scrollPos + 16));
           scrollPos++;
           if (scrollPos >= paddedName.length()) scrollPos = 0;
           delay(350); 
         }
       }

       // --- Screen 2: REGISTERED & Hardware Sync ---
       lcd.clear();
       lcd.setCursor(0, 0);
       lcd.print("REGISTERED!");
       
       // Sync: Double Blink Green LED + Double Beep (Active Buzzer)
       digitalWrite(GREEN_LED, HIGH);
       digitalWrite(BUZZER_PIN, HIGH);
       delay(150);
       
       digitalWrite(GREEN_LED, LOW);
       digitalWrite(BUZZER_PIN, LOW);
       delay(150);
       
       digitalWrite(GREEN_LED, HIGH);
       digitalWrite(BUZZER_PIN, HIGH);
       delay(150);
       
       digitalWrite(GREEN_LED, LOW);
       digitalWrite(BUZZER_PIN, LOW);
       
       delay(1550); // Hold the screen for a bit before resetting

    // ==========================================
    // FAILURE: INVALID OR EXPIRED TOKEN
    // ==========================================
    } else if (incomingData.startsWith("0")) {
      lcd.clear();
      lcd.print("ACCESS DENIED!"); 
      
      // Sync: Solid Red LED + Long Angry Buzz (Active Buzzer)
      digitalWrite(RED_LED, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);
      
      delay(1500); // 1.5 seconds of solid error feedback
      
      digitalWrite(RED_LED, LOW);
      digitalWrite(BUZZER_PIN, LOW);
      
      delay(2000); // Wait before resetting
    }
    
    // --- Reset System ---
    lcd.clear(); 
    lcd.print("Scan ID Please"); 
    lcd.blink();
    
    // Flush serial buffer to prevent lagging
    while(Serial.available() > 0) {
      Serial.read();
    }
  }
}