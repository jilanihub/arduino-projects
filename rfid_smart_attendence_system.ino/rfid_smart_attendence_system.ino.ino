#include <SPI.h>              // For MFRC522 RFID Reader
#include <MFRC522.h>          // For RFID Reader functions
#include <Wire.h>             // For I2C communication (LCD and RTC)
#include <LiquidCrystal_I2C.h>// For 16x2 I2C LCD
#include <RTClib.h>           // For RTC Module

// Pin Definitions for MFRC522 (SPI)
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// I2C Device Initialization
LiquidCrystal_I2C lcd(0x27, 16, 2); // Check your LCD address (0x27 or 0x3F)
RTC_DS3231 rtc; // Or RTC_DS1307

// --- GLOBAL VARIABLES (for idle state display) ---
DateTime now;

// =======================================================
// SETUP FUNCTION
// =======================================================
void setup() {
  Serial.begin(9600); // Initialize serial communication for Excel logging
  
  // Initialize MFRC522
  SPI.begin();
  mfrc522.PCD_Init();

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.print("RFID ATTENDANCE");
  
  // Initialize RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1); // Stop program if RTC is missing
  }
  // This line sets the RTC to the Arduino's compilation time (use once!)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  // Print CSV Header to Serial (for Excel)
  Serial.println("Name,UID,DateTime,Status");
}

// =======================================================
// LOOP FUNCTION
// =======================================================
void loop() {
  now = rtc.now(); // Update the time variable for the idle state

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // 1. READ RFID UID
    String uid = getUIDString(); // Now defined below

    // 2. GET TIME from RTC
    String timeStamp = now.timestamp(DateTime::TIMESTAMP_FULL);
    
    // 3. LOOKUP CARD and GET NAME
    String userName = getAuthorizedUserName(uid); // Now defined below
    String logData = "";

    if (userName != "UNAUTHORIZED") {
      // 4a. EXCEL LOGGING (Via Serial - CSV Format)
      logData = userName + "," + uid + "," + timeStamp + "," + "CHECK IN";
      Serial.println(logData);

      // 4b. LCD OUTPUT (Success)
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Welcome:");
      lcd.setCursor(0, 1);
      lcd.print(userName);
      
      delay(3000); // Display for 3 seconds
    } else {
      // Unauthorized card logic
      // Log UNAUTHORIZED attempts too (optional)
  
      Serial.println(logData);
      
      // LCD OUTPUT (Denied)
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ACCESS DENIED!");
      lcd.setCursor(0, 1);
      lcd.print(uid.substring(0, 15)); // Show part of the UID
      delay(3000);
    }
    mfrc522.PICC_HaltA(); // Stop reading the card
  } else {
    // Idle state on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SCAN YOUR CARD");
    lcd.setCursor(0, 1);
    lcd.print(now.timestamp(DateTime::TIMESTAMP_TIME)); // Current time from RTC
  }
  delay(500);
}

// =======================================================
// CUSTOM FUNCTIONS IMPLEMENTATION
// =======================================================

// Function to convert the RFID card's UID byte array to a readable String
String getUIDString() {
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Format each byte to a two-character hex string
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0";
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidString.toUpperCase();
  return uidString;
}

// Function to map the UID to a name. **MUST BE CALIBRATED!**
String getAuthorizedUserName(String uid) {
  // --- DEFINE AUTHORIZED UIDS AND NAMES HERE ---
  // You must scan your own cards and replace the example UIDs below.

  if (uid == "E774B819") {        // UID of Card 1
    return "John Doe";
  } else if (uid == "8D1A3B0F") { // UID of Card 2
    return "Jane Smith";
  } else if (uid == "C35D7A94") { // UID of Card 3
    return "Tech Admin";
  }
  
  // Return "UNAUTHORIZED" if the UID is not found in the list
  return "UNAUTHORIZED";
}