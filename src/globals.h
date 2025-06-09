// globals.h

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "Free_Fonts.h"
#include "EspMQTTClient.h"
// #include <Adafruit_ADS1X15.h>


#define FT818				// define if you are using a FT-818 instead of FT-817
//#define MQTT                // define if MQTT and Wifi should be used


// Pin definition for touch, used by the XPT2046 library
// The pin definitions for the TFT display are done in
// User_Setup.h of the TFT_eSPI library.

#define CS_PIN    13
#define TIRQ_PIN  4



// This is calibration raw touch data to screen coordinates
// There is currently no calibration feature for the user
// to redefine if needed (work in progress)
#define TS_MINX 438
#define TS_MINY 341
#define TS_MAXX 3784
#define TS_MAXY 3706

// Size of the TFT screen (ILI9341)   2.4" // 2.8" // 3.2" 
#define ILI9341_TFTHEIGHT 240
#define ILI9341_TFTWIDTH 320

// Size of the TFT screen (ST7796)  4.0"

#define ST7796_TFTHEIGHT 320
#define ST7796_TFTWIDTH 480


// Get color codes e.g. from http://drakker.org/convert_rgb565.html
#define TFT_DARKERGREY 0x5AEB
#define TFT_DARKBLUE 0x018C
#define TFT_DARKSILVER 0x8430
#define TFT_DARKRED 0xA800

// Defines ofr the menu buttons of the main page 480x320
#define KEY_X 43          // Centre of the left key
#define KEY_Y 289
#define KEY_W 75             // width of each key
#define KEY_H 48            // height of the keys
#define KEY_SPACING_X 3     // X and Y gap
#define KEY_SPACING_Y 0
#define KEY_TEXTSIZE 1      // Font size multiplier
#define NUM_OF_KEYS 6       // six keys in the row



#define MAX_MENU_PAGES 3    // by key ">>" one can shift to the right to get the next six
                            // buttons. Currently they are 3 sets (pages) of buttons.

#define LABEL1_FONT &FreeSansBold9pt7b // Key label 3 1

#define TUNE_PWR 3         // tuning power (0: 6W, 1:5W,2: 2.5W, 3: 1W

#define POF_PIN 36      // RXTX shut down
#define VUSB_PIN  15    // ACD 5V power
#define PBAT_PIN 2      // ACD bat charge
#define PTUNE_PIN 12   // Active ATU100 Tuner 


extern TFT_eSPI tft;

// this class holds the settings of the status fields arranged on top and below 
// of the frequency display

class Status_field {
 public:

    void initField(uint16_t fg_xpos,uint16_t fg_ypos,uint16_t bg_xpos,uint16_t bg_ypos,uint16_t bg_w,uint16_t bg_h)
    {
        _fg_xpos = fg_xpos;
        _fg_ypos = fg_ypos;
        _bg_xpos = bg_xpos;
        _bg_ypos = bg_ypos;
        _bg_w = bg_w;
        _bg_h = bg_h;
    }

    void refresh() {
        drawField(_str, _fg_color, _bg_color);
    }

    void drawField(char *label, uint16_t fg_color, uint16_t bg_color)
    { 
        strncpy(_str,label,10);
        _fg_color = fg_color;
        _bg_color = bg_color;
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextDatum(TC_DATUM);
        tft.fillRect(_bg_xpos,_bg_ypos,_bg_w,_bg_h,bg_color);
        tft.setTextColor(_fg_color, _bg_color);
        tft.drawString(_str, _fg_xpos, _fg_ypos, GFXFF); // for 480x320 use boold font
    }
 private:
        char _str[10];   // todo: check needed length
 	    uint16_t _bg_xpos;
	    uint16_t _bg_ypos;
        uint16_t _bg_w;
        uint16_t _bg_color;
        uint16_t _bg_h;
	    uint16_t _fg_xpos;
	    uint16_t _fg_ypos;
        uint16_t _fg_color;
};

#endif