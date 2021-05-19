#include "gps.h"

void GPS::getGpsInfo(){
    if (GPS::location.isValid()){
        this->lat = GPS::location.lat();
        this->lon = GPS::location.lng();
    }
    if (GPS::altitude.isValid()){
        this->alt = GPS::altitude.meters();
    }

    if(GPS::satellites.isValid()){
      this->sat = GPS::satellites.value();
    }

    if(GPS::speed.isValid()){
      this->spd = GPS::speed.kmph();
    // this->spd++;
    }

    if(GPS::date.isValid()){
        this->year = GPS::date.year();
        this->month = GPS::date.month();
        this->day = GPS::date.day();
    }
}

String GPS::getDatetime(){
    if (GPS::date.isValid()){
        String datetime = String(
                String(GPS::getYear()) + '-' +
                String(GPS::getMonth()) + '-' +
                String(GPS::getDay()) + 'T' +
                String(GPS::time.hour()) + ':' +
                String(GPS::time.minute()) + ':' +
                String(GPS::time.second()) + 'Z'
                );
        
        return datetime;
    }
    return "2000-01-01T00:00:00Z";
}


float GPS::getLat(){
    return this->lat;
}

float GPS::getLon(){
    return this->lon;
}

float GPS::getAlt(){
    return this->alt;
}

float GPS::getSpd(){
    return this->spd;
}

uint16_t GPS::getYear(){
    return this->year;
}

uint16_t GPS::getMonth(){
    return this->month;
}

uint16_t GPS::getDay(){
    return this->day;
}