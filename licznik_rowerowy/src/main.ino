#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <SoftwareSerial.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>
#include <SPI.h>
#include "display.h"
#include "gps.h"

#define TFT_DC 9              
#define TFT_CS 10             
#define TFT_RST 8                      

int RXPin = A3;
int TXPin = A2;
int hall_pin = A1;

int GPSBaud = 9600;
GPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
HMC5883L_Simple Compass;
Display display = Display(TFT_CS, TFT_DC, TFT_RST);

int angle = 0;
float cadency = 0;
float avg_cadency = 0;
int old_state = 0;
int state = 0;
bool changed = false;
float start = 0;
int count = 0;
int count_avg = 0;
float old_rpm = 0;
int hall_thresh = 10;

void displayGpsInfo();
float countRpm();
static void smartDelay();

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  
  Wire.begin();
  Compass.SetDeclination(4, 47, 'E');  
  Compass.SetSamplingMode(COMPASS_SINGLE);
  Compass.SetOrientation(COMPASS_HORIZONTAL_X_NORTH);

  display.begin();
  display.setRotation(iliRotation180);
  display.fillScreen(ILI9341_LIGHTGREY);
  display.setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);

  display.showGrid();
  display.setFont(SystemFont5x7);
  display.status(Compass, gps);
  pinMode (hall_pin, INPUT);
}

void loop()
{
  float start_2 = millis();
  while(millis() - start_2 < 800){
    avg_cadency = countAvgRpm();
    cadency = countRpm();
  }

  display.showAvgCadency(avg_cadency);
  display.showCurCadency(cadency);

  angle = Compass.GetHeadingDegrees();

  display.showCompass(angle);
  display.showAvgSpeed(1.0);

  gps.getGpsInfo();
  display.showCoords(gps.getLat(), gps.getLon(), gps.getAlt());
  display.showCurSpeed(gps.getSpd());
  display.status(Compass, gps);
  smartDelay(20);
}

float countAvgRpm(){
  float end_time = millis();
  float time_passed = end_time / 1000.0;
  float rpm_val = ((count_avg/2) / time_passed) * 60.0;

  return rpm_val;
}

float countRpm(){
  state = digitalRead(hall_pin);

  if (state != old_state){
    old_state = state;
    count++;
    count_avg++;
  }

  if(millis()-start >= 8000UL){
    float end_time = millis();
    float time_passed = ((end_time - start) / 1000.0);
    float rpm_val = ((count/2) / time_passed) * 60.0;
    old_rpm = rpm_val;

    start = millis();
    count = 0;
  }
  return old_rpm;
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (gpsSerial.available())
      gps.encode(gpsSerial.read());
  } while (millis() - start < ms);
}