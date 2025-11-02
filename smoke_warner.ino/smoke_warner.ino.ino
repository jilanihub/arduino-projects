// ===================================
// MQ-2 Smoke Detector Alarm
// ===================================

// Pin Definitions
const int SMOKE_SENSOR_PIN = A0;  // Analog pin for MQ-2 A0 output
const int ALARM_LED_PIN    = 7;   // Digital pin for the warning LED
const int BUZZER_PIN       = 8;   // Digital pin for the buzzer

// Alarm Threshold
// This value is highly dependent on your sensor, environment, and sensitivity
// You may need to calibrate this. Test what reading you get in clean air vs. smoke.
const int SMOKE_THRESHOLD = 300; // Change this value based on your testing

void setup() {
  // Initialize Serial Communication for debugging/calibration
  Serial.begin(9600);
  Serial.println("MQ-2 Smoke Sensor Initializing...");

  // Set the digital pins as output
  pinMode(ALARM_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initial self-test: flash LED and beep
  digitalWrite(ALARM_LED_PIN, HIGH);
  tone(BUZZER_PIN, 1000); // Play a 1kHz tone

  delay(500); // Wait 0.5 seconds

  digitalWrite(ALARM_LED_PIN, LOW);
  noTone(BUZZER_PIN); // Stop the tone
}

void loop() {
  // Read the analog value from the MQ-2 sensor
  int smokeValue = analogRead(SMOKE_SENSOR_PIN);

  // Print the value to the Serial Monitor for monitoring/calibration
  Serial.print("Sensor Value: ");
  Serial.print(smokeValue);
  Serial.print(" - Threshold: ");
  Serial.println(SMOKE_THRESHOLD);

  // Check if the sensor reading exceeds the defined threshold
  if (smokeValue > SMOKE_THRESHOLD) {
    // --- ALARM STATE ---
    
    // Activate visual alarm (LED)
    digitalWrite(ALARM_LED_PIN, HIGH);
    
    // Activate audible alarm (Buzzer)
    // Using tone() for an active buzzer will just turn it on, 
    // but for a passive buzzer, it generates the sound.
    // We'll use a pulsing tone for a more recognizable alarm sound.
    tone(BUZZER_PIN, 2000); // Play a high-pitched 2kHz tone
    delay(100);
    noTone(BUZZER_PIN);
    delay(100);
    
    // Print warning message
    Serial.println("!!! SMOKE OR HIGH GAS DETECTED - ALARM ACTIVE !!!");

  } else {
    // --- SAFE STATE ---
    
    // Keep alarm outputs off
    digitalWrite(ALARM_LED_PIN, LOW);
    noTone(BUZZER_PIN);
    
    // Print a safe message
    Serial.println("Status: Safe");
  }

  // A short delay for stable readings and not spamming the serial monitor
  delay(200); 
}