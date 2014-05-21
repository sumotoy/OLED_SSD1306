

#include "Adafruit_GFX.h"
#include "OLED_SSD1306.h"
//#include "glcdfont.c" //comment out if you are using the custom version of Adafruit_GFX!!!!!!!!!
#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif
#ifndef __SAM3X8E__
 #include <util/delay.h>
#endif
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

static uint8_t buffer[OLED_HEIGHT * OLED_WIDTH / 8] = { 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x80, 0x80, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xF8, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0x80, 0x80, 0x00, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x8C, 0x8E, 0x84, 0x00, 0x00, 0x80, 0xF8,
0xF8, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80,
0x00, 0xE0, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xC7, 0x01, 0x01,
0x01, 0x01, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x7C, 0xFE, 0xC7, 0x01, 0x01, 0x01, 0x01, 0x83, 0xFF,
0xFF, 0xFF, 0x00, 0x38, 0xFE, 0xC7, 0x83, 0x01, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF, 0x00, 0x00,
0x01, 0xFF, 0xFF, 0x01, 0x01, 0x00, 0xFF, 0xFF, 0x07, 0x01, 0x01, 0x01, 0x00, 0x00, 0x7F, 0xFF,
0x80, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0xFF,
0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0x0F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC7, 0xC7, 0x8F,
0x8F, 0x9F, 0xBF, 0xFF, 0xFF, 0xC3, 0xC0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xFC,
0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x01, 0x03, 0x03, 0x03,
0x03, 0x03, 0x01, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01,
0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x03, 0x03, 0x00, 0x00,
0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x00, 0x00, 0x00, 0x03,
0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
#if (OLED_HEIGHT == 64)
0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x1F, 0x0F,
0x87, 0xC7, 0xF7, 0xFF, 0xFF, 0x1F, 0x1F, 0x3D, 0xFC, 0xF8, 0xF8, 0xF8, 0xF8, 0x7C, 0x7D, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x0F, 0x07, 0x00, 0x30, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xC0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xC0, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x7F, 0x3F, 0x1F,
0x0F, 0x07, 0x1F, 0x7F, 0xFF, 0xFF, 0xF8, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00,
0x00, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x0E, 0xFC, 0xF8, 0x00, 0x00, 0xF0, 0xF8, 0x1C, 0x0E,
0x06, 0x06, 0x06, 0x0C, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFC, 0x00, 0x18, 0x3C, 0x7E, 0x66, 0xE6, 0xCE, 0x84, 0x00, 0x00, 0x06, 0xFF, 0xFF, 0x06,
0x06, 0xFC, 0xFE, 0xFC, 0x0C, 0x06, 0x06, 0x06, 0x00, 0x00, 0xFE, 0xFE, 0x00, 0x00, 0xC0, 0xF8,
0xFC, 0x4E, 0x46, 0x46, 0x46, 0x4E, 0x7C, 0x78, 0x40, 0x18, 0x3C, 0x76, 0xE6, 0xCE, 0xCC, 0x80,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x1F, 0x3F, 0x3F, 0x3F, 0x3F, 0x1F, 0x0F, 0x03,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x03, 0x07, 0x0E, 0x0C,
0x18, 0x18, 0x0C, 0x06, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x01, 0x0F, 0x0E, 0x0C, 0x18, 0x0C, 0x0F,
0x07, 0x01, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00,
0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x07,
0x07, 0x0C, 0x0C, 0x18, 0x1C, 0x0C, 0x06, 0x06, 0x00, 0x04, 0x0E, 0x0C, 0x18, 0x0C, 0x0F, 0x07,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif
};


//Begin function
void OLED_SSD1306::begin(uint8_t switchvcc) {
	_inited = false;
	_vccstate = switchvcc;
	commonInit();
	if (_inited) chipInit(_vccstate);
}

void OLED_SSD1306::invertDisplay(uint8_t i) {
  if (i) {
    writeCommand(_CMD_INVERTDISPLAY);
  } else {
    writeCommand(_CMD_NORMALDISPLAY);
  }
}

void OLED_SSD1306::startscrollright(uint8_t start, uint8_t stop){
	uint8_t cmd[7] = {_CMD_RIGHT_HORIZONTAL_SCROLL,0x00,start,0x00,stop,0x00,0xFF};
	writeCommands(cmd, 7);
	writeCommand(_CMD_ACTIVATE_SCROLL);
}

