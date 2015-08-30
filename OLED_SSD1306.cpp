#include "OLED_SSD1306.h"
#include "OLED_SSD1306_buffer.h"


extern "C" const unsigned char glcdfont[];
//Begin function
void OLED_SSD1306::begin(uint8_t switchvcc) 
{
	_initError = 0b00000000;
	setTextColor(WHITE);
	_width = OLED_WIDTH;
	_height = OLED_HEIGHT;
	_wrap = true;
	_textsize  = 1;
	_font      = NULL;
	_cursor_x = 0;
	_cursor_y = 0;
	setRotation(0);
	_vccstate = switchvcc;
	commonInit();
	if (_initError == 0) chipInit(_vccstate);
}

void OLED_SSD1306::invertDisplay(uint8_t i) 
{
	if (i) {
		writeCommand(_CMD_INVERTDISPLAY);
	} else {
		writeCommand(_CMD_NORMALDISPLAY);
	}
}

void OLED_SSD1306::startScroll(enum OLEDSSD1306SCRLLTYPE type,uint8_t start, uint8_t stop)
{
	byte tt;
	if (type == HR){
		tt = _CMD_RIGHT_HORIZONTAL_SCROLL;
	} else if (type == HL){	
		tt = _CMD_LEFT_HORIZONTAL_SCROLL;
	} else if (type == VR){	
		tt = _CMD_SET_VERTICAL_SCROLL_AREA;
	} else if (type == VL){	
		tt = _CMD_SET_VERTICAL_SCROLL_AREA;
	} else {
		return;
	}
	startTransaction();
		writecommand_cont(tt);
		writecommand_cont(0x00);
		if (tt == _CMD_SET_VERTICAL_SCROLL_AREA){
			writecommand_cont(OLED_HEIGHT);
			if (type == VL){
				writecommand_cont(_CMD_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
			} else {
				writecommand_cont(_CMD_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
			}
			writecommand_cont(0x00);
		}
		writecommand_cont(start);
		writecommand_cont(0x00);
		writecommand_cont(stop);
		if (tt == _CMD_SET_VERTICAL_SCROLL_AREA){
			writecommand_cont(0x01);
			writecommand_cont(0xFF);
		} else {
			writecommand_cont(0x00);
			writecommand_cont(0xFF);
		}
		writecommand_last(_CMD_ACTIVATE_SCROLL);
	endTransaction();
}

void OLED_SSD1306::stopScroll(void)
{
	writeCommand(_CMD_DEACTIVATE_SCROLL);
}

void OLED_SSD1306::dim(boolean dim) 
{
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

void OLED_SSD1306::clearDisplay(void) 
{
	memset(buffer, 0x00, OLED_BUFFERLEN);
}

void OLED_SSD1306::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
	switch(_rotation) { 
		case 0:// 0 degree rotation, do nothing
			break;
		case 1:// 90 degree rotation, swap x & y for rotation, then invert x
			swap(x, y);
			x = OLED_MW - x;
			break;
		case 2:// 180 degree rotation, invert x and y - then shift y around for height.
			x = OLED_MW - x;
			y = OLED_MH - y;
			x -= (w - 1);
			break;
		case 3:// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
			swap(x, y);
			y = OLED_MH - y;
			y -= (w - 1);
			break;
	}
	if (_portrait) { 
		drawFastVLineInternal(x, y, w, color);
	} else { 
		drawFastHLineInternal(x, y, w, color);
	}
}

void OLED_SSD1306::drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
	if (y < 0 || y >= OLED_HEIGHT) return;// Do bounds/limit checks
	if (x < 0) { w += x; x = 0; }// make sure we don't try to draw below 0
	if ((x + w) > OLED_WIDTH) w = (OLED_HEIGHT - x);// make sure we don't go off the edge of the display
	if (w <= 0) return;// if our width is now negative, punt
	if (color > 0) color = WHITE;
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

void OLED_SSD1306::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
	switch(_rotation) { 
		case 0:
			break;
		case 1:
			swap(x, y);
			x = OLED_MW - x;
			x -= (h - 1);
			break;
		case 2:
			x = OLED_MW - x;
			y = OLED_MH - y;
			y -= (h - 1);
			break;
		case 3:
			swap(x, y);
			y = OLED_MH - y;
			break;
	}
	if (_portrait) { 
		drawFastHLineInternal(x, y, h, color);
	} else {
		drawFastVLineInternal(x, y, h, color);
	}
}

void OLED_SSD1306::drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
	if (x < 0 || x >= OLED_WIDTH) return;// do nothing if we're off the left or right side of the screen
	// make sure we don't try to draw below 0
	// y is negative, this will subtract enough from h to account for y being 0
	if (y < 0) { h += y; y = 0; } 
	if ((y + h) > OLED_HEIGHT)  h = (OLED_HEIGHT - y);// make sure we don't go past the height of the display
	if (h <= 0) return;// if our height is now negative, punt 
	if (color > 0) color = WHITE;
	// this display doesn't need ints for coordinates, use local byte registers for faster juggling
	register uint8_t yn = y;
	register uint8_t hn = h;
	// set up the pointer for fast movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += ((yn/8) * OLED_WIDTH);
	// and offset x columns in
	pBuf += x;
	// do the first partial byte, if necessary - this requires some masking
	register uint8_t mod = (yn&7);
	if (mod) {
		// mask off the high n bits we want to set 
		mod = 8 - mod;
		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		// register uint8_t mask = ~(0xFF >> (mod));
		static const uint8_t premask[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE };
		register uint8_t mask = premask[mod];
		// adjust the mask if we're not going to reach the end of this byte
		if (hn < mod) mask &= (0xFF >> (mod - hn));
		if (color == WHITE) { 
			*pBuf |= mask;
		} else {
			*pBuf &= ~mask;
		}
		// fast exit if we're done here!
		if (hn < mod) return;
		hn -= mod;
		pBuf += OLED_WIDTH;
	}
	// write solid bytes while we can - effectively doing 8 rows at a time
	if (hn >= 8) { 
		// store a local value to work with 
		register uint8_t val = (color == WHITE) ? 255 : 0;
		do  {
			// write our value in
			*pBuf = val;
			// adjust the buffer forward 8 rows worth of data
			pBuf += OLED_WIDTH;
			// adjust h & y (there's got to be a faster way for me to do this, but this should still help a fair bit for now)
			hn -= 8;
		} while (hn >= 8);
	}	
	// now do the final partial byte, if necessary
	if (hn) {
		mod = hn & 7;
		// this time we want to mask the low bits of the byte, vs the high bits we did above
		// register uint8_t mask = (1 << mod) - 1;
		// note - lookup table results in a nearly 10% performance improvement in fill* functions
		static const uint8_t postmask[8] = {0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };
		register uint8_t mask = postmask[mod];
		if (color == WHITE) { 
			*pBuf |= mask;
		} else { 
			*pBuf &= ~mask;
		}
	}
}
  
