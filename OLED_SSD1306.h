/*
	OLED_SSD1306 - An Hardware Accellerated and CPU optimized library for drive
	SOLOMON TECH SSD1306 OLED display via SPI serial interface.
	
	Features:
	- Very FAST!, expecially with Teensy 3.x.
	- It uses just 4 or 5 wires.
	- It uses hardware accellerated commands of the OLED chip.
	
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
	Special Thanks:
	Thanks Adafruit for his Adafruit_SSD1331 used as start point for this one!
	Thanks to Hideki Kozima for his help with Hardware Accellerated functions.
	Thanks to Paul Stoffregen for his beautiful Teensy3 and Font Rendering technology
	
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	Version:
	0.5b1: First release, compile and working.
	0.5b2: Even more faster! Tuned  a couple of fixed delays.
	0.6b1: Cleaned code.
	0.9b1: Completely recoded, SPI Transaction compatible, faster
	Dropped Adafruit_GFX, Added Paul Stoffregen's font technology
	+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	BugList of the current version:
	
	- Still some problems with colors. 
*/

#ifndef _OLED_SSD1306H_
#define _OLED_SSD1306H_

#ifdef __cplusplus

	#include "Arduino.h"
	#include "Print.h"
	#include <limits.h>
	#include "pins_arduino.h"
	#include "wiring_private.h"
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <SPI.h>

	#ifdef __AVR__
		#include <avr/pgmspace.h>
	#elif defined(ESP8266)
		#include <pgmspace.h>
	#elif defined(__SAM3X8E__)
		#include <include/pio.h>
		#define PROGMEM
		#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
		#define pgm_read_word(addr) (*(const unsigned short *)(addr))
		typedef unsigned char prog_uchar;
	#endif

	#include "_settings/OLED_SSD1306_registers.h"
	#include "_settings/OLED_SSD1306_settings.h"

	#ifndef swap
		#define swap(a, b) { typeof(a) t = a; a = b; b = t; }
	#endif

	#if defined(SPI_HAS_TRANSACTION)
		static SPISettings SSD1306_SPI;
	#endif

	enum OLEDSSD1306SCRLLTYPE {HR=0,HL=1,VR=2,VL=3 };
	
#endif

#include "font.h"

#ifdef __cplusplus

