#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with I2C address 0x27, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define buzzer 2 // Pin connected to the buzzer
#define led 3    // Pin connected to the LED

// Pins connected to the x, y, and z outputs of the ADXL335 accelerometer
#define x A0
#define y A1
#define z A2

/* Variables */
int xsample = 0; // Stores average calibration value for X-axis
int ysample = 0; // Stores average calibration value for Y-axis
int zsample = 0; // Stores average calibration value for Z-axis
long start;      // Stores the start time for the buzzer/LED alert
int buz = 0;     // Flag to indicate buzzer/LED state (on/off)

/* Macros */
#define samples 50     // Number of samples to average during calibration
#define maxVal 20      // Maximum allowed change in readings
#define minVal -20     // Minimum allowed change in readings
#define buzTime 5000   // Duration for which the buzzer/LED stays on

void setup() {
  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the LCD backlight
  Serial.begin(9600); // Initialize serial communication
  delay(1000);

  // Display initial messages
  Serial.println("System Initialized...");
  lcd.print("EarthQuake ");
  lcd.setCursor(0, 1);
  lcd.print("Detector ");
  delay(2000);
  lcd.clear();
  lcd.print("Calibrating.....");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");

  // Configure buzzer and LED pins as outputs
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);
  buz = 0; // Initialize buzzer/LED state
  digitalWrite(led, buz);

  // Calibration process: Take multiple samples and calculate averages
  Serial.println("Calibration Started...");
  for (int i = 0; i < samples; i++) {
    xsample += analogRead(x); // Read and accumulate X-axis values
    ysample += analogRead(y); // Read and accumulate Y-axis values
    zsample += analogRead(z); // Read and accumulate Z-axis values
  }

  // Calculate the average values for X, Y, and Z axes
  xsample /= samples;
  ysample /= samples;
  zsample /= samples;

  // Indicate calibration completion
  delay(3000);
  lcd.clear();
  lcd.print("Calibrated");
  delay(1000);
  Serial.println("Calibration Complete!");
  Serial.println("Baseline Values:");
  Serial.print("X Baseline: "); Serial.println(xsample);
  Serial.print("Y Baseline: "); Serial.println(ysample);
  Serial.print("Z Baseline: "); Serial.println(zsample);

  lcd.clear();
  lcd.print("Device Ready");
  delay(1000);

  // Display column headers for X, Y, and Z values
  lcd.clear();
  lcd.print(" X    Y    Z ");
}

void loop() {
  // Read accelerometer values
  int value1 = analogRead(x); // Current reading for X-axis
  int value2 = analogRead(y); // Current reading for Y-axis
  int value3 = analogRead(z); // Current reading for Z-axis

  // Calculate change in accelerometer values
  int xValue = xsample - value1;
  int yValue = ysample - value2;
  int zValue = zsample - value3;

  // Calculate magnitude of vibration
  int magnitude = sqrt(sq(xValue) + sq(yValue) + sq(zValue));

  // Check if an earthquake is detected
  if (xValue < minVal || xValue > maxVal || yValue < minVal || yValue > maxVal || zValue < minVal || zValue > maxVal) {
    if (buz == 0) {
      start = millis(); // Start timer for alert duration
      buz = 1;          // Activate buzzer/LED
      Serial.println("Earthquake Detected! Alert Activated.");

      // Display earthquake alert on LCD
      lcd.clear();
      lcd.print("Earthquake !!!!");
    }

    // Display X, Y, Z values on the second line during earthquake detection
    lcd.setCursor(0, 1);
    lcd.print(padValue(xValue)); // X value at column 0
    lcd.setCursor(6, 1);
    lcd.print(padValue(yValue)); // Y value at column 6
    lcd.setCursor(11, 1);
    lcd.print(padValue(zValue)); // Z value at column 11

    // Send X, Y, Z values to Serial Monitor
    Serial.print("Earthquake: X: "); Serial.print(xValue);
    Serial.print(" | Y: "); Serial.print(yValue);
    Serial.print(" | Z: "); Serial.print(zValue);
    Serial.print(" | Magnitude: "); Serial.println(magnitude);

    // Turn on LED and buzzer
    digitalWrite(led, HIGH);
    tone(buzzer, 1000);

    // Turn off alert after buzzer time
    if (millis() >= start + buzTime) {
      buz = 0;
      noTone(buzzer);
      digitalWrite(led, LOW);
      Serial.println("Alert Deactivated: Buzzer and LED OFF");

      // Restore normal display
      lcd.clear();
      lcd.print(" X    Y    Z ");
    }

    // Skip further updates during alert
    return;
  }

  // Display X, Y, Z values on the second line during normal operation
  lcd.setCursor(0, 1);
  lcd.print(padValue(xValue)); // X value at column 0
  lcd.setCursor(6, 1);
  lcd.print(padValue(yValue)); // Y value at column 6
  lcd.setCursor(11, 1);
  lcd.print(padValue(zValue)); // Z value at column 11

  // Send X, Y, Z values to Serial Monitor
  Serial.print("Normal: X: "); Serial.print(xValue);
  Serial.print(" | Y: "); Serial.print(yValue);
  Serial.print(" | Z: "); Serial.print(zValue);
  Serial.print(" | Magnitude: "); Serial.println(magnitude);

  delay(150); // Delay for stable readings
}

// Function to pad values for LCD display
String padValue(int value) {
  String strValue = String(value);
  while (strValue.length() < 4) { // Pad with spaces to ensure proper alignment
    strValue += " ";
  }
  return strValue.substring(0, 4); // Truncate to ensure exact 4 characters
}