void OLED_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
	if (color > 0) color = WHITE;
	// check rotation, move pixel around if necessary
	switch (_rotation) {
		case 1:
			swap(x, y);
			x = OLED_MW - x;
			break;
		case 2:
			x = OLED_MW - x;
			y = OLED_MH - y;
			break;
		case 3:
			swap(x, y);
			y = OLED_MH - y;
			break;
	}
	if (color == WHITE) {
		buffer[x + (y / 8) * OLED_WIDTH] |= (1 << (y & 7));  
	} else {
		buffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y & 7)); 
	}
}


#if defined(__MK20DX128__) || defined(__MK20DX256__)
	OLED_SSD1306::OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin,uint8_t mosi,uint8_t sclk)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
		_mosi = mosi;
		_sclk = sclk;
	}
#elif defined(__MKL26Z64__)
	OLED_SSD1306::OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin,uint8_t mosi,uint8_t sclk)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
		_mosi = mosi;
		_sclk = sclk;
		_useSPI1 = false;
		if ((_mosi == 0 || _mosi == 21) && (_sclk == 20)) _useSPI1 = true;
	}
#else
	OLED_SSD1306::OLED_SSD1306(uint8_t cspin,uint8_t dcpin,uint8_t rstpin)
	{
		_cs   = cspin;
		_rs   = dcpin;
		_rst  = rstpin;
	}
