#ifndef _OLED_SSD1306_REGISTERS_H
#define _OLED_SSD1306_REGISTERS_H

const static byte _CMD_SETLOWCOLUMN = 							0x00;
const static byte _CMD_EXTERNALVCC = 							0x01;
const static byte _CMD_SWITCHCAPVCC = 							0x02;
const static byte _CMD_SETHIGHCOLUMN = 							0x10;
const static byte _CMD_MEMORYMODE = 							0x20;
const static byte _CMD_COLUMNADDR = 							0x21;
const static byte _CMD_PAGEADDR = 								0x22;
const static byte _CMD_RIGHT_HORIZONTAL_SCROLL = 				0x26;
const static byte _CMD_LEFT_HORIZONTAL_SCROLL = 				0x27;
const static byte _CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL = 	0x29;
const static byte _CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL = 	0x2A;
const static byte _CMD_ACTIVATE_SCROLL = 						0x2F;
const static byte _CMD_DEACTIVATE_SCROLL = 						0x2E;
const static byte _CMD_SETSTARTLINE = 							0x40;
const static byte _CMD_SETCONTRAST = 							0x81;
const static byte _CMD_CHARGEPUMP = 							0x8D;
const static byte _CMD_SEGREMAP = 								0xA0;
const static byte _CMD_SET_VERTICAL_SCROLL_AREA = 				0xA3;
const static byte _CMD_DISPLAYALLON_RESUME = 					0xA4;
const static byte _CMD_DISPLAYALLON = 							0xA5;
const static byte _CMD_NORMALDISPLAY = 							0xA6;
const static byte _CMD_INVERTDISPLAY = 							0xA7;
const static byte _CMD_SETMULTIPLEX = 							0xA8;
const static byte _CMD_DISPLAYOFF = 							0xAE;
const static byte _CMD_DISPLAYON = 								0xAF;
const static byte _CMD_COMSCANINC = 							0xC0;
const static byte _CMD_COMSCANDEC = 							0xC8;
const static byte _CMD_SETDISPLAYOFFSET = 						0xD3;
const static byte _CMD_SETDISPLAYCLOCKDIV = 					0xD5;
const static byte _CMD_SETPRECHARGE = 							0xD9;
const static byte _CMD_SETCOMPINS = 							0xDA;
const static byte _CMD_SETVCOMDETECT = 							0xDB;
const static byte _CMD_NOP = 									0xE3;

#if !defined(BLACK)
	#define	BLACK   		0x0000
#endif

#if !defined(WHITE)
	#define WHITE   		0xFFFF
#endif

#endif