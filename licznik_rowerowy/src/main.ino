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

int speed_index = 0;
const int speed_size = 20;
float speed_array[speed_size];
bool speed_full = 0;

int cad_index = 0;
const int cad_size = 20;
float cad_array[speed_size];
bool cad_full = 0;

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
  while(millis() - start_2 < 500){
    cadency = countRpm();
  }
  avg_cadency = getAvgCad(cadency);

  for(int i=0;i<cad_size;i++){
    Serial.print(cad_array[i]);
    Serial.print(" ");
  }
  Serial.println();

  display.showAvgCadency(avg_cadency);
  display.showCurCadency(cadency);

  angle = Compass.GetHeadingDegrees();

  display.showCompass(angle);

  gps.getGpsInfo();

  float speed = gps.getSpd();
  float avg_speed = getAvgSpeed(speed);
  display.showAvgSpeed(avg_speed);

  display.showCoords(gps.getLat(), gps.getLon(), gps.getAlt());
  display.showCurSpeed(speed);
  display.status(Compass, gps);

  smartDelay(20);
}

float getAvgSpeed(float speed){
  float sum = 0;

  speed_array[speed_index] = speed;

  speed_index++;

  if (speed_index == speed_size){
    speed_full = 1;
    speed_index = 0;
  }

  if(speed_full == 0){
    for(int i=0; i<speed_index+1; i++){
      sum += speed_array[i];
    }

    return sum/(speed_index+1);
  }else{
    for(int i=0; i<speed_size; i++){
      sum += speed_array[i];
    }

    return sum/speed_size;
  }
}

float getAvgCad(float cad){
  float sum = 0;

  cad_array[cad_index] = cad;

  cad_index++;

  if (cad_index == cad_size){
    cad_full = 1;
    cad_index = 0;
  }

  if(cad_full == 0){
    for(int i=0; i<cad_index+1; i++){
      sum += cad_array[i];
    }

    return sum/(cad_index+1);
  }else{
    for(int i=0; i<cad_size; i++){
      sum += cad_array[i];
    }

    return sum/cad_size;
  }
}

// float countAvgRpm(){
//   float end_time = millis();
//   float time_passed = end_time / 1000.0;
//   float rpm_val = ((count_avg/2) / time_passed) * 60.0;

//   return rpm_val;
// }

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