void OLED_SSD1306::startscrollleft(uint8_t start, uint8_t stop){
	uint8_t cmd[7] = {_CMD_LEFT_HORIZONTAL_SCROLL,0x00,start,0x00,stop,0x00,0xFF};
	writeCommands(cmd, 7);
	writeCommand(_CMD_ACTIVATE_SCROLL);
}

void OLED_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop){
	uint8_t cmd[6];
	cmd[0] = _CMD_SET_VERTICAL_SCROLL_AREA;
	cmd[1] = 0x00;
	cmd[2] = OLED_HEIGHT;
	writeCommands(cmd, 3);
	cmd[0] = _CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL;
	cmd[1] = 0x00;
	cmd[2] = start;
	cmd[3] = 0x00;
	cmd[4] = stop;
	cmd[5] = 0x01;
	writeCommands(cmd, 6);
	writeCommand(_CMD_ACTIVATE_SCROLL);
}

void OLED_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop){
	uint8_t cmd[6];
	cmd[0] = _CMD_SET_VERTICAL_SCROLL_AREA;
	cmd[1] = 0x00;
	cmd[2] = OLED_HEIGHT;
	writeCommands(cmd, 3);
	cmd[0] = _CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL;
	cmd[1] = 0x00;
	cmd[2] = start;
	cmd[3] = 0x00;
	cmd[4] = stop;
	cmd[5] = 0x01;
	writeCommands(cmd, 6);
	writeCommand(_CMD_ACTIVATE_SCROLL);
}

void OLED_SSD1306::stopscroll(void){
	writeCommand(_CMD_DEACTIVATE_SCROLL);
}


void OLED_SSD1306::dim(boolean dim) {
  uint8_t contrast;
  if (dim) {
    contrast = 0; // Dimmed display
  } else {
    if (_vccstate == _CMD_EXTERNALVCC) {
      contrast = 0x9F;
    } else {
      contrast = 0xCF;
    }
  }
  setRegister(_CMD_SETCONTRAST,contrast);
}


void OLED_SSD1306::clearDisplay(void) {
  memset(buffer, 0, (OLED_WIDTH*OLED_HEIGHT/8));
}


void OLED_SSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  bool bSwap = false;
  switch(rotation) { 
    case 0:
      // 0 degree rotation, do nothing
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x
      bSwap = true;
      swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      x -= (w-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
      bSwap = true;
      swap(x, y);
      y = HEIGHT - y - 1;
      y -= (w-1);
      break;
  }
  if (bSwap) { 
    drawFastVLineInternal(x, y, w, color);
  } else { 
    drawFastHLineInternal(x, y, w, color);
  }
}

void OLED_SSD1306::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) {
  // Do bounds/limit checks
  if(y < 0 || y >= HEIGHT) return;
  // make sure we don't try to draw below 0
  if(x < 0) { 
    w += x;
    x = 0;
  }
  // make sure we don't go off the edge of the display
  if ((x + w) > WIDTH) w = (HEIGHT- x);
  // if our width is now negative, punt
  if (w <= 0) return;
  // set up the pointer for  movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * OLED_WIDTH);
  // and offset x columns in
  pBuf += x;
  register uint8_t mask = 1 << (y&7);
  if (color == WHITE) { 
    while(w--) { *pBuf++ |= mask; }
  } else {
    mask = ~mask;
    while(w--) { *pBuf++ &= mask; }
  }
}


void OLED_SSD1306::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  bool bSwap = false;
  switch(rotation) { 
    case 0:
      break;
    case 1:
      // 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
      bSwap = true;
      swap(x, y);
      x = WIDTH - x - 1;
      x -= (h-1);
      break;
    case 2:
      // 180 degree rotation, invert x and y - then shift y around for height.
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      y -= (h-1);
      break;
    case 3:
      // 270 degree rotation, swap x & y for rotation, then invert y 
      bSwap = true;
      swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }

  if (bSwap) { 
    drawFastHLineInternal(x, y, h, color);
  } else {
    drawFastVLineInternal(x, y, h, color);
  }
}

