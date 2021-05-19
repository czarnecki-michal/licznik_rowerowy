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
#define hall_pin 35
#define button_pin 0
#define GPSBaud 9600


// zmienne okreslajace status podzespolow
bool status_sd = 0;
bool status_compass = 0;

// zmienne okreslajace stan
int button_prev;
int file_created = 0;
int activity_started = -1;
int old_state = 0;
int state = 0;
float old_rpm = 0;
long int last_gpx = 0;
int gpx_ended = 0;
int act_time = 0;
float gps_fix_time = 0;
int gps_fixed = 0;
// zmienne przechowujace odczyty
float heading = 0;
int angle = 0;
float cadency = 0;
float avg_cadency = 0;
float start = 0;
int count = 0;
int count_avg = 0;

// zmienne do obslugi wyznaczania srednich wartosci
int speed_index = 0;
const int speed_size = 50;
float speed_array[speed_size];
bool speed_full = 0;

int cad_index = 0;
const int cad_size = 50;
float cad_array[speed_size];
bool cad_full = 0;

// inicjalizacja komponentow
GPS gps;
SoftwareSerial gpsSerial(RXPin, TXPin);
HMC5883L compass;
Display display = Display();

void createGpx(GPS &gps);
void closeGpx();
static void smartDelay(unsigned long ms);
void startActivity();
float getAvgSpeed(float speed);
float getAvgCad(float cad);
float countRpm();

float lastSwitchDetectedMillis = 0;
int debounceInterval = 40;

void setup()
{
  // inicjalizacja monitora portu szeregowego
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);

  // ustalenie stanu podzespolow
  Serial.print("Inicjalizacja kompasu...");
  // status_compass = compass.begin();
  Serial.println("ok");

  // konfiguracja kompasu 
  // Serial.print("Konfiguracja kompasu...");
  // compass.setRange(HMC5883L_RANGE_1_3GA);
  // compass.setMeasurementMode(HMC5883L_CONTINOUS);
  // compass.setDataRate(HMC5883L_DATARATE_75HZ);
  // compass.setSamples(HMC5883L_SAMPLES_8);
  // compass.setOffset(0, 0);
  // Serial.println("ok");

  // inicjalizacja wyswietlacza
  Serial.print("Inicjalizacja wyswietalcza...");
  display.init();
  display.setRotation(0);
  display.fillScreen(TFT_LIGHTGREY);
  display.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  display.showGrid();
  Serial.println("ok");

  pinMode(hall_pin, INPUT);
  pinMode(button_pin, INPUT_PULLUP);
  button_prev = digitalRead(button_pin);

  delay(200);

  Serial.print("Inicjalizacja karty SD...");
  status_sd = initializeSdCard();
  Serial.println("ok");

  display.status(status_compass, gps, status_sd, 0);
}

void loop()
{
  // obsluga kompasu
  // Vector norm = compass.readNormalize();
  // heading = atan2(norm.YAxis, norm.XAxis);
  // float declinationAngle = (4.0 + (26.0 / 60.0)) / (180 / M_PI);
  // heading += declinationAngle;
  // if (heading < 0)
  // {
  //   heading += 2 * PI;
  // }

  // if (heading > 2 * PI)
  // {
  //   heading -= 2 * PI;
  // }
  
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

  if(gps_fixed == 0 && gps.location.isValid()){
    gps_fix_time = millis();
    gps_fixed = 1;
  }

  // uruchomienie aktywnosci i zapis na karcie sd pliku GPX
  if(status_sd == 1){
    if(gps_fix_time != 0){
      if(button_prev == HIGH && digitalRead(button_pin) == LOW){
        startActivity();
      }

      if (millis() - gps_fix_time >= 5000U && act_time == 0){
        act_time = 1;
        startActivity();
      }

      if(millis() - gps_fix_time >= 20000U && act_time == 1){
        act_time = 2;
        startActivity();
      }
    
      button_prev = digitalRead(button_pin);

      if(activity_started == 1){
        createGpx(gps);
      }
      if(activity_started == 0 && gpx_ended == 0){
        closeGpx();
        gpx_ended = 1;
      }
    }
  }else{
    display.error("NO SD CARD");
  }

  // wyswietlenie wszystkich wartosci
  heading = cadency;
  display.showCompass(heading);
  display.showCoords(gps.getLat(), gps.getLon(), gps.getAlt());
  display.showCurSpeed(speed);
  display.showAvgSpeed(avg_speed);
  display.showAvgCadency(avg_cadency);
  display.showCurCadency(cadency);

  display.status(status_compass, gps, status_sd, activity_started);
}

void startActivity(){
  if(activity_started == 0 or activity_started == -1){
    Serial.println("Activity started");
    activity_started = 1;
    display.fillTriangle(100, 120, 100, 180, 160, 150, TFT_BLACK);
    delay(250);
    display.fillTriangle(100, 120, 100, 180, 160, 150, TFT_LIGHTGREY);
  }else if(activity_started == 1){
    Serial.println("Activity stopped");
    activity_started = 0;
    display.fillRect(100, 120, 10, 60, TFT_BLACK);
    display.fillRect(130, 120, 10, 60, TFT_BLACK);
    delay(250);
    display.fillRect(100, 120, 10, 60, TFT_LIGHTGREY);
    display.fillRect(130, 120, 10, 60, TFT_LIGHTGREY);
  }
}

void closeGpx(){
  String end = "</trkseg>\n</trk>\n</gpx>";

  if (file_created == 1){
    char buffer[32];
    sprintf(
        buffer,
        "/%02d%02d%02d.gpx",
        gps.getYear(),
        gps.getMonth(),
        gps.getDay());
    myFile = SD.open(buffer, FILE_APPEND);
    if(myFile){
      myFile.print(end);
      myFile.close();
    }
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

        myFile = SD.open(buffer, FILE_WRITE);
        Serial.println("File created!");

        if(myFile){
          String header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                          "\n<gpx creator=\"Licznik rowerowy\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                          " xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\""
                          " version=\"1.1\" xmlns=\"http://www.topografix.com/GPX/1/1\">";
          String metadata = "\n<metadata>\n<time>";
          metadata += gps.getDatetime();
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

    if(millis() - last_gpx >= 2000UL){
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
          myFile.print(gps.getDatetime());
          myFile.print("</time>");
          myFile.println();
          myFile.print("<ele> ");
          myFile.print(gps.getAlt());
          myFile.print("</ele>");
          myFile.println();
          myFile.print("<sat> ");
          myFile.print(gps.satellites.value());
          myFile.print("</sat>");
          myFile.println();
          myFile.print("</trkpt>");
          myFile.println();

          last_gpx = millis();
          Serial.println("done.");
          myFile.close();
          Serial.println("File saved!");
      } else {
          // if the file didn't open, print an error:
          Serial.println("error opening file");
      }
    }
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
  if (millis() - lastSwitchDetectedMillis > debounceInterval) {
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

    lastSwitchDetectedMillis = millis();

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