/*
	OLED_SSD1306 - An Hardware Accellerated and CPU optimized library for drive
	SOLOMON TECH SSD1306 OLED display via SPI serial interface.
	
	Features:
	- Very FAST!, expecially with Teensy 3.x where uses DMA SPI.
	- It uses just 4 or 5 wires.
	- It uses hardware accellerated commands of the OLED chip.
	- Compatible at command level with Adafruit display series so it's easy to adapt existing code.
	- It uses the standard Adafruit_GFX Library (you need to install). 
	
	Background:
	I developed this because I've got on ebay a display that uses this chip and cannot find
	any working library. I start by adapting an Adafruit library for another similar chip,
	the SSD1331 that it's similar but not the same but was disappointed about speed.
	In addition the Hardware Accellerated functions described on datasheet was buggy
	and not works as expected (even Adafruit leaved commented) so I spent quite a lot
	of time to find a solution. Thanks a brillant Japanese guy called Hideki Kozima
	that poit me at the right way to find a solution, here's the first fully hardware accellerated
	library developed for Arduino (not DUE) and expecially Teensy3.x that will allow
	amazing performances thanks the Paul Stoffregen's optimized SPI commands that uses DMA.
	
	Code Optimizations:
	The purpose of this library it's SPEED. I have tried to use hardware optimized calls
	where was possible and results are quite good for most applications, actually nly filled circles
    are still a bit slow. Many SPI call has been optimized by reduce un-needed triggers to RS and CS
	lines. Of course it can be improved so feel free to add suggestions.
	-------------------------------------------------------------------------------
    Copyright (c) 2014, .S.U.M.O.T.O.Y., coded by Max MC Costa.    

    OLED_SSD1306 Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OLED_SSD1306 Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
	++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    This file needs the following Libraries:
 
    Adafruit_GFX by Adafruit:
    https://github.com/adafruit/Adafruit-GFX-Library
	Remember to update GFX library often to have more features with this library!
	''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
	Special Thanks:
	Thanks Adafruit for his Adafruit_SSD1331 used as start point for this one!
	Thanks to Hideki Kozima for his help with Hardware Accellerated functions.
	Thanks to Paul Stoffregen for his beautiful Teensy3 and DMA SPI.
	
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Version:
	0.5b1: First release, compile and working.
	0.5b2: Even more faster! Tuned  a couple of fixed delays.
	0.6b1: Cleaned code.
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	BugList of the current version:
	
	- Still some problems with colors. 
*/


#ifndef _OLED_SSD1306H_
#define _OLED_SSD1306H_

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#if defined(__SAM3X8E__)
#include <include/pio.h>
  #define PROGMEM
  #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
  #define pgm_read_word(addr) (*(const unsigned short *)(addr))
typedef unsigned char prog_uchar;
#endif

#if defined(__MK20DX128__) || defined(__MK20DX256__)
	#define CTAR_24MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
	#define CTAR_16MHz   (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0) | SPI_CTAR_DBR)
	#define CTAR_12MHz   (SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
	#define CTAR_8MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) | SPI_CTAR_CSSCK(0))
	#define CTAR_6MHz    (SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))
	#define CTAR_4MHz    (SPI_CTAR_PBR(1) | SPI_CTAR_BR(1) | SPI_CTAR_CSSCK(1))
#endif

#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif


   #define SSD1306_128_64
	//#define SSD1306_128_32
	
#if defined SSD1306_128_64 && defined SSD1306_128_32
  #error "Only one SSD1306 display can be specified at once in SSD1306.h"
#endif
#if !defined SSD1306_128_64 && !defined SSD1306_128_32
  #error "At least one SSD1306 display must be specified in SSD1306.h"
#endif


#if defined SSD1306_128_64
  #define OLED_WIDTH                  128
  #define OLED_HEIGHT                 64
#endif
#if defined SSD1306_128_32
  #define OLED_WIDTH                  128
  #define OLED_HEIGHT                 32
#endif

#define BLACK 0
#define WHITE 1