void OLED_SSD1306::drawFastVLineInternal(int16_t x, int16_t __y, int16_t __h, uint16_t color) {
  // do nothing if we're off the left or right side of the screen
  if (x < 0 || x >= WIDTH) return;
  // make sure we don't try to draw below 0
  if (__y < 0) { 
    // __y is negative, this will subtract enough from __h to account for __y being 0
    __h += __y;
    __y = 0;
  } 
  // make sure we don't go past the height of the display
  if ((__y + __h) > HEIGHT)  __h = (HEIGHT - __y);
  // if our height is now negative, punt 
  if (__h <= 0) return;

  // this display doesn't need ints for coordinates, use local byte registers for faster juggling
  register uint8_t y = __y;
  register uint8_t h = __h;
  // set up the pointer for fast movement through the buffer
  register uint8_t *pBuf = buffer;
  // adjust the buffer pointer for the current row
  pBuf += ((y/8) * OLED_WIDTH);
  // and offset x columns in
  pBuf += x;
  // do the first partial byte, if necessary - this requires some masking
  register uint8_t mod = (y&7);
  if (mod) {
    // mask off the high n bits we want to set 
    mod = 8-mod;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    // register uint8_t mask = ~(0xFF >> (mod));
    static uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
    register uint8_t mask = premask[mod];
    // adjust the mask if we're not going to reach the end of this byte
    if (h < mod) mask &= (0XFF >> (mod-h));
    if (color == WHITE) { 
      *pBuf |= mask;
    } else {
      *pBuf &= ~mask;
    }
    // fast exit if we're done here!
    if (h < mod) return;
    h -= mod;
    pBuf += OLED_WIDTH;
  }
  // write solid bytes while we can - effectively doing 8 rows at a time
  if (h >= 8) { 
    // store a local value to work with 
    register uint8_t val = (color == WHITE) ? 255 : 0;
    do  {
      // write our value in
      *pBuf = val;
      // adjust the buffer forward 8 rows worth of data
      pBuf += OLED_WIDTH;
      // adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
      h -= 8;
    } while (h >= 8);
  }
  // now do the final partial byte, if necessary
  if (h) {
    mod = h & 7;
    // this time we want to mask the low bits of the byte, vs the high bits we did above
    // register uint8_t mask = (1 << mod) - 1;
    // note - lookup table results in a nearly 10% performance improvement in fill* functions
    static uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
    register uint8_t mask = postmask[mod];
    if (color == WHITE) { 
      *pBuf |= mask;
    } else { 
      *pBuf &= ~mask;
    }
  }
}
  
  

/***********************************/


/*
Hardware accellerated pixel set by drawing a 1 pixel rectangle
*/
void OLED_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color){
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height())) return;
	// check rotation, move pixel around if necessary
	switch (getRotation()) {
	case 1:
		swap(x, y);
		x = WIDTH - x - 1;
		break;
	case 2:
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		break;
	case 3:
		swap(x, y);
		y = HEIGHT - y - 1;
		break;
	}
	
	if (color == WHITE) {
		buffer[x+ (y/8)*OLED_WIDTH] |= (1 << (y&7));  
	} else {
		buffer[x+ (y/8)*OLED_WIDTH] &= ~(1 << (y&7)); 
	}
}



/********************************* low level pin initialization */


OLED_SSD1306::OLED_SSD1306(uint8_t cs, uint8_t rs, uint8_t rst) : Adafruit_GFX(OLED_WIDTH, OLED_HEIGHT) {
    _cs = cs;
    _rs = rs;
    _rst = rst;
	//_sid  = _sclk = 0;
	_inited = false;
}

OLED_SSD1306::OLED_SSD1306(uint8_t cs, uint8_t rs) : Adafruit_GFX(OLED_WIDTH, OLED_HEIGHT) {
    _cs = cs;
    _rs = rs;
    _rst = 0;
	//_sid  = _sclk = 0;
	_inited = false;
}

//helper
void OLED_SSD1306::setRegister(const uint8_t reg,uint8_t val){
	uint8_t cmd[2] = {reg,val};
	writeCommands(cmd,2);
}

