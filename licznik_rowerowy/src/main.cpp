#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <LiquidCrystal.h> //Dołączenie bilbioteki

HMC5883L_Simple Compass;
LiquidCrystal LCD(8, 9, 10, 11, 12, 13);


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Compass.SetDeclination(4, 47, 'E');  
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetScale(COMPASS_SCALE_250);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

  LCD.begin(16, 2);
  LCD.clear();
}

void loop(){
   float heading = Compass.GetHeadingDegrees();
   LCD.setCursor(0,0);
   LCD.print("Heading:");
   LCD.setCursor(0,1);
   LCD.print(heading);
   delay(1000);
}