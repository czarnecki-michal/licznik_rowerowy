// display.h
#ifndef DISPLAY_H // include guard
#define DISPLAY_H

// #include <Adafruit_GFX.h>
// #include <Adafruit_ILI9341.h>
#include <TFT_eSPI.h>
// #include <SystemFont5x7.h>
#include <TinyGPS++.h>
#include <HMC5883L.h>

class Display : public TFT_eSPI
{
public:
    using TFT_eSPI::TFT_eSPI;
    void showGrid();
    void showInfo(String msg, float value=-9999, int pos_x=0, int pos_y=0,
                      uint16_t f_color = 0xFFFF, uint16_t b_color = 0x0000);
    void showCoords(float lat, float lon, float alt);
    void showCurCadency(int cadency);
    void showAvgCadency(float avg_cadency);
    void showCurSpeed(float speed);
    void showAvgSpeed(float avg_speed);
    void showCompass(int heading);
    void error(String msg);
    void status(bool compass, TinyGPSPlus &gps, bool status_sd, int activity = 0);
};

#endif /* DISPLAY_H */