#endif

#if defined(SPI_HAS_TRANSACTION)
	void OLED_SSD1306::setBitrate(uint32_t n)
	{
	}
#else
	#ifdef __AVR__

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

		void OLED_SSD1306::setBitrate(uint32_t n){
			uint32_t divider=1;
			while (divider < 255) {
				if (n >= 84000000 / divider) break;
				divider = divider - 1;
			}
			SPI.setClockDivider(divider);
		}

	#elif defined(__MK20DX128__) || defined(__MK20DX256__) || defined(__MKL26Z64__)
		void OLED_SSD1306::setBitrate(uint32_t n)
		{

		}
	#else
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
	#endif
#endif

void OLED_SSD1306::display(void) 
{
	uint16_t i;
	
	startTransaction();
	
	setAdrsCont(0x00,OLED_MW,0x00,OLED_PGADRSVAL);
	
	for (i=0; i < OLED_BUFFERLEN; i++) {
		if (i < OLED_BUFFERLEN-1){
			writedata_cont(buffer[i]);
		}
	}
	writecommand_last(_CMD_NOP);
	
	endTransaction();
}


uint8_t OLED_SSD1306::getError()
{
	return _initError;
}

void OLED_SSD1306::commonInit()
{
	
#if defined(__AVR__) 
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = portOutputRegister(digitalPinToPort(_cs));
	rsport    = portOutputRegister(digitalPinToPort(_rs));
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
	#if !defined(SPI_HAS_TRANSACTION)
		SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
	#else
		SSD1306_SPI = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	#endif
	*rsport &= ~rspinmask;//LOW
	_DC_state = LOW;
	*csport |= cspinmask;//HI
#elif defined(__SAM3X8E__) 
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = digitalPinToPort(_cs);
	rsport    = digitalPinToPort(_rs);
	cspinmask = digitalPinToBitMask(_cs);
	rspinmask = digitalPinToBitMask(_rs);
    SPI.begin();
	#if !defined(SPI_HAS_TRANSACTION)
		SPI.setClockDivider(21); // 4 MHz
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
	#else
		SSD1306_SPI = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	#endif
	rsport->PIO_CODR |=  rspinmask;//LOW
	csport->PIO_SODR  |=  cspinmask;//HI
	_DC_state = LOW;
#elif defined(__MK20DX128__) || defined(__MK20DX256__)
	#if defined(SPI_HAS_TRANSACTION)
		SSD1306_SPI = SPISettings(30000000, MSBFIRST, SPI_MODE0);
	#endif
	if ((_mosi == 11 || _mosi == 7) && (_sclk == 13 || _sclk == 14)) {
        SPI.setMOSI(_mosi);
        SPI.setSCK(_sclk);
	} else {
		 bitSet(_initError,0);
		 return;
	}
	SPI.begin();
	if (SPI.pinIsChipSelect(_cs, _rs)) {
		pcs_data = SPI.setCS(_cs);
		pcs_command = pcs_data | SPI.setCS(_rs);
	} else {
		pcs_data = 0;
		pcs_command = 0;
		bitSet(_initError,1);
		return;
	}
#elif defined(__MKL26Z64__)
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);

	SPI.begin();
	#if defined(SPI_HAS_TRANSACTION)
		SSD1306_SPI = SPISettings(30000000, MSBFIRST, SPI_MODE0);
	#endif
	digitalWriteFast(_cs,HIGH);
	digitalWriteFast(_rs,LOW);
	_DC_state = LOW;
#else
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);

	SPI.begin();
	#if !defined(SPI_HAS_TRANSACTION)
		SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz
		SPI.setBitOrder(MSBFIRST);
		SPI.setDataMode(SPI_MODE0);
	#else
		SSD1306_SPI = SPISettings(8000000, MSBFIRST, SPI_MODE0);
	#endif
	digitalWrite(_cs,HIGH);
	digitalWrite(_rs,LOW);
	_DC_state = LOW;
#endif
	if (_rst && (_initError == 0)) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}
}