#define _CMD_SETCONTRAST 			0x81
#define _CMD_DISPLAYALLON_RESUME 	0xA4
#define _CMD_DISPLAYALLON 			0xA5
#define _CMD_NORMALDISPLAY 			0xA6
#define _CMD_INVERTDISPLAY 			0xA7
#define _CMD_DISPLAYOFF 			0xAE
#define _CMD_DISPLAYON 				0xAF
#define _CMD_SETDISPLAYOFFSET 		0xD3
#define _CMD_SETCOMPINS 			0xDA
#define _CMD_SETVCOMDETECT 			0xDB
#define _CMD_SETDISPLAYCLOCKDIV 	0xD5
#define _CMD_SETPRECHARGE 			0xD9
#define _CMD_SETMULTIPLEX 			0xA8
#define _CMD_SETLOWCOLUMN 			0x00
#define _CMD_SETHIGHCOLUMN 			0x10
#define _CMD_SETSTARTLINE 			0x40
#define _CMD_MEMORYMODE 			0x20
#define _CMD_COLUMNADDR 			0x21
#define _CMD_PAGEADDR   			0x22
#define _CMD_COMSCANINC 			0xC0
#define _CMD_COMSCANDEC 			0xC8
#define _CMD_SEGREMAP 				0xA0
#define _CMD_CHARGEPUMP 			0x8D
#define _CMD_EXTERNALVCC 			0x1
#define _CMD_SWITCHCAPVCC 			0x2
#define _CMD_ACTIVATE_SCROLL 		0x2F
#define _CMD_DEACTIVATE_SCROLL 		0x2E
#define _CMD_SET_VERTICAL_SCROLL_AREA 				0xA3
#define _CMD_RIGHT_HORIZONTAL_SCROLL 				0x26
#define _CMD_LEFT_HORIZONTAL_SCROLL 				0x27
#define _CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x29
#define _CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 	0x2A


//this chip uses fixed resolution but in case...
#define OLED_MW						OLED_WIDTH -1
#define OLED_MH						OLED_HEIGHT -1

// the class
class OLED_SSD1306 : public Adafruit_GFX {
 public:
	//4 or 5 wire configuration
	OLED_SSD1306(uint8_t CS, uint8_t RS, uint8_t RST);
	OLED_SSD1306(uint8_t CS, uint8_t RS);
	void begin(uint8_t switchvcc = _CMD_SWITCHCAPVCC);
	void invertDisplay(uint8_t i);
	void startscrollright(uint8_t start, uint8_t stop);
	void startscrollleft(uint8_t start, uint8_t stop);
	void startscrolldiagright(uint8_t start, uint8_t stop);
	void startscrolldiagleft(uint8_t start, uint8_t stop);
	void stopscroll(void);
	void dim(uint8_t contrast);
	void clearDisplay(void);
	virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	//following are hardware accellerated and will be used by Adafruit_GFX.
	//please refere to Adafruit_GFX for more functions.
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	void display();
	//in case it's needed
	void writeData(uint8_t data);
	void setBitrate(uint32_t n);//speed for the SPI interface
  
 private:
	uint8_t			_vccstate;
	bool			_inited;
	inline void 	drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
	inline void 	drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
	void 			writeCommand(uint8_t c);
	void  			writeCommands(uint8_t *cmd, uint8_t length);
	void 			setRegister(const uint8_t reg,uint8_t val);


	void 			chipInit(uint8_t switchvcc);
	void	 		commonInit();
	
#if defined(__AVR__)
	void	 		spiwrite(uint8_t);
	volatile uint8_t *dataport, *clkport, *csport, *rsport;
	uint8_t 		_cs,_rs,_sid,_sclk,_rst;
	uint8_t  		datapinmask, clkpinmask, cspinmask, rspinmask;
#endif
#if defined(__SAM3X8E__)
	void	 		spiwrite(uint8_t);
	Pio *dataport, *clkport, *csport, *rsport;
	uint8_t 		_cs,_rs,_sid,_sclk,_rst;
	uint32_t  		datapinmask, clkpinmask, cspinmask, rspinmask;
#endif 

#if defined(__MK20DX128__) || defined(__MK20DX256__)
	uint8_t 		_cs,_rs,_sid,_sclk,_rst;
	uint8_t 		pcs_data, pcs_command;
	uint32_t 		ctar;
	volatile uint8_t *datapin, *clkpin, *cspin, *rspin;
#endif
};
#endif