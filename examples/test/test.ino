#include <SPI.h>
#include <OLED_SSD1306.h>

//Uncomment this block to use hardware SPI
#define OLED_DC     21
#define OLED_CS     10
#define OLED_RESET  6
OLED_SSD1306 oled(OLED_CS, OLED_DC, OLED_RESET);


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{
  B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

boolean inited = false;

void setup()   {
  Serial.begin(38400);
  long unsigned debug_start = millis ();
  while (!Serial && ((millis () - debug_start) <= 5000)) ;
  Serial.println("start");
  //delay(1000);
  oled.begin();
  if (oled.getError() != 0) {
    Serial.println(oled.getError());
  } else {
    inited = true;
    Serial.println("Benchmarks\n");
  }
}

/*
testdrawline1:            25418 ms  ->  23055 ms
testdrawline2:            25422 ms  ->  23055 ms
testdrawline3:            25419 ms  ->  23061 ms
testdrawline4:            25508 ms  ->  23127 ms
testdrawrect:             7538 ms   ->  6895 ms
testfillrect:             6887 ms   ->  6626 ms
testdrawcircle:           20808 ms  ->  17671 ms
fillCircle:               514 ms    ->  479 ms
testdrawroundrect:        9216 ms   ->  8147 ms
testfillroundrect:        10104 ms  ->  9787 ms
testdrawtriangle:         3627 ms   ->  3340 ms
testfilltriangle:         3960 ms   ->  3426 ms
testdrawchar:             5333 ms   ->  3383 ms
testText:                 2176 ms   ->  1481 ms
*/


uint8_t rot = 0;
void loop() {
  if (inited) {
    oled.setRotation(rot);
    testPixel(500);
    testdrawline(250);
    testdrawrect(500);
    testfillrect(500);
    testdrawcircle(500);
    testCircle(500);
    testdrawroundrect(500);
    testfillroundrect(500);
    testdrawtriangle(500);
    testfilltriangle(500);
    testdrawchar(500);
    testscrolltext(1000);
    testText(500);
    bitMapTest(500);
    testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
    
    if (rot >= 3){
      rot = 0;
    } else {
      rot++;
    }
  }
}

void testPixel(int dly) {
  oled.clearDisplay();   // clears the screen and buffer
  oled.drawPixel(oled.width() / 2, oled.height() / 2, WHITE);
  oled.display();
  delay(dly);
}

void testCircle(int dly) {
  oled.clearDisplay();
  unsigned long start = micros();
  oled.fillCircle(oled.width() / 2, oled.height() / 2, 10, WHITE);
  oled.display();
  Serial.print("fillCircle: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}
void testText(int dly) {
  oled.clearDisplay();
  unsigned long start = micros();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.println("Hello, world!");
  oled.setTextColor(BLACK, WHITE); // 'inverted' text
  oled.println(3.141592);
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.print("0x");
  oled.println(0xDEADBEEF, HEX);
  oled.display();
  Serial.print("testText: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void bitMapTest(int dly) {
  oled.clearDisplay();
  oled.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  oled.display();
  // invert the display
  oled.invertDisplay(true);
  delay(dly);
  oled.invertDisplay(false);
  delay(dly);
}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  oled.clearDisplay();
  uint8_t icons[NUMFLAKES][3];
  uint8_t f;
  // initialize
  for (f = 0; f < NUMFLAKES; f++) {
    icons[f][XPOS] = random(oled.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
  }

  for (int i = 0; i < 1000; i++) {
    // draw each icon
    for (f = 0; f < NUMFLAKES; f++) {
      oled.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, WHITE);
    }
    oled.display();
    //delay(5);

    // then erase it + move it
    for (f = 0; f < NUMFLAKES; f++) {
      oled.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, BLACK);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > oled.height()) {
        icons[f][XPOS] = random(oled.width());
        icons[f][YPOS] = 0;
        icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}


void testdrawchar(int dly) {
  oled.clearDisplay();
  uint8_t i;
  unsigned long start = micros();
  oled.setTextSize(1);
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);

  for (i = 0; i < 168; i++) {
    if (i == '\n') continue;
    oled.write(i);
    if ((i > 0) && (i % 21 == 0))
      oled.println();
  }
  oled.display();
  Serial.print("testdrawchar: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testdrawcircle(int dly) {
  oled.clearDisplay();
  uint16_t i;
  unsigned long start = micros();
  for (i = 0; i < oled.height(); i += 2) {
    oled.drawCircle(oled.width() / 2, oled.height() / 2, i, WHITE);
    oled.display();
  }
  Serial.print("testdrawcircle: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testfillrect(int dly) {
  oled.clearDisplay();
  uint16_t color = WHITE;
  int16_t i;
  unsigned long start = micros();
  for (i = 0; i < oled.height() / 2; i += 3) {
    // alternate colors
    oled.fillRect(i, i, oled.width() - i * 2, oled.height() - i * 2, color % 2);
    oled.display();
    color++;
  }
  Serial.print("testfillrect: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testdrawtriangle(int dly) {
  oled.clearDisplay();
  int16_t i;
  unsigned long start = micros();
  for (i = 0; i < min(oled.width(), oled.height()) / 2; i += 5) {
    oled.drawTriangle(oled.width() / 2, oled.height() / 2 - i,
                      oled.width() / 2 - i, oled.height() / 2 + i,
                      oled.width() / 2 + i, oled.height() / 2 + i, WHITE);
    oled.display();
  }
  Serial.print("testdrawtriangle: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testfilltriangle(int dly) {
  oled.clearDisplay();
  uint16_t color = WHITE;
  int16_t i;
  unsigned long start = micros();
  for (i = min(oled.width(), oled.height()) / 2; i > 0; i -= 5) {
    oled.fillTriangle(oled.width() / 2, oled.height() / 2 - i,
                      oled.width() / 2 - i, oled.height() / 2 + i,
                      oled.width() / 2 + i, oled.height() / 2 + i, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oled.display();
  }
  Serial.print("testfilltriangle: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testdrawroundrect(int dly) {
  oled.clearDisplay();
  unsigned long start = micros();
  int16_t i;
  for (i = 0; i < oled.height() / 2 - 2; i += 2) {
    oled.drawRoundRect(i, i, oled.width() - 2 * i, oled.height() - 2 * i, oled.height() / 4, WHITE);
    oled.display();
  }
  Serial.print("testdrawroundrect: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testfillroundrect(int dly) {
  oled.clearDisplay();
  uint16_t color = WHITE;
  int16_t i;
  unsigned long start = micros();
  for (i = 0; i < oled.height() / 2 - 2; i += 2) {
    oled.fillRoundRect(i, i, oled.width() - 2 * i, oled.height() - 2 * i, oled.height() / 4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    oled.display();
  }
  Serial.print("testfillroundrect: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testdrawrect(int dly) {
  oled.clearDisplay();
  unsigned long start = micros();
  int16_t i;
  for (i = 0; i < oled.height() / 2; i += 2) {
    oled.drawRect(i, i, oled.width() - 2 * i, oled.height() - 2 * i, WHITE);
    oled.display();
  }
  Serial.print("testdrawrect: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testdrawline(int dly) {
  oled.clearDisplay();
  int16_t i;
  unsigned long start = micros();
  for (i = 0; i < oled.width(); i += 4) {
    oled.drawLine(0, 0, i, oled.height() - 1, WHITE);
    oled.display();
  }
  for (i = 0; i < oled.height(); i += 4) {
    oled.drawLine(0, 0, oled.width() - 1, i, WHITE);
    oled.display();
  }
  Serial.print("testdrawline1: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);

  oled.clearDisplay();
  start = micros();
  for (i = 0; i < oled.width(); i += 4) {
    oled.drawLine(0, oled.height() - 1, i, 0, WHITE);
    oled.display();
  }
  for (i = oled.height() - 1; i >= 0; i -= 4) {
    oled.drawLine(0, oled.height() - 1, oled.width() - 1, i, WHITE);
    oled.display();
  }
  Serial.print("testdrawline2: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);

  oled.clearDisplay();
  start = micros();
  for (i = oled.width() - 1; i >= 0; i -= 4) {
    oled.drawLine(oled.width() - 1, oled.height() - 1, i, 0, WHITE);
    oled.display();
  }
  for (i = oled.height() - 1; i >= 0; i -= 4) {
    oled.drawLine(oled.width() - 1, oled.height() - 1, 0, i, WHITE);
    oled.display();
  }
  Serial.print("testdrawline3: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);

  oled.clearDisplay();
  start = micros();
  for (i = 0; i < oled.height(); i += 4) {
    oled.drawLine(oled.width() - 1, 0, 0, i, WHITE);
    oled.display();
  }
  for (i = 0; i < oled.width(); i += 4) {
    oled.drawLine(oled.width() - 1, 0, i, oled.height() - 1, WHITE);
    oled.display();
  }
  Serial.print("testdrawline4: ");
  Serial.print(micros() - start);
  Serial.print(" ms\n");
  delay(dly);
}

void testscrolltext(int dly) {
  oled.display();
  oled.setTextSize(2);
  oled.setTextColor(WHITE);
  oled.setCursor(10, 0);
  oled.clearDisplay();
  oled.println("scroll");
  oled.display();

  oled.startScroll(HR, 0x00, 0x0F);
  delay(dly);
  oled.stopScroll();
  delay(dly);
  oled.startScroll(HL, 0x00, 0x0F);
  delay(dly);
  oled.stopScroll();
  delay(dly);
  oled.startScroll(VR, 0x00, 0x07);
  delay(dly);
  oled.startScroll(VL, 0x00, 0x07);
  delay(dly);
  oled.stopScroll();
}