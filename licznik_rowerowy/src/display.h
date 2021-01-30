// display.h
#ifndef DISPLAY_H // include guard
#define DISPLAY_H

#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>
#include <TinyGPS++.h>
#include <HMC5883L_Simple.h>

class Display : public ILI9341_due
{
public:
    using ILI9341_due::ILI9341_due;
    void showGrid();
    void showInfo(String msg, float value=-9999, int pos_x=0, int pos_y=0,
                      uint16_t f_color = 0xFFFF, uint16_t b_color = 0x0000);
    void showCoords(float lat, float lon, float alt);
    void showCurCadency(int cadency);
    void showAvgCadency(int avg_cadency);
    void showCurSpeed(float speed);
    void showAvgSpeed(float avg_speed);
    void showCompass(int heading);
    void error(String msg);
    void status(HMC5883L_Simple &compass, TinyGPSPlus &gps);
};

#endif /* DISPLAY_H */