#include <EEPROM.h>
#include "GravityTDS.h"
//#include <LiquidCrystal.h>
 
//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
 #include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define TdsSensorPin A1
GravityTDS gravityTds;
 #define sensor_pin A0 
int read_ADC;
int ntu;
float temperature = 25,tdsValue = 0;
float calibration_value =0;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;
void setup()
{
  pinMode(sensor_pin, INPUT);
    Serial.begin(115200);
    lcd.begin(16,2);
    lcd.backlight();
    gravityTds.setPin(TdsSensorPin);
    gravityTds.setAref(5.0);  //reference voltage on ADC, default 5.0V on Arduino UNO
    gravityTds.setAdcRange(1024);  //1024 for 10bit ADC;4096 for 12bit ADC
    gravityTds.begin();  //initialization
}
 
void loop()
{
    //temperature = readTemperature();  //add your temperature sensor and read it
    gravityTds.setTemperature(temperature);  // set the temperature and execute temperature compensation
    gravityTds.update();  //sample and calculate
    tdsValue = gravityTds.getTdsValue();  // then get the value
    Serial.print(tdsValue,0);
    Serial.println("ppm");
    lcd.setCursor(0, 0);
    lcd.print("TDS Value:");
    lcd.setCursor(0, 1);
    lcd.print(tdsValue,0);
    lcd.print(" PPM");
    delay(1000);
    lcd.clear();

{
  
read_ADC = analogRead(sensor_pin);
if(read_ADC>208)read_ADC=208;

ntu = map(read_ADC, 0, 208, 300, 0); 
 
lcd.setCursor(0,0);
lcd.print("Turbidity: ");
lcd.print(ntu);
lcd.print("  ");

lcd.setCursor(0,1);//set cursor (colum by row) indexing from 0
if(ntu<10)            lcd.print("Water Very Clean");
if(ntu>=10 && ntu<30) lcd.print("Water Norm Clean");
if(ntu>=30)           lcd.print("Water Very Dirty");

delay(200);
}

{
 for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(A2);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buffer_arr[i]>buffer_arr[j])
 {
 temp=buffer_arr[i];
 buffer_arr[i]=buffer_arr[j];
 buffer_arr[j]=temp;
 }
 }
 }
 avgval=0;
 for(int i=2;i<8;i++)
 avgval+=buffer_arr[i];
 float volt=(float)avgval*3.5/1024/6;
 float ph_act = 3.5 * volt + calibration_value;
 lcd.setCursor(0, 0);
 lcd.print("pH Val:");
 lcd.setCursor(8, 0);
 lcd.print(ph_act);
 delay(1000);
}

    
}