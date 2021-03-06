// -*- mode: c++ -*-
// Copyright 2013 Pervasive Displays, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.  See the License for the specific language
// governing permissions and limitations under the License.



#include <inttypes.h>
#include <ctype.h>

// required libraries
#include <SPI.h>
#include <FLASH.h>
#include <EPD2.h>
#include <Wire.h>
#include <LM75.h>

#include "pixelbit.h"

// Change this for different display size
// supported sizes: 144 200 270
#define SCREEN_SIZE 270

// select two images from:  text_image ea cat aphrodite venus saturn
#define IMAGE_1  aphrodite
#define IMAGE_2  text_image

// set up images from screen size2
#if (SCREEN_SIZE == 144)
#error "not yet available"
#define EPD_SIZE EPD_1_44
#define FILE_SUFFIX _1_44.xbm
#define NAME_SUFFIX _1_44_bits

#elif (SCREEN_SIZE == 200)
#define EPD_SIZE EPD_2_0
#define FILE_SUFFIX _2_0.xbm
#define NAME_SUFFIX _2_0_bits

#elif (SCREEN_SIZE == 270)
#define EPD_SIZE EPD_2_7
#define FILE_SUFFIX _2_7.xbm
#define NAME_SUFFIX _2_7_bits

#else
#error "Unknown EPB size: Change the #define SCREEN_SIZE to a supported value"
#endif

// Error message for MSP430
#if (SCREEN_SIZE == 270) && defined(__MSP430_CPU__)
#error MSP430: not enough memory
#endif

// no futher changed below this point

// current version number
#define DEMO_VERSION "1"


// pre-processor convert to string
#define MAKE_STRING1(X) #X
#define MAKE_STRING(X) MAKE_STRING1(X)

// other pre-processor magic
// tiken joining and computing the string for #include
#define ID(X) X
#define MAKE_NAME1(X,Y) ID(X##Y)
#define MAKE_NAME(X,Y) MAKE_NAME1(X,Y)
#define MAKE_JOIN(X,Y) MAKE_STRING(MAKE_NAME(X,Y))

// calculate the include name and variable names
#define IMAGE_1_FILE MAKE_JOIN(IMAGE_1,FILE_SUFFIX)
#define IMAGE_1_BITS MAKE_NAME(IMAGE_1,NAME_SUFFIX)
#define IMAGE_2_FILE MAKE_JOIN(IMAGE_2,FILE_SUFFIX)
#define IMAGE_2_BITS MAKE_NAME(IMAGE_2,NAME_SUFFIX)


// Add Images library to compiler path
#include <Images.h>  // this is just an empty file

// images
PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_1_FILE
#undef char
#undef unsigned

PROGMEM const
#define unsigned
#define char uint8_t
#include IMAGE_2_FILE
#undef char
#undef unsigned



#if defined(__MSP430_CPU__)

// TI LaunchPad IO layout
#error TI LaunchPad not tested
//const int Pin_TEMPERATURE = A4;
//const int Pin_PANEL_ON = P2_3;
//const int Pin_BORDER = P2_5;
//const int Pin_DISCHARGE = P2_4;
//const int Pin_PWM = P2_1;
//const int Pin_RESET = P2_2;
//const int Pin_BUSY = P2_0;
//const int Pin_EPD_CS = P2_6;
//const int Pin_FLASH_CS = P2_7;
//const int Pin_SW2 = P1_3;
//const int Pin_RED_LED = P1_0;

#else

// Arduino IO layout
//const int Pin_TEMPERATURE = A0; // Temperature is handled by LM75 over I2C and not an analog pin
const int Pin_PANEL_ON = 5;
const int Pin_BORDER = 10;
const int Pin_DISCHARGE = 4;
//const int Pin_PWM = 5;    // Not used by COG v2
const int Pin_RESET = 6;
const int Pin_BUSY = 7;
const int Pin_EPD_CS = 8;
const int Pin_FLASH_CS = 9;
const int Pin_SW2 = 12;
const int Pin_RED_LED = 13;

#endif


// LED anode through resistor to I/O pin
// LED cathode to Ground
#define LED_ON  HIGH
#define LED_OFF LOW


// define the E-Ink display
EPD_Class EPD(EPD_SIZE, Pin_PANEL_ON, Pin_BORDER, Pin_DISCHARGE, Pin_RESET, Pin_BUSY, Pin_EPD_CS);

