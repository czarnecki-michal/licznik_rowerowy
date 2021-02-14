#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L_Simple.h>
#include <SoftwareSerial.h>
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>
#include <SPI.h>
#include "sd_card.h"
#include "display.h"
#include "gps.h"

#define TFT_DC 9              
#define TFT_CS 10             
#define TFT_RST 8                      

int RXPin = A3;
int TXPin = A2;
int hall_pin = A1;
int button_pin = 2;
int activity_started = 0;
int button_prev;

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
float start = 0;
int count = 0;
int count_avg = 0;
float old_rpm = 0;

int speed_index = 0;
const int speed_size = 20;
float speed_array[speed_size];
bool speed_full = 0;

int cad_index = 0;
const int cad_size = 20;
float cad_array[speed_size];
bool cad_full = 0;

long int last_gpx = 0;

void createGpx(GPS &gps);

void setup()
{
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  pinMode(button_pin, INPUT_PULLUP);
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
  button_prev = digitalRead(button_pin);
  display.status(Compass, gps);
  pinMode (hall_pin, INPUT);
  initializeSdCard();
}

void loop()
{
  gps.getGpsInfo();
  float speed = gps.getSpd();
  float start_2 = millis();
  
  while(millis() - start_2 < 500){
    cadency = countRpm();
  }

  avg_cadency = getAvgCad(cadency);
  angle = Compass.GetHeadingDegrees();
  float avg_speed = getAvgSpeed(speed);
  display.showAvgSpeed(avg_speed);
  display.status(Compass, gps);

  if(button_prev == HIGH && digitalRead(button_pin) == LOW){
    startActivity();
  }

  button_prev = digitalRead(button_pin);

  if(activity_started == 1){
    createGpx(gps);
  }

  display.showAvgCadency(avg_cadency);
  display.showCurCadency(cadency);
  display.showCompass(angle);
  display.showCoords(gps.getLat(), gps.getLon(), gps.getAlt());
  display.showCurSpeed(speed);
  smartDelay(20);
}

void stopActivity(){
  if(activity_started == 1){
    Serial.println("Activity stopped");

  }
}

void startActivity(){
  if(activity_started == 0){
    Serial.println("Activity started");
    activity_started = 1;
  }else{
    Serial.println("Stop previous activity");
  }
}

void createGpx(GPS &gps){
  char buffer[32];
  sprintf(
      buffer,
      "%02d%02d%02d.gpx",
      gps.getYear(),
      gps.getMonth(),
      gps.getDay());

  myFile = SD.open(buffer, FILE_WRITE);

  // if the file opened okay, write to it:
  if(gps.getLat() != 0){
    if(millis() - last_gpx >= 8000UL){
      if (myFile) {
          Serial.print("Writing to test.txt...");
          myFile.print("<trkpt lat=\"");
          myFile.print(gps.getLat(), 10);
          myFile.print("\" lon=\"");
          myFile.print(gps.getLon(), 10);
          myFile.print("\">");
          myFile.println();
          myFile.print("<time> ");
          myFile.print("</time>");
          myFile.println();
          myFile.print("<ele> ");
          myFile.print("</ele>");
          myFile.println();
          myFile.print("</trkpt>");
          myFile.println();
          last_gpx = millis();
          Serial.println("done.");
      } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
      }
    }
  }
  myFile.close();

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