/********************************** low level pin interface */
#ifdef __AVR__

	inline void OLED_SSD1306::spiwrite(uint8_t c){
		SPDR = c;
		while(!(SPSR & _BV(SPIF)));
	}

	void OLED_SSD1306::writeCommand(uint8_t c){
		*rsport &= ~rspinmask;//LOW
		*csport &= ~cspinmask;//LOW
		spiwrite(c);
		*csport |= cspinmask;//HI
	}

	void OLED_SSD1306::writeCommands(uint8_t *cmd, uint8_t length){
		*rsport &= ~rspinmask;//LOW
		*csport &= ~cspinmask;//LOW
		for (uint8_t i = 0; i < length; i++) {
			spiwrite(*cmd++);
		}
		*csport |= cspinmask;//HI
	}
	
	void OLED_SSD1306::writeData(uint8_t c){
		*rsport |=  rspinmask;//HI
		*csport &= ~cspinmask;//LOW
		spiwrite(c);
		*csport |= cspinmask;//HI
	} 

	void OLED_SSD1306::setBitrate(uint32_t n){
		if (n >= 8000000) {
			SPI.setClockDivider(SPI_CLOCK_DIV2);
		} else if (n >= 4000000) {
			SPI.setClockDivider(SPI_CLOCK_DIV4);
		} else if (n >= 2000000) {
			SPI.setClockDivider(SPI_CLOCK_DIV8);
		} else {
			SPI.setClockDivider(SPI_CLOCK_DIV16);
		}
	}
#elif defined(__SAM3X8E__)

	inline void OLED_SSD1306::spiwrite(uint8_t c){
		SPI.transfer(c);
	}
	
	void OLED_SSD1306::writeCommand(uint8_t c){
		rsport->PIO_CODR |=  rspinmask;//LOW
		csport->PIO_CODR  |=  cspinmask;//LOW
		spiwrite(c);
		csport->PIO_SODR  |=  cspinmask;//HI
	}
	
	void OLED_SSD1306::writeCommands(uint8_t *cmd, uint8_t length){
		rsport->PIO_CODR |=  rspinmask;//LOW
		csport->PIO_CODR  |=  cspinmask;//LOW
		for (uint8_t i = 0; i < length; i++) {
			spiwrite(*cmd++);
		}
		csport->PIO_SODR  |=  cspinmask;//HI
	}
	
	void OLED_SSD1306::writeData(uint8_t c){
		rsport->PIO_SODR |=  rspinmask;//HI
		csport->PIO_CODR  |=  cspinmask;//LOW
		spiwrite(c);
		csport->PIO_SODR  |=  cspinmask;//HI
	} 
	
	
	void OLED_SSD1306::setBitrate(uint32_t n){
		uint32_t divider=1;
		while (divider < 255) {
			if (n >= 84000000 / divider) break;
			divider = divider - 1;
		}
		SPI.setClockDivider(divider);
	}
#elif defined(__MK20DX128__) || defined(__MK20DX256__)

	void OLED_SSD1306::writeCommand(uint8_t c){
		SPI0.PUSHR = c | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
		while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	}

	void OLED_SSD1306::writeCommands(uint8_t *cmd, uint8_t length){
		for (uint8_t i = 0; i < length; i++) {
			SPI0.PUSHR = *cmd++ | (pcs_command << 16) | SPI_PUSHR_CTAS(0);
			while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
		}
	}
	
	void OLED_SSD1306::writeData(uint8_t c){
		SPI0.PUSHR = c | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
		while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
	}

	/*
	Helper:
	This function return true only if the choosed pin can be used for CS or RS
	*/
	static bool spi_pin_is_cs(uint8_t pin){
		if (pin == 2 || pin == 6 || pin == 9) return true;
		if (pin == 10 || pin == 15) return true;
		if (pin >= 20 && pin <= 23) return true;
		return false;
	}
	
	/*
	Helper:
	This function configure the correct pin
	*/
	static uint8_t spi_configure_cs_pin(uint8_t pin){
		switch (pin) {
			case 10: CORE_PIN10_CONFIG = PORT_PCR_MUX(2); return 0x01; // PTC4
			case 2:  CORE_PIN2_CONFIG  = PORT_PCR_MUX(2); return 0x01; // PTD0
			case 9:  CORE_PIN9_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTC3
			case 6:  CORE_PIN6_CONFIG  = PORT_PCR_MUX(2); return 0x02; // PTD4
			case 20: CORE_PIN20_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTD5
			case 23: CORE_PIN23_CONFIG = PORT_PCR_MUX(2); return 0x04; // PTC2
			case 21: CORE_PIN21_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTD6
			case 22: CORE_PIN22_CONFIG = PORT_PCR_MUX(2); return 0x08; // PTC1
			case 15: CORE_PIN15_CONFIG = PORT_PCR_MUX(2); return 0x10; // PTC0
		}
		return 0;
	}

	/*
	Helper:
	This function set the speed of the SPI interface
	*/
	void OLED_SSD1306::setBitrate(uint32_t n){
		if (n >= 24000000) {
			ctar = CTAR_24MHz;
		} else if (n >= 16000000) {
			ctar = CTAR_16MHz;
		} else if (n >= 12000000) {
			ctar = CTAR_12MHz;
		} else if (n >= 8000000) {
			ctar = CTAR_8MHz;
		} else if (n >= 6000000) {
			ctar = CTAR_6MHz;
		} else {
			ctar = CTAR_4MHz;
		}
		SIM_SCGC6 |= SIM_SCGC6_SPI0;
		SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
		SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
		SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
		SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
	}