void OLED_SSD1306::chipInit(uint8_t switchvcc) 
{
	writeCommand(_CMD_DISPLAYOFF);
	setRegister(_CMD_SETDISPLAYCLOCKDIV,0x80);
	#if defined SSD1306_128_32
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
	#endif
	#if defined SSD1306_128_64
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

	#endif
	setRegister(_CMD_SETVCOMDETECT,0x40);
	writeCommand(_CMD_DISPLAYALLON_RESUME);
	writeCommand(_CMD_NORMALDISPLAY);
	writeCommand(_CMD_DISPLAYON);
	clearDisplay();
	display();
}

void OLED_SSD1306::drawLine(int16_t x0, int16_t y0,int16_t x1, int16_t y1,uint16_t color) 
{
	if (y0 == y1) {
		if (x1 > x0) {
			drawFastHLine(x0, y0, x1 - x0 + 1, color);
		} else if (x1 < x0) {
			drawFastHLine(x1, y0, x0 - x1 + 1, color);
		} else {
			drawPixel(x0, y0, color);
		}
		return;
	} else if (x0 == x1) {
		if (y1 > y0) {
			drawFastVLine(x0, y0, y1 - y0 + 1, color);
		} else {
			drawFastVLine(x0, y1, y0 - y1 + 1, color);
		}
		return;
	}

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {swap(x0, y0);swap(x1, y1);}
	if (x0 > x1) {swap(x0, x1);swap(y0, y1);}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}


	int16_t xbegin = x0;
	if (steep) {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastVLine(y0, xbegin, len + 1, color);
				} else {
					drawPixel(y0, x0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) drawFastVLine(y0, xbegin, x0 - xbegin, color);
	} else {
		for (; x0<=x1; x0++) {
			err -= dy;
			if (err < 0) {
				int16_t len = x0 - xbegin;
				if (len) {
					drawFastHLine(xbegin, y0, len + 1, color);
				} else {
					drawPixel(x0, y0, color);
				}
				xbegin = x0 + 1;
				y0 += ystep;
				err += dx;
			}
		}
		if (x0 > xbegin + 1) drawFastHLine(xbegin, y0, x0 - xbegin, color);
	}
}

void OLED_SSD1306::drawRect(int16_t x,int16_t y,int16_t w,int16_t h,uint16_t color) 
{
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y+h-1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w-1, y, h, color);
}

void OLED_SSD1306::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color) 
{
  // Update in subclasses if desired!
	for (int16_t i=x; i<x+w; i++) {
		drawFastVLine(i, y, h, color);
	}
}

void OLED_SSD1306::fillScreen(uint16_t color) 
{
	/* fillRect(0, 0, _width, _height, color); */
	uint8_t _color = 0x00;
	if (color != 0) _color = 0xFF;
	memset(buffer, _color, OLED_BUFFERLEN);
	display();
}

void OLED_SSD1306::drawCircle(int16_t cx, int16_t cy, int16_t radius, uint16_t color)
{
	int error = -radius;
	int16_t x = radius;
	int16_t y = 0;
	while (x >= y){
		plot8points(cx, cy, x, y, color);
		error += y;
		++y;
		error += y;
		if (error >= 0){
			--x;
			error -= x;
			error -= x;
		}
	}
}

void OLED_SSD1306::plot8points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color)
{
	plot4points(cx, cy, x, y, color);
	if (x != y) plot4points(cx, cy, y, x, color);
}

void OLED_SSD1306::plot4points(uint16_t cx, uint16_t cy, uint16_t x, uint16_t y, uint16_t color)
{
	drawPixel(cx + x, cy + y, color);
	if (x != 0) drawPixel(cx - x, cy + y, color);
	if (y != 0) drawPixel(cx + x, cy - y, color);
	if (x != 0 && y != 0) drawPixel(cx - x, cy - y, color);
}

void OLED_SSD1306::drawCircleHelper(int16_t x0,int16_t y0,int16_t r,uint8_t cornername,uint16_t color) 
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x4) {drawPixel(x0 + x, y0 + y, color); drawPixel(x0 + y, y0 + x, color);} 
		if (cornername & 0x2) {drawPixel(x0 + x, y0 - y, color);drawPixel(x0 + y, y0 - x, color);}
		if (cornername & 0x8) {drawPixel(x0 - y, y0 + x, color);drawPixel(x0 - x, y0 + y, color);}
		if (cornername & 0x1) {drawPixel(x0 - y, y0 - x, color);drawPixel(x0 - x, y0 - y, color);}
	}
}

