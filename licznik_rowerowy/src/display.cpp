#include "display.h"
#include <ILI9341_due_config.h>
#include <ILI9341_due.h>
#include <SystemFont5x7.h>
#include <math.h>

void Display::showGrid(){
    // ILI9341_due::drawRect(0, 10, 120, 120, ILI9341_GRAY);
    // ILI9341_due::drawRect(0, 130, 320, 120, ILI9341_GRAY);
    // ILI9341_due::drawRect(120, 10, 120, 120, ILI9341_GRAY);
    // ILI9341_due::drawRect(0, 0, 320, 10, ILI9341_NAVY);
    ILI9341_due::fillRect(0, 0, 320, 10, ILI9341_NAVY);
}

void Display::showAvgCadency(int avg_cadency){
    ILI9341_due::setTextScale(2);
    ILI9341_due::setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);
    ILI9341_due::setTextArea(120, 230, 120, 100);

    ILI9341_due::cursorToXY(10, 5);
    ILI9341_due::print("AVG CAD");
    ILI9341_due::setTextScale(2);
    ILI9341_due::cursorTo(1, 2);
    if(avg_cadency < 100 && avg_cadency > 9){
        ILI9341_due::print(" ");
    }else if (avg_cadency < 10){
        ILI9341_due::print("  ");
    }


    ILI9341_due::print(avg_cadency);
}

void Display::showAvgSpeed(float avg_speed){
    ILI9341_due::setTextScale(2);
    ILI9341_due::setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);
    ILI9341_due::setTextArea(0, 230, 120, 100);

    ILI9341_due::cursorToXY(10, 5);
    ILI9341_due::print("AVG SPD");

    ILI9341_due::setTextScale(2);
    ILI9341_due::cursorTo(1, 2);
    ILI9341_due::print(avg_speed);
}

void Display::showCurSpeed(float speed){
    ILI9341_due::setTextScale(2);
    ILI9341_due::setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);
    ILI9341_due::setTextArea(0, 100, 240, 20);
    ILI9341_due::cursorTo(15, 1);
    ILI9341_due::print("SPEED");

    ILI9341_due::setTextScale(6);
    ILI9341_due::cursorTo(7, 1);
    ILI9341_due::print(speed, 1);

    ILI9341_due::setTextScale(2);
    ILI9341_due::cursorTo(28, 3);
    ILI9341_due::print("KM/H");
}

void Display::showCurCadency(int cadency){
    ILI9341_due::setTextScale(2);
    ILI9341_due::setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);
    ILI9341_due::setTextArea(120, 10, 120, 30);

    ILI9341_due::cursorToXY(10, 2);
    ILI9341_due::print("CAD");

    ILI9341_due::setTextArea(120, 50, 120, 50);
    ILI9341_due::clearTextArea(ILI9341_LIGHTGREY);
    ILI9341_due::setTextScale(3);
    ILI9341_due::cursorTo(1, 0);
    ILI9341_due::print(cadency, 1);
    ILI9341_due::setTextScale(2);
    ILI9341_due::cursorTo(12, 0);
    ILI9341_due::print("RPM");
}

void Display::showCoords(float lat, float lon, float alt){
    ILI9341_due::setTextScale(2);
    ILI9341_due::setTextColor(ILI9341_BLACK, ILI9341_LIGHTGREY);
    ILI9341_due::setTextArea(0, 10, 120, 120);
    ILI9341_due::cursorToXY(10, 5);
    ILI9341_due::print("POS");
    ILI9341_due::setTextScale(1);

    ILI9341_due::cursorTo(1, 3);
    ILI9341_due::print(lat, 6);
    ILI9341_due::cursorToXY(6, 32);
    ILI9341_due::print(lon, 6);
    ILI9341_due::cursorToXY(6, 42);
    ILI9341_due::print(alt, 2);
}

void Display::error(String msg){
    ILI9341_due::setTextScale(2);
    ILI9341_due::drawRect(0, 300, 320, 20, ILI9341_RED);
    ILI9341_due::setTextColor(ILI9341_WHITE, ILI9341_RED);
    ILI9341_due::fillRect(0, 300, 320, 20, ILI9341_RED);
    ILI9341_due::cursorTo(0, 300);
    ILI9341_due::print(msg);
    Serial.println(msg);
}

void Display::status(HMC5883L_Simple &compass, TinyGPSPlus &gps){
    ILI9341_due::setTextArea(0, 0, 320, 10);
    ILI9341_due::setTextScale(1);
    ILI9341_due::setTextColor(ILI9341_WHITE, ILI9341_NAVY);
    ILI9341_due::cursorTo(0, 0);
    if(gps.location.isValid()){
        ILI9341_due::print("GPS: ");
        ILI9341_due::print(gps.satellites.value());
    }
    if(compass.GetHeadingDegrees()){
        ILI9341_due::print(" CMP: OK ");
    }
    if (true){
        ILI9341_due::print(" HAL: OK ");
    }
}

void Display::showCompass(int angle){
    int center_x = 60;
    int center_y = 85;
    int radius = 30;
    
    int x0 = center_x;
    int y0 = center_y - radius;
    
    float rad = angle*M_PI/180;

    int x0_new = (x0 - center_x) * cos(rad) - (y0 - center_y) * sin(rad) + center_x;
    int y0_new = (x0 - center_x) * sin(rad) + (y0 - center_y) * cos(rad) + center_y;

    ILI9341_due::fillCircle(center_x, center_y, radius+2, ILI9341_LIGHTGREY);
    ILI9341_due::drawCircle(center_x, center_y, radius, ILI9341_BLACK);
    ILI9341_due::drawLine(center_x, center_y, x0_new, y0_new, ILI9341_BLACK);
}