#endif

void OLED_SSD1306::display(void) {
	uint8_t cmd[3];
	cmd[0] = _CMD_COLUMNADDR;
	cmd[1] = 0;
	cmd[2] = 127;
	writeCommands(cmd, 3);
	cmd[0] = _CMD_PAGEADDR;
	cmd[1] = 0;
	cmd[2] = (OLED_HEIGHT == 64) ? 7 : 3;
	writeCommands(cmd, 3);
    // SPI
	#ifdef __AVR__
		*csport |= cspinmask;//H
		*rsport |= rspinmask;//H
		*csport &= ~cspinmask;//L
		for (uint16_t i=0; i<(OLED_WIDTH*OLED_HEIGHT/8); i++) {
			spiwrite(buffer[i]);
		}
		*csport |= cspinmask;//H
	#elif defined(__SAM3X8E__)
		rsport->PIO_CODR |=  rspinmask;//H
		csport->PIO_CODR  |=  cspinmask;//H
		for (uint16_t i=0; i<(OLED_WIDTH*OLED_HEIGHT/8); i++) {
			spiwrite(buffer[i]);
		}
		csport->PIO_SODR  |=  cspinmask;//L
	#elif defined(__MK20DX128__) || defined(__MK20DX256__)		
		for (uint16_t i=0; i<(OLED_WIDTH*OLED_HEIGHT/8); i++) {
			SPI0.PUSHR = buffer[i] | (pcs_data << 16) | SPI_PUSHR_CTAS(0);
			while (((SPI0.SR) & (15 << 12)) > (3 << 12)) ; // wait if FIFO full
		}
	#endif
}


/*
----------------------------------------- HI SPEED SPI
*/

/*
Initialize PIN, direction and stuff related to hardware on CPU
*/
void OLED_SSD1306::commonInit(){
#if defined(__AVR__) 
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = portOutputRegister(digitalPinToPort(_cs));
	rsport    = portOutputRegister(digitalPinToPort(_rs));
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
    //Due defaults to 4mHz (clock divider setting of 21)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
	*csport &= ~cspinmask;
	_inited = true;
#elif defined(__SAM3X8E__) 
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = digitalPinToPort(_cs);
	rsport    = digitalPinToPort(_rs);
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
    SPI.setClockDivider(21); // 4 MHz
    //Due defaults to 4mHz (clock divider setting of 21), but we'll set it anyway 
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
	// toggle RST low to reset; CS low so it'll listen to us
	csport ->PIO_CODR  |=  cspinmask; // Set control bits to LOW (idle)
	_inited = true;
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
	_sid = 11;
	_sclk = 13;
	if (spi_pin_is_cs(_cs) && spi_pin_is_cs(_rs)
	 && (_sid == 7 || _sid == 11) && (_sclk == 13 || _sclk == 14)
	 && !(_cs ==  2 && _rs == 10) && !(_rs ==  2 && _cs == 10)
	 && !(_cs ==  6 && _rs ==  9) && !(_rs ==  6 && _cs ==  9)
	 && !(_cs == 20 && _rs == 23) && !(_rs == 20 && _cs == 23)
	 && !(_cs == 21 && _rs == 22) && !(_rs == 21 && _cs == 22)) {
		if (_sclk == 13) {
			CORE_PIN13_CONFIG = PORT_PCR_MUX(2) | PORT_PCR_DSE;
			SPCR.setSCK(13);
		} else {
			CORE_PIN14_CONFIG = PORT_PCR_MUX(2);
			SPCR.setSCK(14);
		}
		if (_sid == 11) {
			CORE_PIN11_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(11);
		} else {
			CORE_PIN7_CONFIG = PORT_PCR_MUX(2);
			SPCR.setMOSI(7);
		}
		ctar = CTAR_12MHz;
		pcs_data = spi_configure_cs_pin(_cs);
		pcs_command = pcs_data | spi_configure_cs_pin(_rs);
		SIM_SCGC6 |= SIM_SCGC6_SPI0;
		SPI0.MCR = SPI_MCR_MDIS | SPI_MCR_HALT;
		SPI0.CTAR0 = ctar | SPI_CTAR_FMSZ(7);
		SPI0.CTAR1 = ctar | SPI_CTAR_FMSZ(15);
		SPI0.MCR = SPI_MCR_MSTR | SPI_MCR_PCSIS(0x1F) | SPI_MCR_CLR_TXF | SPI_MCR_CLR_RXF;
		_inited = true;
	} else {
		_inited = false;
	}
#endif
	if (_rst && _inited) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}
}