void OLED_SSD1306::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) 
{
	int16_t f     = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x     = 0;
	int16_t y     = r;

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f     += ddF_y;
		}
		x++;
		ddF_x += 2;
		f     += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
			drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
		}
	}
}

void OLED_SSD1306::ellipse(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t a = abs(x1 - x0), b = abs(y1 - y0), b1 = b & 1; /* values of diameter */
	long dx = 4 * (1 - a) * b * b, dy = 4 * (b1 + 1) * a * a; /* error increment */
	long err = dx + dy + b1 * a * a, e2; /* error of 1.step */

	if (x0 > x1) { x0 = x1; x1 += a; } /* if called with swapped points */
	if (y0 > y1) y0 = y1; /* .. exchange them */
	y0 += (b + 1) / 2; /* starting pixel */
	y1 = y0 - b1;
	a *= 8 * a;
	b1 = 8 * b * b;

	do {
		drawPixel(x1, y0, color); /*   I. Quadrant */
		drawPixel(x0, y0, color); /*  II. Quadrant */
		drawPixel(x0, y1, color); /* III. Quadrant */
		drawPixel(x1, y1, color); /*  IV. Quadrant */
		e2 = 2 * err;
		if (e2 >= dx) { x0++; x1--; err += dx += b1; } /* x step */
		if (e2 <= dy) { y0++; y1--; err += dy += a; }  /* y step */
	} while (x0 <= x1);

	while (y0 - y1 < b) {  /* too early stop of flat ellipses a=1 */
		drawPixel(x0 - 1, ++y0, color); /* -> complete tip of ellipse */
		drawPixel(x0 - 1, --y1, color);
	}
}

void OLED_SSD1306::fillCircle(int16_t x0, int16_t y0, int16_t r,uint16_t color) 
{
	drawFastVLine(x0, y0-r, 2*r+1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
}

void OLED_SSD1306::drawTriangle(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}


void OLED_SSD1306::fillTriangle ( int16_t x0, int16_t y0,
				  int16_t x1, int16_t y1,
				  int16_t x2, int16_t y2, uint16_t color) {

  int16_t a, b, y, last;

  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }
  if (y1 > y2) {
    swap(y2, y1); swap(x2, x1);
  }
  if (y0 > y1) {
    swap(y0, y1); swap(x0, x1);
  }

  if(y0 == y2) { // Handle awkward all-on-same-line case as its own thing
    a = b = x0;
    if(x1 < a)      a = x1;
    else if(x1 > b) b = x1;
    if(x2 < a)      a = x2;
    else if(x2 > b) b = x2;
    drawFastHLine(a, y0, b-a+1, color);
    return;
  }

  int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1,
    sa   = 0,
    sb   = 0;

  if(y1 == y2) last = y1;   // Include y1 scanline
  else         last = y1-1; // Skip it

  for(y=y0; y<=last; y++) {
    a   = x0 + sa / dy01;
    b   = x0 + sb / dy02;
    sa += dx01;
    sb += dx02;

    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }


  sa = dx12 * (y - y1);
  sb = dx02 * (y - y0);
  for(; y<=y2; y++) {
    a   = x1 + sa / dy12;
    b   = x0 + sb / dy02;
    sa += dx12;
    sb += dx02;

    if(a > b) swap(a,b);
    drawFastHLine(a, y, b-a+1, color);
  }
}



void OLED_SSD1306::drawQuad(int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2,int16_t x3, int16_t y3, uint16_t color) 
{
	drawLine(x0, y0, x1, y1, color);//low 1
	drawLine(x1, y1, x2, y2, color);//high 1
	drawLine(x2, y2, x3, y3, color);//high 2
	drawLine(x3, y3, x0, y0, color);//low 2
}

void OLED_SSD1306::fillQuad ( int16_t x0, int16_t y0,int16_t x1, int16_t y1,int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t color) 
{
    fillTriangle(x0,y0,x1,y1,x2,y2,color);
    fillTriangle(x0,y0,x2,y2,x3,y3,color);
}

