// gps.h
#ifndef GPS_H // include guard
#define GPS_H

#include <TinyGPS++.h>

class GPS : public TinyGPSPlus{
protected:
    float lat = 0;
    float lon = 0;
    float alt = 0;
    float spd = 0;
    int sat = 0;
public:
    using TinyGPSPlus::TinyGPSPlus;
    float getLat();
    float getLon();
    float getAlt();
    float getSpd();
    void getGpsInfo();
};

#endif /* DISPLAY_H */