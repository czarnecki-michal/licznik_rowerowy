// sd_card.h
#ifndef SD_CARD_H // include guard
#define SD_CARD_H

#include <SD.h>

File myFile;

bool initializeSdCard(){
    Serial.print("Initializing SD card...");

    bool sd_init = false;

    while(!sd_init){
        Serial.print("Initializing SD card...");
        sd_init = SD.begin(26);
        if(millis() > 20000U){
            break;
        }
    };

    if (!sd_init) {
        Serial.println("initialization failed!");
        return false;
    }else{
        Serial.println("initialization done.");
        return true;
    }
}



#endif /* SD_CARD_H */