void OLED_SSD1306::drawPolygon(int16_t cx, int16_t cy, uint8_t sides, int16_t diameter, float rot, uint16_t color)
{ 
	sides = (sides > 2? sides : 3);
	float dtr = (PI/180.0) + PI;
	float rads = 360.0 / sides;//points spacd equally
	uint8_t i;
	for (i = 0; i < sides; i++) { 
		drawLine(
			cx + (sin((i*rads + rot) * dtr) * diameter),
			cy + (cos((i*rads + rot) * dtr) * diameter),
			cx + (sin(((i+1)*rads + rot) * dtr) * diameter),
			cy + (cos(((i+1)*rads + rot) * dtr) * diameter),
			color);
	}
}

void OLED_SSD1306::drawRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) 
{
  // smarter version
	drawFastHLine(x+r  , y    , w-2*r, color); // Top
	drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
	drawFastVLine(x    , y+r  , h-2*r, color); // Left
	drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
  // draw four corners
	drawCircleHelper(x+r    , y+r    , r, 1, color);
	drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
	drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
	drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

void OLED_SSD1306::fillRoundRect(int16_t x, int16_t y, int16_t w,int16_t h, int16_t r, uint16_t color) 
{
	// smarter version
	fillRect(x+r, y, w-2*r, h, color);
	// draw four corners
	fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
	fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

void OLED_SSD1306::drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color) 
{
	int16_t i, j, byteWidth = (w + 7) / 8;
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++ ) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) drawPixel(x+i, y+j, color);
		}
	}
}

void OLED_SSD1306::drawBitmap(int16_t x, int16_t y,const uint8_t *bitmap, int16_t w, int16_t h,uint16_t color, uint16_t bg) 
{
	int16_t i, j, byteWidth = (w + 7) / 8;
	for (j=0; j<h; j++) {
		for (i=0; i<w; i++ ) {
			if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x+i, y+j, color);
			} else {
				drawPixel(x+i, y+j, bg);
			}
		}
	}
}

void OLED_SSD1306::setCursor(int16_t x, int16_t y) 
{
	_cursor_x = x;
	_cursor_y = y;
}

void OLED_SSD1306::setTextColor(uint16_t c) 
{
	// For 'transparent' background, we'll set the bg 
	// to the same as fg instead of using a flag
	if (c != 0) c = WHITE;
	_textcolor = _textbgcolor = c;
}

void OLED_SSD1306::setTextColor(uint16_t c, uint16_t b) 
{
	if (c != 0) c = WHITE;
	if (b != 0) b = WHITE;
	_textcolor   = c;
	_textbgcolor = b; 
}

void OLED_SSD1306::setTextSize(uint8_t s) 
{
	_textsize = (s > 0) ? s : 1;
}

void OLED_SSD1306::setTextWrap(boolean w) 
{
	_wrap = w;
}

void OLED_SSD1306::setRotation(uint8_t x) 
{
	_rotation = (x & 3);
	switch(_rotation) {
		case 0:
		case 2:
			_width  = OLED_WIDTH;
			_height = OLED_HEIGHT;
			_portrait = false;
			break;
		case 1:
		case 3:
			_width  = OLED_HEIGHT;
			_height = OLED_WIDTH;
			_portrait = true;
			break;
	}
}

int16_t OLED_SSD1306::width(void)
{
	return _width;
}
 
int16_t OLED_SSD1306::height(void)
{
	return _height;
}

void OLED_SSD1306::drawMesh(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t spacing, uint16_t color)
{
	if (spacing < 2) spacing = 2;
	if (((x + w) - 1) >= _width)  w = _width  - x;
	if (((y + h) - 1) >= _height) h = _height - y;
	
	int16_t n, m;

	if (w < x) {n = w; w = x; x = n;}
	if (h < y) {n = h; h = y; y = n;}
	for (m = y; m <= h; m += spacing) {
		for (n = x; n <= w; n += spacing) {
			drawPixel(n, m, color);
		}
	}
}

static uint32_t fetchbit(const uint8_t *p, uint32_t index)
{
	if (p[index >> 3] & (1 << (7 - (index & 7)))) return 1;
	return 0;
}

