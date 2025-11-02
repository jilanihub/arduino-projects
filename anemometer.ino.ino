#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 1000;    // the debounce time; increase if the output flickers
 
int pinInterrupt = 2;
 
int Count = 0;
 
void onChange()
{
  if ( digitalRead(pinInterrupt) == LOW )
    Count++;
}
 
 
void setup()
{
  Serial.begin(115200); //Initialize serial port
  pinMode( pinInterrupt, INPUT_PULLUP);// set the interrupt pin
 
  //Enable
  attachInterrupt( digitalPinToInterrupt(pinInterrupt), onChange, FALLING);
 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  delay(500);
  display.clearDisplay();
  display.setTextColor(WHITE);
  
}
 
void loop()
{
  if ((millis() - lastDebounceTime) > debounceDelay)
  {
    lastDebounceTime = millis();
    
    Serial.print((Count * 8.75)/100);

       //clear display
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(4, 0);
    display.print("Anemometer");

    display.setCursor(2, 20); //oled display
    display.setTextSize(2);
    display.println("Wind Speed");

    display.setCursor(0, 40); //oled display
    display.setTextSize(3);
    display.print((Count * 8.75)/100);
    display.setCursor(70, 45);
    display.setTextSize(2);
    display.println(" m/s");
    display.display();
    
    Count = 0;
    
    Serial.println("m/s");
    
  }
  delay(1);
}