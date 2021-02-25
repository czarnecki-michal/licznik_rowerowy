#include <Arduino.h>
#include <Wire.h>
#include <HMC5883L.h>
#include <SoftwareSerial.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "sd_card.h"
#include "display.h"
#include "gps.h"                  

#define RXPin 13
#define TXPin 12
#define hall_pin 34
#define button_pin 0
#define GPSBaud 9600

// zmienne okreslajace status podzespolow
bool status_sd = 0;
bool status_compass = 0;

// zmienne okreslajace stan
int button_prev;
int file_created = 0;
int activity_started = 0;
int old_state = 0;
int state = 0;
float old_rpm = 0;
long int last_gpx = 0;

// zmienne przechowujace odczyty
int angle = 0;
float cadency = 0;
float avg_cadency = 0;
float start = 0;
int count = 0;
int count_avg = 0;

// zmienne do obslugi wyznaczania srednich wartosci
int speed_index = 0;
const int speed_size = 20;
float speed_array[speed_size];
bool speed_full = 0;

int cad_index = 0;
const int cad_size = 20;
float cad_array[speed_size];
bool cad_full = 0;

// inicjalizacja komponentow
GPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
HMC5883L compass;
Display display = Display();

void createGpx(GPS &gps);
static void smartDelay(unsigned long ms);
void startActivity();
float getAvgSpeed(float speed);
float getAvgCad(float cad);
float countRpm();

void setup()
{
  // inicjalizacja monitora portu szeregowego
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  // ustalenie stanu podzespolow
  status_compass = compass.begin();
  status_sd = initializeSdCard();

  // konfiguracja kompasu 
  compass.setRange(HMC5883L_RANGE_1_3GA);
  compass.setMeasurementMode(HMC5883L_CONTINOUS);
  compass.setDataRate(HMC5883L_DATARATE_0_75_HZ);
  compass.setSamples(HMC5883L_SAMPLES_2);
  compass.setOffset(0, 0);

  // inicjalizacja wyswietlacza
  display.init();
  display.setRotation(2);
  display.fillScreen(TFT_LIGHTGREY);
  display.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  display.showGrid();

  pinMode (hall_pin, INPUT);
  pinMode(button_pin, INPUT_PULLUP);
  button_prev = digitalRead(button_pin);

  display.status(status_compass, gps, status_sd, 0);
}

void loop()
{
  // obsluga kompasu
  Vector norm = compass.readNormalize();
  float heading = atan2(norm.YAxis, norm.XAxis);
  float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  heading += declinationAngle;
  if (heading < 0)
  {
    heading += 2 * PI;
  }

  if (heading > 2 * PI)
  {
    heading -= 2 * PI;
  }
  
  // pobranie danych z GPS
  gps.getGpsInfo();
  float speed = gps.getSpd();
  smartDelay(20);

  // wyznaczenie kadencji
  float start_2 = millis();
  while(millis() - start_2 < 500){
    cadency = countRpm();
  }

  avg_cadency = getAvgCad(cadency);

  // obliczenie sredniej predkosci
  float avg_speed = getAvgSpeed(speed);


  // uruchomienie aktywnosci i zapis na karcie sd pliku GPX
  if(status_sd == 1){
    if(button_prev == HIGH && digitalRead(button_pin) == LOW){
      startActivity();
    }

    button_prev = digitalRead(button_pin);

    if(activity_started == 1){
      createGpx(gps);
    }
  }else{
    display.error("NO SD CARD");
  }

  // wyswietlenie wszystkich wartosci
  display.showCompass(heading);
  display.showCoords(gps.getLat(), gps.getLon(), gps.getAlt());
  display.showCurSpeed(speed);
  display.showAvgSpeed(avg_speed);
  display.showAvgCadency(avg_cadency);
  display.showCurCadency(cadency);

  display.status(status_compass, gps, status_sd, activity_started);
}

void startActivity(){
  if(activity_started == 0){
    Serial.println("Activity started");
    activity_started = 1;
  }else if(activity_started == 1){
    Serial.println("Activity stopped");
    activity_started = 0;
  }
}

void createGpx(GPS &gps){
  char buffer[32];
  sprintf(
      buffer,
      "/%02d%02d%02d.gpx",
      gps.getYear(),
      gps.getMonth(),
      gps.getDay());

  // if the file opened okay, write to it:
  if(gps.getLat() != 0){
    if (file_created == 0){
        file_created = 1;
        char buffer[32];
        sprintf(
            buffer,
            "/%02d%02d%02d.gpx",
            gps.getYear(),
            gps.getMonth(),
            gps.getDay());

        char date_buffer[40];

        sprintf(
            date_buffer,
            "%02d-%02d-%02d",
            gps.getYear(),
            gps.getMonth(),
            gps.getDay());

        myFile = SD.open(buffer, FILE_WRITE);

        if(myFile){
          String header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                          "\n<gpx creator=\"StravaGPX iPhone\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                          " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\""
                          " version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">";
          String metadata = "\n<metadata>\n<time>";
          metadata += date_buffer;
          metadata += "</time>\n</metadata>";

          String beg = "<trk>"
                        "<name>Test ride</name>"
                        "<type>9</type>"
                        "<trkseg>";

          myFile.print(header);
          myFile.print(metadata);
          myFile.print(beg);

          myFile.close();
        }
    }

    if(millis() - last_gpx >= 8000UL){
      myFile = SD.open(buffer, FILE_APPEND);
      if (myFile) {
          Serial.print("Writing to ");
          Serial.print(buffer);
          Serial.print(" ");

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
          myFile.close();
          Serial.println("File saved!");
      } else {
          // if the file didn't open, print an error:
          Serial.println("error opening test.txt");
      }
    }
  }
  String buffer2;
  myFile = SD.open(buffer, FILE_READ);
  myFile.close();

  while(myFile.available()){
    buffer2 = myFile.readStringUntil('\n');
    Serial.println(buffer2); //Printing for debugging purpose  
  }

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

  if(millis()-start >= 3000UL){
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