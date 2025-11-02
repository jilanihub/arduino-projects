#include <Servo.h>

// Define component pins
const int MOISTURE_SENSOR_PIN = A0; // Analog pin for the soil moisture sensor
const int LEFT_SERVO_PIN = 9;       // Digital PWM pin for the left servo
const int RIGHT_SERVO_PIN = 10;     // Digital PWM pin for the right servo

// Define bridge states (servo angles)
// The bridge should be DOWN when the angle is 0, and UP when the angle is 90.
// If your servos are mounted in opposite directions, you might need to reverse 
// the angles for one of them (e.g., left up = 90, right up = 0).
const int BRIDGE_DOWN_ANGLE = 0;   
const int BRIDGE_UP_ANGLE = 90;    

// Define the moisture threshold (adjust based on testing)
// A LOWER value means MORE moisture/water detected (0 is wettest).
const int MOISTURE_THRESHOLD = 500; // If sensor reading is BELOW this, lift the bridge.

// Create two Servo objects
Servo leftServo;
Servo rightServo;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Automatic Bridge System with TWO Servos Initialized.");

  // Attach both servo motors to their specified pins
  leftServo.attach(LEFT_SERVO_PIN);
  rightServo.attach(RIGHT_SERVO_PIN);

  // Set the bridge to the initial "down" position
  leftServo.write(BRIDGE_DOWN_ANGLE);
  rightServo.write(BRIDGE_DOWN_ANGLE);
  delay(1000); // Wait a moment for the servos to move
}

void loop() {
  // Read the analog value from the moisture sensor
  int moistureValue = analogRead(MOISTURE_SENSOR_PIN);

  // Print the value for debugging
  Serial.print("Moisture Sensor Reading: ");
  Serial.println(moistureValue);

  // Check the moisture level against the threshold
  if (moistureValue < MOISTURE_THRESHOLD) {
    // Condition: Water level is HIGH (reading is low) -> Raise the bridge
    Serial.println("!!! FLOOD ALERT - RAISING BRIDGE !!!");
    
    // Check if the bridge is not already up before sending the command
    if (leftServo.read() != BRIDGE_UP_ANGLE) {
      leftServo.write(BRIDGE_UP_ANGLE);
      rightServo.write(BRIDGE_UP_ANGLE); // Move both servos simultaneously
      Serial.println("Bridge is up.");
      delay(500); // Give the servos time to complete the move
    }
  } else {
    // Condition: Water level is NORMAL (reading is high) -> Lower the bridge
    Serial.println("Normal water level - LOWERING BRIDGE.");
    
    // Check if the bridge is not already down before sending the command
    if (leftServo.read() != BRIDGE_DOWN_ANGLE) {
      leftServo.write(BRIDGE_DOWN_ANGLE);
      rightServo.write(BRIDGE_DOWN_ANGLE); // Move both servos simultaneously
      Serial.println("Bridge is down.");
      delay(500); // Give the servos time to complete the move
    }
  }

  // Wait a short time before the next reading
  delay(2000);
}