static uint32_t fetchbits_unsigned(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = 0;
	do {
		uint8_t b = p[index >> 3];
		uint32_t avail = 8 - (index & 7);
		if (avail <= required) {
			val <<= avail;
			val |= b & ((1 << avail) - 1);
			index += avail;
			required -= avail;
		} else {
			b >>= avail - required;
			val <<= required;
			val |= b & ((1 << required) - 1);
			break;
		}
	} while (required);
	return val;
}

static uint32_t fetchbits_signed(const uint8_t *p, uint32_t index, uint32_t required)
{
	uint32_t val = fetchbits_unsigned(p, index, required);
	if (val & (1 << (required - 1))) return (int32_t)val - (1 << required);
	return (int32_t)val;
}

size_t OLED_SSD1306::write(uint8_t c)
{
	if (_font) {
		if (c == '\n') {
			//cursor_y += ??
			_cursor_x = 0;
		} else {
			drawFontChar(c);
		}
	} else {
		if (c == '\n') {
			_cursor_y += _textsize * 8;
			_cursor_x  = 0;
		} else if (c == '\r') {
			// skip em
		} else {
			drawChar(_cursor_x, _cursor_y, c, _textcolor, _textbgcolor, _textsize);
			_cursor_x += _textsize * 6;
			if (_wrap && (_cursor_x > (_width - _textsize * 6))) {
				_cursor_y += _textsize * 8;
				_cursor_x = 0;
			}
		}
	}
	return 1;
}

void OLED_SSD1306::drawFontChar(unsigned int c)
{
	uint32_t bitoffset;
	const uint8_t *data;

	if (c >= _font->index1_first && c <= _font->index1_last) {
		bitoffset = c - _font->index1_first;
		bitoffset *= _font->bits_index;
	} else if (c >= _font->index2_first && c <= _font->index2_last) {
		bitoffset = c - _font->index2_first + _font->index1_last - _font->index1_first + 1;
		bitoffset *= _font->bits_index;
	} else if (_font->unicode) {
		return; // TODO: implement sparse unicode
	} else {
		return;
	}
	data = _font->data + fetchbits_unsigned(_font->index, bitoffset, _font->bits_index);

	uint32_t encoding = fetchbits_unsigned(data, 0, 3);
	if (encoding != 0) return;
	uint32_t width = fetchbits_unsigned(data, 3, _font->bits_width);
	bitoffset = _font->bits_width + 3;
	uint32_t height = fetchbits_unsigned(data, bitoffset, _font->bits_height);
	bitoffset += _font->bits_height;
	int32_t xoffset = fetchbits_signed(data, bitoffset, _font->bits_xoffset);
	bitoffset += _font->bits_xoffset;
	int32_t yoffset = fetchbits_signed(data, bitoffset, _font->bits_yoffset);
	bitoffset += _font->bits_yoffset;
	uint32_t delta = fetchbits_unsigned(data, bitoffset, _font->bits_delta);
	bitoffset += _font->bits_delta;
	// horizontally, we draw every pixel, or none at all
	if (_cursor_x < 0) _cursor_x = 0;
	int32_t origin_x = _cursor_x + xoffset;
	if (origin_x < 0) {
		_cursor_x -= xoffset;
		origin_x = 0;
	}
	if (origin_x + (int)width > _width) {
		if (!_wrap) return;
		origin_x = 0;
		if (xoffset >= 0) {
			_cursor_x = 0;
		} else {
			_cursor_x = -xoffset;
		}
		_cursor_y += _font->line_space;
	}
	if (_cursor_y >= _height) return;
	_cursor_x += delta;

	// vertically, the top and/or bottom can be clipped
	int32_t origin_y = _cursor_y + _font->cap_height - height - yoffset;
	// TODO: compute top skip and number of lines
	int32_t linecount = height;
	uint32_t y = origin_y;
	while (linecount) {
		uint32_t b = fetchbit(data, bitoffset++);
		if (b == 0) {
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, 1);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y++;
			linecount--;
		} else {
			uint32_t n = fetchbits_unsigned(data, bitoffset, 3) + 2;
			bitoffset += 3;
			uint32_t x = 0;
			do {
				uint32_t xsize = width - x;
				if (xsize > 32) xsize = 32;
				uint32_t bits = fetchbits_unsigned(data, bitoffset, xsize);
				drawFontBits(bits, xsize, origin_x + x, y, n);
				bitoffset += xsize;
				x += xsize;
			} while (x < width);
			y += n;
			linecount -= n;
		}
	}
}

