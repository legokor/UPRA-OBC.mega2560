#ifndef SDCARD_H
#define SDCARD_H

//SD CARD - SPI
#define MOSI    51        //MOSI
#define MISO    50        //MISO
#define CLK     52        //CLK        
#define SD_CS   26        //CHIP SELECT


SdFat sd;
SdFile dataFile;


#endif
