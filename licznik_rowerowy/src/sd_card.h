// sd_card.h
#ifndef SD_CARD_H // include guard
#define SD_CARD_H

#include <SD.h>

File myFile;

void initializeSdCard(){
    Serial.print("Initializing SD card...");

    if (!SD.begin(26)) {
        Serial.println("initialization failed!");
        while (1);
    }
    Serial.println("initialization done.");
}

// void writeToFile(String msg){
//     myFile = SD.open("test.txt", FILE_WRITE);

//     // if the file opened okay, write to it:
//     if (myFile) {
//         Serial.print("Writing to test.txt...");
//         myFile.println(msg);
//         // close the file:
//         myFile.close();
//         Serial.println("done.");
//     } else {
//         // if the file didn't open, print an error:
//         Serial.println("error opening test.txt");
//     }
// }





#endif /* SD_CARD_H */