// 
display ea27;

// 264 * 176 / 8
char frame_buffer[ 5808 ] = { 0 };

// I/O setup
void setup() {
	pinMode(Pin_RED_LED, OUTPUT);
	pinMode(Pin_SW2, INPUT);
	//pinMode(Pin_TEMPERATURE, INPUT);
	//pinMode(Pin_PWM, OUTPUT);
	pinMode(Pin_BUSY, INPUT);
	pinMode(Pin_RESET, OUTPUT);
	pinMode(Pin_PANEL_ON, OUTPUT);
	pinMode(Pin_DISCHARGE, OUTPUT);
	pinMode(Pin_BORDER, OUTPUT);
	pinMode(Pin_EPD_CS, OUTPUT);
	pinMode(Pin_FLASH_CS, OUTPUT);

	digitalWrite(Pin_RED_LED, LOW);
	//digitalWrite(Pin_PWM, LOW);  // not actually used - set low so can use current eval board unmodified
	digitalWrite(Pin_RESET, LOW);
	digitalWrite(Pin_PANEL_ON, LOW);
	digitalWrite(Pin_DISCHARGE, LOW);
	digitalWrite(Pin_BORDER, LOW);
	digitalWrite(Pin_EPD_CS, LOW);
	digitalWrite(Pin_FLASH_CS, HIGH);

	Serial.begin(9600);
#if !defined(__MSP430_CPU__)
	// wait for USB CDC serial port to connect.  Arduino Leonardo only
	while (!Serial) {
	}
	delay(20);  // allows terminal time to sync
#endif
	Serial.println();
	Serial.println();
	Serial.println("Demo G2 version: " DEMO_VERSION);
	Serial.println("Display: " MAKE_STRING(EPD_SIZE));
	Serial.println();

	FLASH.begin(Pin_FLASH_CS);
	if (FLASH.available()) {
		Serial.println("FLASH chip detected OK");
	} else {
		uint8_t maufacturer;
		uint16_t device;
		FLASH.info(&maufacturer, &device);
		Serial.print("unsupported FLASH chip: MFG: 0x");
		Serial.print(maufacturer, HEX);
		Serial.print("  device: 0x");
		Serial.print(device, HEX);
		Serial.println();
	}

	// configure temperature sensor
	Wire.begin();
	LM75.begin();

  // empty framebuffer
  memset(frame_buffer,0,sizeof(frame_buffer));

  ea27.frame_buffer = frame_buffer;
  ea27.w = 264; 
  ea27.h = 176;
}


static int state = 0;
int latch = 1;

// main loop
void loop() {
  int temperature = LM75.read();

  if (latch) {
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.println(" Celcius");
    Serial.println(int(frame_buffer[0]));
    Serial.println(int(frame_buffer[1]));

    EPD.begin(); // power up the EPD panel
    EPD.clear();
    EPD.setFactor(temperature); // adjust for current temperature
   
    /*EPD.image(IMAGE_1_BITS);*/

    pixelbit_vline(&ea27, 0, 175, 20);
    pixelbit_vline(&ea27, 0, 175, 243);

    pixelbit_hline(&ea27, 0, 263, 20);
    pixelbit_hline(&ea27, 0, 263, 155);
    
    
    pixelbit_hline(&ea27, 20, 130, 40);

    pixelbit_rectangle(&ea27, 60, 60, 
                              100, 100, 0);
    
    pixelbit_rectangle(&ea27, 80, 80, 
                              120, 120, 1);

    // top left corner
    pixelbit_line(&ea27, 0, 0, 20, 20);
    pixelbit_line(&ea27, 20, 0, 0, 20);
   
    // bottom left corner
    pixelbit_line(&ea27, 0, 155, 20, 175);
    pixelbit_line(&ea27, 20, 155, 0, 175);

    // top right corner
    pixelbit_line(&ea27, 243, 0, 263, 20);
    pixelbit_line(&ea27, 263, 0, 243, 20);
    
    // bottom right corner
    pixelbit_line(&ea27, 243, 155, 263, 175);
    pixelbit_line(&ea27, 263, 155, 243, 175);

    // top pane
    pixelbit_line(&ea27, 20, 0, 243, 20);
    pixelbit_line(&ea27, 20, 20, 243, 0);

    EPD.image_sram(frame_buffer);
    
    EPD.end();   // power down the EPD panel

    latch = 0;
  }

  delay(10000);	
}
