#include <TFT_eSPI.h>
#include <HMC5883L.h>
#include "display.h"

void Display::showGrid(){
    TFT_eSPI::fillRect(0, 0, 320, 10, TFT_NAVY);
    
}

void Display::showAvgCadency(float avg_cadency){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setTextColor(TFT_BLACK, TFT_LIGHTGREY);

    TFT_eSPI::setCursor(120, 230);
    TFT_eSPI::print("AVG CAD");
    TFT_eSPI::setCursor(140, 250);

    TFT_eSPI::print(avg_cadency, 1);
}

void Display::showCurCadency(int cadency){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setTextColor(TFT_BLACK, TFT_LIGHTGREY);

    TFT_eSPI::setCursor(120, 20);
    TFT_eSPI::print("CAD");

    TFT_eSPI::setTextSize(4);
    TFT_eSPI::setCursor(120, 60);

    if(cadency < 100 && cadency > 9){
        TFT_eSPI::print(" ");
    }else if (cadency < 10){
        TFT_eSPI::print("  ");
    }

    TFT_eSPI::print(cadency);

    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setCursor(195, 60);
    TFT_eSPI::print("RPM");
}

void Display::showAvgSpeed(float avg_speed){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setTextColor(TFT_BLACK, TFT_LIGHTGREY);

    TFT_eSPI::setCursor(20, 230);
    TFT_eSPI::print("AVG SPD");
    TFT_eSPI::setCursor(40, 250);

    TFT_eSPI::print(avg_speed, 1);
}

void Display::showCurSpeed(float speed){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    TFT_eSPI::setCursor(90, 120);
    TFT_eSPI::print("SPEED");

    TFT_eSPI::setTextSize(5);
    TFT_eSPI::setCursor(40, 145);
    TFT_eSPI::print(speed, 1);

    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setCursor(145, 165);
    TFT_eSPI::print("KM/H");
}

void Display::showCoords(float lat, float lon, float alt){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    TFT_eSPI::setCursor(0, 20);
    TFT_eSPI::print("POS");

    TFT_eSPI::setTextSize(1);

    TFT_eSPI::setCursor(0, 40);
    TFT_eSPI::print(lat, 3);
    TFT_eSPI::setCursor(40, 40);
    TFT_eSPI::print(lon, 3);
    TFT_eSPI::setCursor(80, 40);
    TFT_eSPI::print(alt, 3);
}

void Display::error(String msg){
    TFT_eSPI::setTextSize(2);
    TFT_eSPI::drawRect(0, 300, 320, 20, TFT_RED);
    TFT_eSPI::setTextColor(TFT_WHITE, TFT_RED);
    TFT_eSPI::fillRect(0, 300, 320, 20, TFT_RED);
    TFT_eSPI::setCursor(0, 300);
    TFT_eSPI::print(msg);
    Serial.println(msg);
}
void Display::status(HMC5883L &compass, TinyGPSPlus &gps, int activity){
    TFT_eSPI::setTextSize(1);
    // TFT_eSPI::drawRect(0, 0, 320, 10, TFT_NAVY);
    TFT_eSPI::fillRect(0, 0, 320, 10, TFT_NAVY);
    TFT_eSPI::setTextColor(TFT_WHITE, TFT_NAVY);
    TFT_eSPI::setCursor(0, 2);
    if(gps.location.isValid()){
        TFT_eSPI::print("GPS: ");
        TFT_eSPI::print(gps.satellites.value());
    }
    TFT_eSPI::print(" CMP: OK ");

    if (true){
        TFT_eSPI::print(" HAL: OK ");
    }

    TFT_eSPI::setCursor(200, 2);
    if(activity == 1){
        TFT_eSPI::print(" > ");
    }else{
        TFT_eSPI::print(" || ");
    }
}

void Display::showCompass(int angle){
    int center_x = 60;
    int center_y = 85;
    int radius = 30;
    
    int x0 = center_x;
    int y0 = center_y - radius;
    
    float rad = angle;

    int x0_new = (x0 - center_x) * cos(rad) - (y0 - center_y) * sin(rad) + center_x;
    int y0_new = (x0 - center_x) * sin(rad) + (y0 - center_y) * cos(rad) + center_y;

    TFT_eSPI::fillCircle(center_x, center_y, radius+2, TFT_LIGHTGREY);
    TFT_eSPI::drawCircle(center_x, center_y, radius, TFT_BLACK);
    TFT_eSPI::drawLine(center_x, center_y, x0_new, y0_new, TFT_BLACK);
}