/*
Here's the
*/
void OLED_SSD1306::chipInit(uint8_t switchvcc) {
	#if defined SSD1306_128_32
	writeCommand(_CMD_DISPLAYOFF);
	setRegister(_CMD_SETDISPLAYCLOCKDIV,0x80);
	setRegister(_CMD_SETMULTIPLEX,0x1F);
	setRegister(_CMD_SETDISPLAYOFFSET,0x00);
	writeCommand(_CMD_SETSTARTLINE | 0x00);
	if (switchvcc == _CMD_EXTERNALVCC){
		setRegister(_CMD_CHARGEPUMP,0x10);
	} else {
		setRegister(_CMD_CHARGEPUMP,0x14);
	}
	setRegister(_CMD_MEMORYMODE,0x00);
	writeCommand(_CMD_SEGREMAP | 0x01);
	writeCommand(_CMD_COMSCANDEC);
	setRegister(_CMD_SETCOMPINS,0x02);
	setRegister(_CMD_SETCONTRAST,0x8F);
	if (switchvcc == _CMD_EXTERNALVCC){
		setRegister(_CMD_SETPRECHARGE,0x22);
	} else {
		setRegister(_CMD_SETPRECHARGE,0xF1);
	}
	setRegister(_CMD_SETVCOMDETECT,0x40);
	writeCommand(_CMD_DISPLAYALLON_RESUME);
	writeCommand(_CMD_NORMALDISPLAY);
	#endif
	#if defined SSD1306_128_64
	writeCommand(_CMD_DISPLAYOFF);
	setRegister(_CMD_SETDISPLAYCLOCKDIV,0x80);
	setRegister(_CMD_SETMULTIPLEX,0x3F);
	setRegister(_CMD_SETDISPLAYOFFSET,0x00);
	writeCommand(_CMD_SETSTARTLINE | 0x00);
	if (switchvcc == _CMD_EXTERNALVCC){
		setRegister(_CMD_CHARGEPUMP,0x10);
	} else {
		setRegister(_CMD_CHARGEPUMP,0x14);
	}
	setRegister(_CMD_MEMORYMODE,0x00);
	writeCommand(_CMD_SEGREMAP | 0x01);
	writeCommand(_CMD_COMSCANDEC);
	setRegister(_CMD_SETCOMPINS,0x12);
	if (switchvcc == _CMD_EXTERNALVCC){
		setRegister(_CMD_SETCONTRAST,0x9F);
		setRegister(_CMD_SETPRECHARGE,0x22);
	} else {
		setRegister(_CMD_SETCONTRAST,0xCF);
		setRegister(_CMD_SETPRECHARGE,0xF1);
	}
	setRegister(_CMD_SETVCOMDETECT,0x40);
	writeCommand(_CMD_DISPLAYALLON_RESUME);
	writeCommand(_CMD_NORMALDISPLAY);
	#endif
	writeCommand(_CMD_DISPLAYON);
	clearDisplay();
}