// the class
class OLED_SSD1306 : public Print {
 public:
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255,uint8_t mosi=11,uint8_t sclk=13);
	#elif defined(__MKL26Z64__)
		OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255,uint8_t mosi=11,uint8_t sclk=13);
	#else
		OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin=255);
	#endif
	uint8_t	getError();
	void 	begin(uint8_t switchvcc = _CMD_SWITCHCAPVCC);
	void 	invertDisplay(uint8_t i);
	void 	startScroll(enum OLEDSSD1306SCRLLTYPE type,uint8_t start, uint8_t stop);
	void 	stopScroll(void);
	void 	dim(boolean dim);
	void 	clearDisplay(void);
	void 	clearScreen(void){clearDisplay();};
	int16_t height(void);
	int16_t width(void);
	void 	drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void 	drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
	void 	drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
	void 	drawRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color);
	void 	fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
	void 	fillScreen(uint16_t color);
	void 	drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void 	fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
	void 	ellipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
	void 	drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
	void 	fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color);
	void 	drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);
	void 	fillQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color);
	void 	drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color);
	void 	drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
	void 	fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,int16_t radius, uint16_t color);
	void 	drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,int16_t w, int16_t h, uint16_t color);
	void 	drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,int16_t w, int16_t h, uint16_t color, uint16_t bg);
	void 	setCursor(int16_t x, int16_t y);
	void	getCursor(int16_t &x,int16_t &y){x = _cursor_x;y = _cursor_y;};
	void 	setTextColor(uint16_t c);
	void 	setTextColor(uint16_t c, uint16_t bg);
	void 	setTextSize(uint8_t s);
	void 	setTextWrap(boolean w);
	void 	setRotation(uint8_t r);
	uint8_t	getRotation(void) {return _rotation;}
	void 	drawPixel(int16_t x, int16_t y, uint16_t color);
	void 	drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing,uint16_t color);
	void 	display(void);
	void 	setBitrate(uint32_t n);//speed for the SPI interface
	void 	setFont(const OLEDSSD1306_font_t &f) { _font = &f; }
	void 	setFontInternal(void) { _font = NULL; }
	void 	drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
	void 	drawFontChar(unsigned int c);
	virtual size_t write(uint8_t);
 protected:
 
	int16_t 				_width, _height;
	boolean					_portrait;
	int16_t 				_cursor_x, _cursor_y;
	uint8_t 				_textsize, 
							_rotation;
	uint16_t 				_textcolor, _textbgcolor;						
	boolean 				_wrap;
	const OLEDSSD1306_font_t *_font;
	
	
	#if defined(__MK20DX128__) || defined(__MK20DX256__)
		uint8_t 			pcs_data, pcs_command;
		uint8_t 			_mosi, _sclk;
		uint8_t 			_cs,_rs,_rst;
		
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1306_SPI);
			#endif
		}
		
		void endTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		}
		
		//Here's Paul Stoffregen magic in action...
		void waitFifoNotFull(void) {
			uint32_t sr;
			uint32_t tmp __attribute__((unused));
			do {
				sr = KINETISK_SPI0.SR;
				if (sr & 0xF0) tmp = SPI0_POPR;  // drain RX FIFO
			} while ((sr & (15 << 12)) > (3 << 12));
		}
		
		void waitFifoEmpty(void) {
			uint32_t sr;
			uint32_t tmp __attribute__((unused));
			do {
				sr = KINETISK_SPI0.SR;
				if (sr & 0xF0) tmp = KINETISK_SPI0.POPR;  // drain RX FIFO
			} while ((sr & 0xF0F0) > 0);// wait both RX & TX empty
		}
		

		void waitTransmitComplete(uint32_t mcr)
		__attribute__((always_inline)) {
			uint32_t tmp __attribute__((unused));
			while (1) {
				uint32_t sr = KINETISK_SPI0.SR;
				if (sr & SPI_SR_EOQF) break;  // wait for last transmit
				if (sr &  0xF0) tmp = KINETISK_SPI0.POPR;
			}
			KINETISK_SPI0.SR = SPI_SR_EOQF;
			SPI0_MCR = mcr;
			while (KINETISK_SPI0.SR & 0xF0) { tmp = KINETISK_SPI0.POPR; }
		}
	
		
		void writecommand_cont(uint8_t c) 
		__attribute__((always_inline)) {
			KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			waitFifoNotFull();
		}
		
		void writedata_cont(uint8_t d) 
		__attribute__((always_inline)) {
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_CONT;
			waitFifoNotFull();
		}

		void writecommand_last(uint8_t c) 
		__attribute__((always_inline)) {
			uint32_t mcr = SPI0_MCR;
			KINETISK_SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			waitTransmitComplete(mcr);
		}
			
		void writedata_last(uint8_t d) 
		__attribute__((always_inline)) {
			uint32_t mcr = SPI0_MCR;
			KINETISK_SPI0.PUSHR = d | (pcs_data << 16) | SPI_PUSHR_CTAS(0) | SPI_PUSHR_EOQ;
			waitTransmitComplete(mcr);
		}	
	
		
	#elif defined(__MKL26Z64__)
		uint8_t 			_mosi, _sclk;
		uint8_t 			_cs,_rs,_rst;
		volatile bool		_DC_state;
		bool				_useSPI1;
		
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1306_SPI);
			#endif
			digitalWriteFast(_cs,LOW);
		}
		
		void endTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		}
		
		void writecommand_cont(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				digitalWriteFast(_rs,LOW);
				_DC_state = 0;
			}
			SPI.transfer(c);
		}
		
		void writecommand_last(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				digitalWriteFast(_rs,LOW);
				_DC_state = 0;
			}
			SPI.transfer(c);
			digitalWriteFast(_cs,HIGH);
		}
		
		void writedata_cont(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				digitalWriteFast(_rs,HIGH);
				_DC_state = 1;
			}
			SPI.transfer(d);
		}
		
		void writedata_last(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				digitalWriteFast(_rs,HIGH);
				_DC_state = 1;
			}
			SPI.transfer(d);
			digitalWriteFast(_cs,HIGH);
		}
		
	#elif defined(__SAM3X8E__) 
		Pio 				*csport, *rsport;
		uint8_t 			_cs,_rs,_rst;
		uint32_t  			cspinmask, rspinmask;
		
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1306_SPI);
			#endif
			csport->PIO_CODR  |=  cspinmask;//LOW
		}
		
		void endTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		}
		
		void writecommand_cont(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				rsport->PIO_CODR |=  rspinmask;//LOW
				_DC_state = 0;
			}
			SPI.transfer(c);
		}
		
		void writecommand_last(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				rsport->PIO_CODR |=  rspinmask;//LOW
				_DC_state = 0;
			}
			SPI.transfer(c);
			csport->PIO_SODR  |=  cspinmask;//HI
		}
		
		void writedata_cont(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				rsport->PIO_SODR |=  rspinmask;//HI
				_DC_state = 1;
			}
			SPI.transfer(d);
		}
		
		void writedata_last(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				rsport->PIO_SODR |=  rspinmask;//HI
				_DC_state = 1;
			}
			SPI.transfer(d);
			csport->PIO_SODR  |=  cspinmask;//HI
		}
	#elif defined(__AVR__) 
		volatile uint8_t 	*csport, *rsport;
		uint8_t  			cspinmask, rspinmask;
		uint8_t 			_cs,_rs,_rst;
		volatile bool		_DC_state;
		
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1306_SPI);
			#endif
			*csport &= ~cspinmask;//LOW
		}
		
		void endTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		}
		
		void OLED_SSD1306::spiwrite(uint8_t c)
		__attribute__((always_inline)) {
			SPDR = c;
			while(!(SPSR & _BV(SPIF)));
		}
		
		void writecommand_cont(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				*rsport &= ~rspinmask;//LOW
				_DC_state = 0;
			}
			spiwrite(c);
		}
		
		void writecommand_last(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				*rsport &= ~rspinmask;//LOW
				_DC_state = 0;
			}
			spiwrite(c);
			*csport |= cspinmask;//HI
		}
		
		
		void writedata_cont(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				*rsport |=  rspinmask;//HI
				_DC_state = 1;
			}
			spiwrite(d);
		}
		
		void writedata_last(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				*rsport |=  rspinmask;//HI
				_DC_state = 1;
			}
			spiwrite(d);
			*csport |= cspinmask;//HI
		}
	#else
		uint8_t 			_cs,_rs,_rst;
		volatile bool		_DC_state;
		
		void startTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.beginTransaction(SSD1306_SPI);
			#endif
			digitalWrite(_cs,LOW);
		}
		
		void endTransaction(void)
		__attribute__((always_inline)) {
			#if defined(SPI_HAS_TRANSACTION)
				SPI.endTransaction();
			#endif
		}
		
		void writecommand_cont(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				digitalWrite(_rs,LOW);
				_DC_state = 0;
			}
			SPI.transfer(c);
		}
		
		void writecommand_last(uint8_t c) 
		__attribute__((always_inline)) {
			if (_DC_state){//should LOW
				digitalWrite(_rs,LOW);
				_DC_state = 0;
			}
			SPI.transfer(c);
			digitalWrite(_cs,HIGH);
		}
		
		
		void writedata_cont(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				digitalWrite(_rs,HIGH);
				_DC_state = 1;
			}
			SPI.transfer(d);
		}
		
		void writedata_last(uint8_t d) 
		__attribute__((always_inline)) {
			if (!_DC_state){//should HI
				digitalWrite(_rs,HIGH);
				_DC_state = 1;
			}
			spiwrite(d);
			digitalWrite(_cs,HIGH);
		}		

	#endif
		void writeCommand(uint8_t c)//fixed
		__attribute__((always_inline)) {
			startTransaction();
			writecommand_last(c);
			endTransaction();
		}

		void writeCommands(uint8_t *cmd, uint8_t length)//fixed
		__attribute__((always_inline)) {
			startTransaction();
			for (uint8_t i = 0; i < length; i++) {
				if (i < length-1){
					writecommand_cont(*cmd++);
				} else {
					writecommand_last(*cmd++);
				}
			}
			endTransaction();
		}
	
		void writeData(uint8_t d)//fixed
		__attribute__((always_inline)) {
			startTransaction();
			writedata_last(d);
			endTransaction();
		}
		
		void setRegister(const byte reg,uint8_t val)//fixed
		__attribute__((always_inline)) {
			startTransaction();
			writecommand_cont(reg);
			writecommand_last(val);
			endTransaction();
		}
		
		void setAdrsCont(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
		__attribute__((always_inline)) {
			writecommand_cont(_CMD_COLUMNADDR);
			writecommand_cont(x1);
			writecommand_cont(x2);
	
			writecommand_cont(_CMD_PAGEADDR);
			writecommand_cont(y1);
			writecommand_cont(y2);
		}
		
 private:
	uint8_t			_vccstate;
	uint8_t			_initError;
	inline void 	drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
	inline void 	drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
	void 			chipInit(uint8_t switchvcc);
	void	 		commonInit();
	void 			plot8points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	void 			plot4points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color);
	void			drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,uint16_t color);
	void			fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,int16_t delta, uint16_t color);
	void 			drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat);

};

#endif // __cplusplus
#endif