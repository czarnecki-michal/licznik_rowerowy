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