void OLED_SSD1306::drawFontBits(uint32_t bits, uint32_t numbits, uint32_t x, uint32_t y, uint32_t repeat)
{
	// TODO: replace this *slow* code with something fast...
	if (bits == 0) return;
	do {
		uint32_t x1 = x;
		uint32_t n = numbits;
		do {
			n--;
			if (bits & (1 << n)) drawPixel(x1, y, _textcolor);
			x1++;
		} while (n > 0);
		y++;
		repeat--;
	} while (repeat);
}

void OLED_SSD1306::drawChar(int16_t x, int16_t y, unsigned char c,
			    uint16_t fgcolor, uint16_t bgcolor, uint8_t size)
{
	if ((x >= _width)            || // Clip right
	   (y >= _height)            || // Clip bottom
	   ((x + 6 * size - 1) < 0)  || // Clip left  TODO: is this correct?
	   ((y + 8 * size - 1) < 0))   // Clip top   TODO: is this correct?
		return;

	if (fgcolor == bgcolor) {
		// This transparent approach is only about 20% faster
		if (size == 1) {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						drawFastHLine(x + xoff, y + yoff, 5, fgcolor);
						break;
					} else if (line == 0x1E) {
						drawFastHLine(x + xoff, y + yoff, 4, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						drawFastHLine(x + xoff, y + yoff, 3, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						drawFastHLine(x + xoff, y + yoff, 2, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						drawPixel(x + xoff, y + yoff, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		} else {
			uint8_t mask = 0x01;
			int16_t xoff, yoff;
			for (yoff=0; yoff < 8; yoff++) {
				uint8_t line = 0;
				for (xoff=0; xoff < 5; xoff++) {
					if (glcdfont[c * 5 + xoff] & mask) line |= 1;
					line <<= 1;
				}
				line >>= 1;
				xoff = 0;
				while (line) {
					if (line == 0x1F) {
						fillRect(x + xoff * size, y + yoff * size,
							5 * size, size, fgcolor);
						break;
					} else if (line == 0x1E) {
						fillRect(x + xoff * size, y + yoff * size,
							4 * size, size, fgcolor);
						break;
					} else if ((line & 0x1C) == 0x1C) {
						fillRect(x + xoff * size, y + yoff * size,
							3 * size, size, fgcolor);
						line <<= 4;
						xoff += 4;
					} else if ((line & 0x18) == 0x18) {
						fillRect(x + xoff * size, y + yoff * size,
							2 * size, size, fgcolor);
						line <<= 3;
						xoff += 3;
					} else if ((line & 0x10) == 0x10) {
						fillRect(x + xoff * size, y + yoff * size,
							size, size, fgcolor);
						line <<= 2;
						xoff += 2;
					} else {
						line <<= 1;
						xoff += 1;
					}
				}
				mask = mask << 1;
			}
		}
	} else {
		// This solid background approach is about 5 time faster
		/*
		SPI.beginTransaction(SPISettings(SPICLOCK, MSBFIRST, SPI_MODE0));
		setAddr(x, y, x + 6 * size - 1, y + 8 * size - 1);
		writecommand_cont(ILI9341_RAMWR);
		uint8_t xr, yr;
		uint8_t mask = 0x01;
		uint16_t color;
		for (y=0; y < 8; y++) {
			for (yr=0; yr < size; yr++) {
				for (x=0; x < 5; x++) {
					if (glcdfont[c * 5 + x] & mask) {
						color = fgcolor;
					} else {
						color = bgcolor;
					}
					for (xr=0; xr < size; xr++) {
						writedata16_cont(color);
					}
				}
				for (xr=0; xr < size; xr++) {
					writedata16_cont(bgcolor);
				}
			}
			mask = mask << 1;
		}
		writecommand_last(ILI9341_NOP);
		SPI.endTransaction();
		*/
	}
}

//via guglielmo oberdan cerea