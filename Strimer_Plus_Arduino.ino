/*
 * Strimer Plus Arduino
 * Version: 2021.10.27
 * Author: Murat TAMCI
 * Web Site: www.themt.co
 * License: Read LICENSE.txt
 * Note: In loving memory of my grandfather (Ahmet Ozdil).
 */
 
/*
 *  You can edit following lines to according your setup
 * 
 */

#define BUFFER_WIDTH 20                       // Strimer Plus horizontal led count
#define BUFFER_HEIGHT 6                       // Strimer Plus vertical led count

#define ROW_1_PIN 3                           // Arduino PWM pin for Strimer Plus row 1 led strip
#define ROW_2_PIN 5                           // Arduino PWM pin for Strimer Plus row 2 led strip
#define ROW_3_PIN 6                           // Arduino PWM pin for Strimer Plus row 3 led strip
#define ROW_4_PIN 9                           // Arduino PWM pin for Strimer Plus row 4 led strip
#define ROW_5_PIN 10                          // Arduino PWM pin for Strimer Plus row 5 led strip
#define ROW_6_PIN 11                          // Arduino PWM pin for Strimer Plus row 6 led strip

#define COLOR_ORDER GRB                       // color order for Strimer Plus led strip (FastLED)
#define CHIPSET WS2812B                       // chipset for Strimer Plus led strip (FastLED)

#define DEFAULT_MSG "Strimer Plus Arduino  "      // text message for scrolling.
#define DEFAULT_COLOR "C8FF00"                // foreground color.
#define DEFAULT_BGCOLOR "000000"              // background color.
#define DEFAULT_SCROLLDELAY 35                // scrolling delay. lower value is faster scrolling. higher value is slower.
#define DEFAULT_INVERT false                  // invert status. exchange color between bg color.
#define DEFAULT_BRIGHTNESS 200                // brightness value. range is 0-255. 0 = dark. 255 = brighted


/*
 *  Please, don't edit following lines if not necessary 
 * 
 */

#include <FastLED.h>
#include "font.h";                            // bit formatted font characters are here
#include "command.h";                         // serial com port commands parsing in here
#include "config.h";

CRGB buffer [BUFFER_HEIGHT][BUFFER_WIDTH];
int message_cursor = 0;
int character_column = 0;
String message;
unsigned int message_len = 0;
unsigned long loop_start = 0;
bool reset_waiting = false;
bool pause = false;

// process commands come from serial port

void cmdWork (String key, String value) {
  
  if (key == "msg") {
    character_column = 0;
    message_cursor = 0;
    message = value;
    message_len = value.length();
    strcpy (config.message, value.c_str());
    Serial.println ("OK");
    
  } else if (key == "color") {
    config.color = strtol(("0x" + value).c_str(), NULL, 0);
    Serial.println ("OK");
    
  } else if (key == "bgcolor") {
    config.bg_color = strtol(("0x" + value).c_str(), NULL, 0);
    Serial.println ("OK");
    
  } else if (key == "delay") {
    config.scrollDelay = value.toInt();
    Serial.println ("OK");
    
  } else if (key == "bri") {
    config.brightness = (byte)value.toInt();
    FastLED.setBrightness(config.brightness);
    Serial.println ("OK");
    
  } else if (key == "reset") {
    Serial.println ("OK");
    delay(250);
    reset_waiting = true;
    
  } else if (key == "inv") {
    config.invert = value.toInt() ? true : false;
    Serial.println ("OK");
    
  } else if (key == "pause") {
    pause = value.toInt() ? true : false;
    Serial.println ("OK");
    
  } else if (key == "save") {
    saveConfig();
    Serial.println ("OK");
    
  } else {
    Serial.println ("FAIL unkown command: " + key);
  }
}

byte column_of_font_char = B10000000;
char char_of_message;

// draw everything to led strips

void draw () {
  for (int colInx = BUFFER_WIDTH - 1; colInx > 0; colInx--) {
    for (int rowInx = 0; rowInx < BUFFER_HEIGHT; rowInx++) {
      buffer [rowInx][colInx] = buffer [rowInx][colInx - 1];
    }
  }

  if (message_cursor >= message_len) {
    character_column = 0;
    message_cursor = 0;
  }

  char_of_message = config.message[message_cursor];

  if (char_of_message >= 0x20 && char_of_message <= 0x7f) {

    byte char_width = pgm_read_byte_near(font5x6 + ((char_of_message - 0x20) * 7) + 6);

    if (character_column < char_width) {
      for (int rowInx = 0; rowInx < BUFFER_HEIGHT; rowInx++) {
        int rfRow = BUFFER_HEIGHT - rowInx - 1;
        unsigned long char_row_bit = pgm_read_byte_near(font5x6 + ((char_of_message - 0x20) * 7) + rfRow);

        if (config.invert) buffer [rowInx][0] = (char_row_bit & (column_of_font_char) >> character_column) > 0 ? config.bg_color : config.color;
        else buffer [rowInx][0] = (char_row_bit & (column_of_font_char) >> character_column) > 0 ? config.color : config.bg_color;
      }

      character_column++;
    } else {
      character_column = 0;
      message_cursor++;
    }

  }

  FastLED.show();
}

void reset() { asm volatile ("jmp 0x7800"); } // reset for ATmega323P

void setup() {
  Serial.begin (9600);

  if (!loadConfig()) {
    config.color = strtol(DEFAULT_COLOR, NULL, 0);
    config.bg_color = strtol(DEFAULT_BGCOLOR, NULL, 0);
  }

  message_len = strlen(config.message);

  for (int colInx = 0; colInx < BUFFER_WIDTH - 1; colInx++) {
    for (int rowInx = 0; rowInx < BUFFER_HEIGHT; rowInx++) {
      if (config.invert) buffer [rowInx][colInx] = config.color;
      else buffer [rowInx][colInx] = config.bg_color;
    }
  }

  FastLED.addLeds<CHIPSET, ROW_1_PIN, COLOR_ORDER>(buffer[0], BUFFER_WIDTH);
  FastLED.addLeds<CHIPSET, ROW_2_PIN, COLOR_ORDER>(buffer[1], BUFFER_WIDTH);
  FastLED.addLeds<CHIPSET, ROW_3_PIN, COLOR_ORDER>(buffer[2], BUFFER_WIDTH);
  FastLED.addLeds<CHIPSET, ROW_4_PIN, COLOR_ORDER>(buffer[3], BUFFER_WIDTH);
  FastLED.addLeds<CHIPSET, ROW_5_PIN, COLOR_ORDER>(buffer[4], BUFFER_WIDTH);
  FastLED.addLeds<CHIPSET, ROW_6_PIN, COLOR_ORDER>(buffer[5], BUFFER_WIDTH);

  FastLED.setBrightness(config.brightness);

  FastLED.show();
  
  Serial.println ("** Strimer Plus Arduino");
  Serial.println ("** Version: 2021.10.27 ** Author: Murat TAMCI www.themt.co ** License: MIT");
  Serial.println ("** Hello there! I'm waiting your commands :] Everything in README.md");

  delay(200);
}

void loop() {
  cmdProc (); // proccess commands come from serial port.

  if (millis()-loop_start > config.scrollDelay && pause == false) {
    loop_start = millis();
    draw (); // draw everything to led strips
  }

  // if reset request then do it
  
  if (reset_waiting) {
    reset_waiting = false;
    reset();
  }
}
