/*
driver:             SSD1322     SPIx4, SPIx3, I2C, 80xx, 68xx
resolution:         256 x 64  white

aliexpress link     https://www.aliexpress.com/item/1005005985371717.html

description         DIYTZT Real OLED Display 3.12" 256*64 25664 Dots Graphic LCD Module Display Screen LCM Screen SSD1322 Controller Support SPI
cost                £11.79
*/

/*
  Portions of code adapted from examples in this library:

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

#include <UMS3.h>
UMS3 UM;

#include <U8g2lib.h>

#define OLED_VCC_3V3_RED   "ABOVE DIN_MOSI_BLUE"
#define OLED_GND_BLACK     "BELOW BATTERY PIN (TOP RIGHT NEXT TO USB-C)"
#define OLED_RST_BROWN              2
#define OLED_DC_PURPLE              1
#define OLED_CS_ORANGE              34  // Standard Arduino Hardware SPI Chip-Select Pro S3
#define OLED_CLK_SCL_YELLOW         36  // Standard Arduino Hardware SPI CLK for Pro S3
#define OLED_DIN_MOSI_SDA_BLUE      35  // Standard Arduino Hardware SPI MOSI for Pro S3

/*
D0 RST BROWN  -->  logic blue    1
D1 DC PURPLE  -->  logic green   2
D2 CS ORANGE  -->  logic grey    34
D3 CLK YELLOW -->  logic purple  36
D4 MOSI BLUE  -->  logic white   35
*/

// This HW SPI code default to the standard arduino pins defined for Pro S3.
// which are CLK on OLED_CLK_SCL_YELLOW and OLED_DIN_MOSI_SDA_BLUE on 35.
// This ensures there is no SW SPI involved.
U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R0,
                                            OLED_CS_ORANGE,
                                            OLED_DC_PURPLE, 
                                            OLED_RST_BROWN);

// ################# START FONTUSAGE TEST ##############
#define INFO_SCREEN_DELAY 3000
void draw(int is_blank);
void draw_m1_t();
void draw_m0_t();
void draw_m1_h();
void draw_m0_h();
void draw_m0_h_with_extra_blank();

/*
  Linear Congruential Generator (LCG)
  z = (a*z + c) % m;  
  m = 256 (8 Bit)
  
  for period:
  a-1: dividable by 2
  a-1: multiple of 4
  c: not dividable by 2
  
  c = 17
  a-1 = 64 --> a = 65
*/
uint8_t z = 127;	// start value
uint32_t lcg_rnd(void) {
  z = (uint8_t)((uint16_t)65*(uint16_t)z + (uint16_t)17);
  return (uint32_t)z;
}
// ################# END FONTUSAGE TEST ##############

// ################# START FPS TEST ##############

typedef u8g2_uint_t u8g_uint_t;

#define SECONDS 10
uint8_t flip_color = 0;
uint8_t draw_color = 1;

void draw_set_screen(void);
void draw_clip_test(void);
void draw_char(void);
void draw_pixel(void);
void draw_line(void);
uint16_t execute_with_fps(void (*draw_fn)(void));
const char *convert_FPS(uint16_t fps);
void show_result(const char *s, uint16_t fps);

// ################# END FPS TEST ##############

// ######### start graphics test 



void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void u8g2_box_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawBox");
  u8g2.drawBox(5,10,20,10);
  u8g2.drawBox(10+a,15,30,7);
  u8g2.drawStr( 0, 30, "drawFrame");
  u8g2.drawFrame(5,10+30,20,10);
  u8g2.drawFrame(10+a,15+30,30,7);
}

void u8g2_disc_circle(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawDisc");
  u8g2.drawDisc(10,18,9);
  u8g2.drawDisc(24+a,16,7);
  u8g2.drawStr( 0, 30, "drawCircle");
  u8g2.drawCircle(10,18+30,9);
  u8g2.drawCircle(24+a,16+30,7);
}

void u8g2_r_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawRFrame/Box");
  u8g2.drawRFrame(5, 10,40,30, a+1);
  u8g2.drawRBox(50, 10,25,40, a+1);
}

void u8g2_string(uint8_t a) {
  u8g2.setFontDirection(0);
  u8g2.drawStr(30+a,31, " 0");
  u8g2.setFontDirection(1);
  u8g2.drawStr(30,31+a, " 90");
  u8g2.setFontDirection(2);
  u8g2.drawStr(30-a,31, " 180");
  u8g2.setFontDirection(3);
  u8g2.drawStr(30,31-a, " 270");
}

void u8g2_line(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawLine");
  u8g2.drawLine(7+a, 10, 40, 55);
  u8g2.drawLine(7+a*2, 10, 60, 55);
  u8g2.drawLine(7+a*3, 10, 80, 55);
  u8g2.drawLine(7+a*4, 10, 100, 55);
}

void u8g2_triangle(uint8_t a) {
  uint16_t offset = a;
  u8g2.drawStr( 0, 0, "drawTriangle");
  u8g2.drawTriangle(14,7, 45,30, 10,40);
  u8g2.drawTriangle(14+offset,7-offset, 45+offset,30-offset, 57+offset,10-offset);
  u8g2.drawTriangle(57+offset*2,10, 45+offset*2,30, 86+offset*2,53);
  u8g2.drawTriangle(10+offset,40+offset, 45+offset,30+offset, 86+offset,53+offset);
}

void u8g2_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 1");
  for( y = 0; y < 6; y++ ) {
    for( x = 0; x < 16; x++ ) {
      s[0] = y*16 + x + 32;
      u8g2.drawStr(x*7, y*10+10, s);
    }
  }
}

void u8g2_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 2");
  for( y = 0; y < 6; y++ ) {
    for( x = 0; x < 16; x++ ) {
      s[0] = y*16 + x + 160;
      u8g2.drawStr(x*7, y*10+10, s);
    }
  }
}

void u8g2_extra_page(uint8_t a)
{
  u8g2.drawStr( 0, 0, "Unicode");
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setFontPosTop();
  u8g2.drawUTF8(0, 24, "☀ ☁");
  switch(a) {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g2.drawUTF8(a*3, 36, "☂");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      u8g2.drawUTF8(a*3, 36, "☔");
      break;
  }
}

#define cross_width 24
#define cross_height 24
static const unsigned char cross_bits[] U8X8_PROGMEM  = {
  0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 
  0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80, 
  0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03, 
  0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00, };

#define cross_fill_width 24
#define cross_fill_height 24
static const unsigned char cross_fill_bits[] U8X8_PROGMEM  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26, 
  0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08, 
  0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01, 
  0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04, 
  0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21, 
  0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

#define cross_block_width 14
#define cross_block_height 14
static const unsigned char cross_block_bits[] U8X8_PROGMEM  = {
  0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 
  0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 
  0x01, 0x20, 0xFF, 0x3F, };

void u8g2_bitmap_overlay(uint8_t a) {
  uint8_t frame_size = 28;

  u8g2.drawStr(0, 0, "Bitmap overlay");

  u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
  u8g2.setBitmapMode(false /* solid */);
  u8g2.drawFrame(0, 10, frame_size, frame_size);
  u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
  if(a & 4)
    u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);

  u8g2.setBitmapMode(true /* transparent*/);
  u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
  u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
  if(a & 4)
    u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void u8g2_bitmap_modes(uint8_t transparent) {
  const uint8_t frame_size = 24;

  u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  u8g2.drawStr(frame_size * 2, 50, "White");
  u8g2.drawStr(frame_size * 3.5, 50, "XOR");
  
  if(!transparent) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawStr(0, 0, "Solid bitmap");
  } else {
    u8g2.setBitmapMode(true /* transparent*/);
    u8g2.drawStr(0, 0, "Transparent bitmap");
  }
  u8g2.setDrawColor(0);// Black
  u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(1); // White
  u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(2); // XOR
  u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}

uint8_t draw_state = 0;

void draw(void) {
  u8g2_prepare();
  switch(draw_state >> 3) {
    case 0: u8g2_box_frame(draw_state&7); break;
    case 1: u8g2_disc_circle(draw_state&7); break;
    case 2: u8g2_r_frame(draw_state&7); break;
    case 3: u8g2_string(draw_state&7); break;
    case 4: u8g2_line(draw_state&7); break;
    case 5: u8g2_triangle(draw_state&7); break;
    case 6: u8g2_ascii_1(); break;
    case 7: u8g2_ascii_2(); break;
    case 8: u8g2_extra_page(draw_state&7); break;
    case 9: u8g2_bitmap_modes(0); break;
    case 10: u8g2_bitmap_modes(1); break;
    case 11: u8g2_bitmap_overlay(draw_state&7); break;
  }
}

// ##### end graphics test

// ##### start shennong test


// To read the short story with all glyphs you need at least a font with _gb2312b postfix
// However, a font with _gb2312b postfix is very large and will not always fit on
// the target controller. For testing you can use _chinese1 extenstion, but then
// many gylphs are skipped.
//
// wqy fonts are available in different sizes, here are only wqy12, wqy14 and wqy16 listed

//#define FONT u8g2_font_wqy12_t_chinese1
//#define FONT u8g2_font_wqy12_t_gb2312b

//#define FONT u8g2_font_wqy14_t_chinese1
#define FONT u8g2_font_wqy14_t_gb2312b

//#define FONT u8g2_font_wqy16_t_chinese1
//#define FONT u8g2_font_wqy16_t_gb2312b

// The next two macros define the scroll speed of the short story
#define SCROLL_DELTA 2
#define SCROLL_DELAY 200


// Chinese Short Story
// The Farmer God Shen Nong Tastes All the Plants 
// Please excause wrong line breaks

const char c_str[] = 
"Shen Nong\n\n"
"神农一生下来就是\n"
"个水晶肚子，五脏\n"
"六腑全都能看得一\n"
"清二楚。那时侯，\n"
"人们经常因为乱吃\n"
"东西而生病，甚至\n"
"丧命。神农决心尝\n"
"遍所有的东西，好\n"
"吃的放在身边左边\n"
"的袋子里，给人吃\n"
"；\n"
"不好吃的就放在身\n"
"子右边的袋子里，\n"
"作药用。\n"
"第一次，神农尝了\n"
"一片小嫩叶。这叶\n"
"片一落进肚里，就\n"
"上上下下地把里面\n"
"各器官擦洗得清清\n"
"爽爽，\n"
"象巡查似的，\n"
"神农把它叫做\n"
"“查”，\n"
"就是后人所称的\n"
"“茶”。\n"
"神农将它放进右边\n"
"袋子里。第二次，\n"
"神农尝了朵蝴蝶样\n"
"的淡红小花，甜津\n"
"津的，香味扑鼻，\n"
"这是“甘草”。他把\n"
"它放进了左边袋子\n"
"里。就这样，神农\n"
"辛苦地尝遍百草，\n"
"每次中毒，都靠茶\n"
"来解救。后来，\n"
"他左边的袋子里花\n"
"草根叶有四万七千\n"
"种，右边有三十九\n"
"万八千种。\n"
"但有一天，神农尝\n"
"到了“断肠草”，这\n"
"种毒草太厉害了，\n"
"他还来不及吃茶解\n"
"毒就死了。\n"
"他是为了拯救人们\n"
"而牺牲的，人们称\n"
"他为“药王菩萨”，\n"
"人间以这个神话故\n"
"事永远地纪念他。\n";


char buf[48];			// there are at most 8 chinese glyphs per line, max buf size is 8*3 = 24 
 
uint8_t total_lines;		// the total number of lines in the story
uint8_t i;				// loop variable for the lines
uint8_t line_cnt;		// number of lines to draw, usually equal to lines_per_draw
uint8_t start_line;		// topmost visible line, derived from top_window_pos
uint8_t lines_per_draw;	// how many lines to draw on the screen, derived from font and display height
uint16_t glyph_height;	// height of the glyphs

uint16_t top_window_pos;		// defines the display position in pixel within the text
uint16_t total_height;			// total height in pixel, derived from font height and total_lines
u8g2_uint_t top_offset;			// offset between the first visible line and the display

// ##### snd shennong test

// ############ start ug28 logo


void drawLogo(void)
{
    u8g2.setFontMode(1);	// Transparent
#ifdef MINI_LOGO

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(0, 22, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb19_mn);
    u8g2.drawStr(14,8,"8");
    
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(36,22,"g");
    u8g2.drawStr(48,22,"\xb2");
    
    u8g2.drawHLine(2, 25, 34);
    u8g2.drawHLine(3, 26, 34);
    u8g2.drawVLine(32, 22, 12);
    u8g2.drawVLine(33, 23, 12);
#else

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(0, 30, "U");
    
    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb30_mn);
    u8g2.drawStr(21,8,"8");
        
    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(51,30,"g");
    u8g2.drawStr(67,30,"\xb2");
    
    u8g2.drawHLine(2, 35, 47);
    u8g2.drawHLine(3, 36, 47);
    u8g2.drawVLine(45, 32, 12);
    u8g2.drawVLine(46, 33, 12);
    
#endif
}

void drawURL(void)
{
#ifndef MINI_LOGO
  u8g2.setFont(u8g2_font_4x6_tr);
  if ( u8g2.getDisplayHeight() < 59 )
  {
    u8g2.drawStr(89,20,"github.com");
    u8g2.drawStr(73,29,"/olikraus/u8g2");
  }
  else
  {
    u8g2.drawStr(1,54,"github.com/olikraus/u8g2");
  }
#endif
}

//  ************* end u82g logo

// ########### update area

int16_t offset;				// current offset for the scrolling text
u8g2_uint_t width;			// pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined
const char *text = "U8g2";	// scroll this text from right to left

const uint8_t tile_area_x_pos = 2;	// Update area left position (in tiles)
const uint8_t tile_area_y_pos = 3;	// Update area upper position (distance from top in tiles)
const uint8_t tile_area_width = 12;
const uint8_t tile_area_height = 3;	// this will allow cour18 chars to fit into the area

const u8g2_uint_t pixel_area_x_pos = tile_area_x_pos*8;
const u8g2_uint_t pixel_area_y_pos = tile_area_y_pos*8;
const u8g2_uint_t pixel_area_width = tile_area_width*8;
const u8g2_uint_t pixel_area_height = tile_area_height*8;

// ######## end update area

// ###### weather test




#define SUN	0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
  
  switch(symbol)
  {
    case SUN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 69);	
      break;
    case SUN_CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 65);	
      break;
    case CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 64);	
      break;
    case RAIN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 67);	
      break;
    case THUNDER:
      u8g2.setFont(u8g2_font_open_iconic_embedded_6x_t);
      u8g2.drawGlyph(x, y, 67);
      break;      
  }
}

void drawWeather(uint8_t symbol, int degree)
{
  drawWeatherSymbol(0, 48, symbol);
  u8g2.setFont(u8g2_font_logisoso32_tf);
  u8g2.setCursor(48+3, 42);
  u8g2.print(degree);
  u8g2.print("°C");		// requires enableUTF8Print()
}

/*
  Draw a string with specified pixel offset. 
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];	// should for screen with up to 256 pixel width 
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  

  u8g2.setDrawColor(0);		// clear the scrolling area
  u8g2.drawBox(0, 49, u8g2.getDisplayWidth()-1, u8g2.getDisplayHeight()-1);
  u8g2.setDrawColor(1);		// set the color for the text
    
  
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset)/8;
    dx = offset + char_offset*8;
    if ( char_offset >= u8g2.getDisplayWidth()/8 )
      return;
    visible = u8g2.getDisplayWidth()/8-char_offset+1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset*8-dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return;	// nothing visible
    dx = offset - char_offset*8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth()/8+1 )
      visible = u8g2.getDisplayWidth()/8+1;
    strncpy(buf, s+char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
  
}

void draw(const char *s, uint8_t symbol, int degree)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);
  
  u8g2.clearBuffer();					// clear the internal memory
  drawWeather(symbol, degree);		// draw the icon and degree only once
  for(;;)							// then do the scrolling
  {
  
    drawScrollString(offset, s);  			// no clearBuffer required, screen will be partially cleared here
    u8g2.sendBuffer();				// transfer internal memory to the display

    delay(20);
    offset+=2;
    if ( offset > len*8+1 )
      break;
  }
}

// ###### end weather test


void testPins();
void testHelloWorld();
void testFontUsage();
void testFPS();
void testGraphics();
void testPrintUTF8();
void testScreenshot();
void testShennong();
void testu82glogo();
void testUpdateArea();
void testWeather();

bool firstLoop=true;

void loop()
{
//  testHelloWorld();
//  testFontUsage();
//   testFPS();
//  testGraphics();
//  testPrintUTF8();
//  testScreenshot();
// testShennong();
//  testu82glogo();
//testUpdateArea();
testWeather();

  firstLoop=false;
}

void setup() {
  UM.begin();
  UM.setPixelPower(true);
  UM.setPixelColor(0,0,255);
  UM.writePixel();

  Serial.begin(115200);

  delay(500);

  Serial.println("Starting OLED test...");

  Serial.println("Initializing display...");

  u8g2.begin();
  u8g2.setContrast(255);
  Serial.println("Display initialized");

}

void testPins() {
  Serial.println("Testing GPIO pins with logic analyzer...");
  
  pinMode(OLED_RST_BROWN, OUTPUT);
  pinMode(OLED_DC_PURPLE, OUTPUT);
  pinMode(OLED_CS_ORANGE, OUTPUT);
  pinMode(OLED_CLK_SCL_YELLOW, OUTPUT);
  pinMode(OLED_DIN_MOSI_SDA_BLUE, OUTPUT);
  
  int D0_LOGIC_BLUE_TOGGLE=LOW;     // OLED_RST_BROWN
  int D1_LOGIC_GREEN_TOGGLE=LOW;    // OLED_DC_PURPLE
  int D2_LOGIC_GREY_TOGGLE=LOW;     // OLED_CS_ORANGE
  int D3_LOGIC_PURPLE_TOGGLE=LOW;   // OLED_CLK_SCL_YELLOW
  int D4_LOGIC_WHITE_TOGGLE=LOW;    // OLED_DIN_MOSI_SDA_BLUE

  for(int i = 0; i < 100000000; i++) {
    Serial.print("Pin test cycle: "); Serial.println(i);

    D0_LOGIC_BLUE_TOGGLE   =  i % 2;
    D1_LOGIC_GREEN_TOGGLE  =  i % 4;
    D2_LOGIC_GREY_TOGGLE   =  i % 8;
    D3_LOGIC_PURPLE_TOGGLE =  i % 16;
    D4_LOGIC_WHITE_TOGGLE  =  i % 32;

    digitalWrite(OLED_RST_BROWN, D0_LOGIC_BLUE_TOGGLE);           // D0
    digitalWrite(OLED_DC_PURPLE, D1_LOGIC_GREEN_TOGGLE);          // D1
    digitalWrite(OLED_CS_ORANGE, D2_LOGIC_GREY_TOGGLE);           // D2
    digitalWrite(OLED_CLK_SCL_YELLOW, D3_LOGIC_PURPLE_TOGGLE);    // D3
    digitalWrite(OLED_DIN_MOSI_SDA_BLUE, D4_LOGIC_WHITE_TOGGLE);  // D4

    delay(100);
  }
}

void testFPS()
{
  uint16_t fps;
  fps = execute_with_fps(draw_clip_test);
  show_result("draw clip test", fps);
  delay(5000);
  fps = execute_with_fps(draw_set_screen);
  show_result("clear screen", fps);
  delay(5000);
  fps = execute_with_fps(draw_char);
  show_result("draw @", fps);
  delay(5000);  
  fps = execute_with_fps(draw_pixel);
  show_result("draw pixel", fps);
  delay(5000);
  fps = execute_with_fps(draw_line);
  show_result("draw line", fps);
  delay(5000);
}

void testHelloWorld()
{
  Serial.println("testHelloWorld");
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory  Serial.println("");
  u8g2.drawStr(0,30,String(millis()).c_str());  // changing content each loop
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(300);  // Much faster updates for continuous activity
}

void testFontUsage()
{
    // This problem applies only to full buffer mode
  u8g2.clearBuffer();  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		  
  u8g2.setCursor(0,15);
  u8g2.print(F("Problems with"));
  u8g2.setCursor(0,30);
  u8g2.print(F("full buffer mode"));
  u8g2.setCursor(0,45);
  u8g2.print(F("and skipped clear."));
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  draw_m1_t();			// fontmode 1, t font --> wrong
  draw_m1_h();			// fontmode 1, h font --> wrong
  draw_m0_t();			// fontmode 0, t font --> wrong
  draw_m0_h();			// fontmode 1, h font --> ok
  draw_m0_h_with_extra_blank();	// fontmode 1, h font with extra blank --> correct
  delay(1000);  
}

void testGraphics()
{
    // picture loop  
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();
  
  // increase the state
  draw_state++;
  if ( draw_state >= 12*8 )
    draw_state = 0;

  // delay between each page
  delay(100);
}

void testPrintUTF8()
{
  if (firstLoop)
      u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function

  u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFontDirection(0);
  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  u8g2.print("Hello World!");
  u8g2.setCursor(0, 40);
  u8g2.print("你好世界");		// Chinese "Hello World" 
  u8g2.sendBuffer();  
  delay(1000);
}

void testScreenshot()
{
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
  u8g2.drawStr(0,10,"Hello World!");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display
  
  // Create a screenshot. The picture (XBM or PBM format) is sent to the serial output. 
  // Copy and paste the output from the Arduino serial monitor into a file.
  u8g2.writeBufferXBM(Serial);			// Write XBM image to serial out
  
  delay(10000);  
}

void testShennong()
{
  if (firstLoop)
  {
    /* select a font */
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);	// two unknown glyphs
    //u8g2.setFont(u8g2_font_wqy12_t_chinese3);	// two unknown glyphs

    //u8g2.setFont(u8g2_font_wqy12_t_gb2312a);	// ";" is missing
    //u8g2.setFont(u8g2_font_wqy12_t_gb2312b);	// all glyphs available

    u8g2.setFont(FONT);

    /* calculate the length of the text in lines */
    total_lines = u8x8_GetStringLineCnt(c_str);

    /* get the height of the glyphs */
    glyph_height = u8g2.getMaxCharHeight();

    /* calculate the height of the text in pixel */
    total_height = (uint16_t)total_lines * (uint16_t)glyph_height;


    /* calculate how many lines must be drawn on the screen */
    lines_per_draw = u8g2.getDisplayHeight() / glyph_height;
    lines_per_draw += 2;

    /* start at the top of the text */
    top_window_pos = 0;
  }

  start_line = top_window_pos / glyph_height;
  top_offset = top_window_pos % glyph_height;
  
  line_cnt = total_lines - start_line;
  if ( line_cnt > lines_per_draw )
    line_cnt = lines_per_draw;

  u8g2.clearBuffer();					// clear the internal memory
  for( i = 0; i < line_cnt; i++ )			// draw visible lines
  {
    /* copy a line of the text to the local buffer */
    u8x8_CopyStringLine(buf, i+start_line, c_str);
    
    /* draw the content of the local buffer */
    u8g2.drawUTF8(0, i*glyph_height-top_offset +glyph_height, buf);
  }
  u8g2.sendBuffer();					// transfer internal memory to the display
  
  delay(SCROLL_DELAY);
  top_window_pos += SCROLL_DELTA;
}

void testu82glogo()
{
  u8g2.clearBuffer();
  drawLogo();
  drawURL();
  u8g2.sendBuffer();
  delay(1000);
}

void testUpdateArea()
{
  if (firstLoop)
  {
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_helvR10_tr);	// choose a suitable font
    u8g2.drawStr(0,12,"UpdateDisplayArea");	// write something to the internal memory
    
    // draw a frame, only the content within the frame will be updated
    // the frame is never drawn again, but will stay on the display
    u8g2.drawBox(pixel_area_x_pos-1, pixel_area_y_pos-1, pixel_area_width+2, pixel_area_height+2);
    
    u8g2.sendBuffer();					// transfer internal memory to the display
    
    u8g2.setFont(u8g2_font_courB18_tr);	// set the target font for the text width calculation
    width = u8g2.getUTF8Width(text);		// calculate the pixel width of the text
    offset = width+pixel_area_width;
  }

   u8g2.clearBuffer();						// clear the complete internal memory

  // draw the scrolling text at current offset
  u8g2.setFont(u8g2_font_courB18_tr);		// set the target font
  u8g2.drawUTF8(
    pixel_area_x_pos-width+offset, 
    pixel_area_y_pos+pixel_area_height+u8g2.getDescent()-1, 
    text);								// draw the scolling text
  
  // now only update the selected area, the rest of the display content is not changed
  u8g2.updateDisplayArea(tile_area_x_pos, tile_area_y_pos, tile_area_width, tile_area_height);
      
  offset--;								// scroll by one pixel
  if ( offset == 0 )	
    offset = width+pixel_area_width;			// start over again
    
  delay(10);							// do some small delay 
}

void testWeather()
{
  if (firstLoop)
  {
    u8g2.enableUTF8Print();
  }

  draw("What a beautiful day!", SUN, 27);
  draw("The sun's come out!", SUN_CLOUD, 19);
  draw("It's raining cats and dogs.", RAIN, 8);
  draw("That sounds like thunder.", THUNDER, 12);
  draw("It's stopped raining", CLOUD, 15);

}

// ########## START TEST FPS FUNCTIONS

void draw_set_screen(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.setColorIndex(flip_color);
  u8g2.drawBox( 0, 0, u8g2.getWidth(), u8g2.getHeight() );
}


void draw_clip_test(void) {
  u8g_uint_t i, j, k;
  char buf[3] = "AB";
  k = 0;
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_6x10_tf);
  
  for( i = 0; i  < 6; i++ ) {
    for( j = 1; j  < 8; j++ ) {
      u8g2.drawHLine(i-3, k, j);
      u8g2.drawHLine(i-3+10, k, j);
      
      u8g2.drawVLine(k+20, i-3, j);
      u8g2.drawVLine(k+20, i-3+10, j);
      
      k++;
    }
  }
  u8g2.setFontDirection(0);
  u8g2.drawStr(0-3, 50, buf);
  u8g2.setFontDirection(2);
  u8g2.drawStr(0+3, 50, buf);
  
  u8g2.setFontDirection(0);
  u8g2.drawStr(u8g2.getWidth()-3, 40, buf);
  u8g2.setFontDirection(2);
  u8g2.drawStr(u8g2.getWidth()+3, 40, buf);

  u8g2.setFontDirection(1);
  u8g2.drawStr(u8g2.getWidth()-10, 0-3, buf);
  u8g2.setFontDirection(3);
  u8g2.drawStr(u8g2.getWidth()-10, 3, buf);

  u8g2.setFontDirection(1);
  u8g2.drawStr(u8g2.getWidth()-20, u8g2.getHeight()-3, buf);
  u8g2.setFontDirection(3);
  u8g2.drawStr(u8g2.getWidth()-20, u8g2.getHeight()+3, buf);

  u8g2.setFontDirection(0);

}


void draw_char(void) {
  char buf[2] = "@";
  u8g_uint_t i, j;
  // graphic commands to redraw the complete screen should be placed here  
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_6x10_tf);
  j = 8;
  for(;;) {
    i = 0;
    for(;;) {
      u8g2.drawStr( i, j, buf);
      i += 8;
      if ( i > u8g2.getWidth() )
        break;
    }
    j += 8;
    if ( j > u8g2.getHeight() )
      break;
  }
  
}

void draw_pixel(void) {
  u8g_uint_t x, y, w2, h2;
  u8g2.setColorIndex(draw_color);
  w2 = u8g2.getWidth();
  h2 = u8g2.getHeight();
  w2 /= 2;
  h2 /= 2;
  for( y = 0; y < h2; y++ ) {
    for( x = 0; x < w2; x++ ) {
      if ( (x + y) & 1 ) {
        u8g2.drawPixel(x,y);
        u8g2.drawPixel(x,y+h2);
        u8g2.drawPixel(x+w2,y);
        u8g2.drawPixel(x+w2,y+h2);
      }
    }
  }
}


void draw_line(void) {
  u8g2.setColorIndex(draw_color);
  u8g2.drawLine(0,0, u8g2.getWidth()-1, u8g2.getHeight()-1);
}
    // returns unadjusted FPS
uint16_t execute_with_fps(void (*draw_fn)(void)) {
  uint16_t FPS10 = 0;
  uint32_t time;
  
  time = millis() + SECONDS*1000;
  
  // picture loop
  do {
    u8g2.clearBuffer();
    draw_fn();
    u8g2.sendBuffer();
    FPS10++;
    flip_color = flip_color ^ 1;
  } while( millis() < time );
  return FPS10;  
}

const char *convert_FPS(uint16_t fps) {
  static char buf[6];
  strcpy(buf, u8g2_u8toa( (uint8_t)(fps/10), 3));
  buf[3] =  '.';
  buf[4] = (fps % 10) + '0';
  buf[5] = '\0';
  return buf;
}

void show_result(const char *s, uint16_t fps) {
  // assign default color value
  u8g2.setColorIndex(draw_color);
  u8g2.setFont(u8g2_font_8x13B_tf);
  u8g2.clearBuffer();
  u8g2.drawStr(0,12, s);
  u8g2.drawStr(0,24, convert_FPS(fps));
  u8g2.sendBuffer();
}

// ########## START TEST FPS FUNCTIONS


// ########## START TEST FONT USAGE FUNCTIONS
void draw(int is_blank)
{
  int i, j;
  int n;
  char s[4];
  
  for( j = 0; j < 20; j++ )
  {
    // random number
    n = lcg_rnd();
    
    // random string
    for( i = 0; i < 3; i++ )
    {
      s[i] = lcg_rnd()>>3;
      if ( s[i] < 16 )
	s[i] += 'a';
      else
	s[i] += 'A';
    }
    s[3] = '\0';
    
    // print number
    u8g2.setCursor(0,15);
    u8g2.print("Number: ");
    if ( is_blank )
      u8g2.print("       ");
    u8g2.setCursor(70,15);
    u8g2.print(n);
    
    
    // print string
    u8g2.setCursor(0,30);
    u8g2.print("Text: ");
    u8g2.setCursor(70,30);
    u8g2.print(s);
    if ( is_blank )
      u8g2.print("        ");
    
    // make the result visible
    u8g2.sendBuffer();
    
    // delay, so that the user can see the result
    delay(200);
  }
}

void draw_m1_t()
{
  u8g2.clearBuffer();

  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		

  u8g2.setCursor(0,15);
  u8g2.print(F("setFontMode(1);"));
  u8g2.setCursor(0,30);
  u8g2.print(F("setFont(..._tr);"));
  u8g2.setCursor(0, 55);
  u8g2.print(F("Very Bad"));
    
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  u8g2.setFontMode(1);		
  u8g2.setFont(u8g2_font_cu12_tr);		
  u8g2.clearBuffer();					// clear the internal memory once
  draw(0);
}

void draw_m0_t()
{
  u8g2.clearBuffer();
  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		
  
  u8g2.setCursor(0,15);
  u8g2.print(F("setFontMode(0);"));
  u8g2.setCursor(0,30);
  u8g2.print(F("setFont(.._tr);"));
  u8g2.setCursor(0, 55);
  u8g2.print(F("Wrong"));
  
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_cu12_tr);		  
  u8g2.clearBuffer();					// clear the internal memory once
  draw(0);
}

void draw_m1_h()
{
  u8g2.clearBuffer();
  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		
  
  u8g2.setCursor(0,15);
  u8g2.print(F("setFontMode(0);"));
  u8g2.setCursor(0,30);
  u8g2.print(F("setFont(.._hr);"));  
  u8g2.setCursor(0, 55);
  u8g2.print(F("Still bad"));
  
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_hr);		
  u8g2.clearBuffer();					// clear the internal memory once
  draw(0);
}



void draw_m0_h()
{
  u8g2.clearBuffer();
  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		
  
  u8g2.setCursor(0,15);
  u8g2.print(F("setFontMode(0);"));
  u8g2.setCursor(0,30);
  u8g2.print(F("setFont(.._hr);"));  
  u8g2.setCursor(0, 55);
  u8g2.print(F("Almost ok"));
  
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_cu12_hr);		
  u8g2.clearBuffer();					// clear the internal memory once
  draw(0);
}

void draw_m0_h_with_extra_blank()
{
  u8g2.clearBuffer();
  
  u8g2.setFontMode(1);
  u8g2.setFont(u8g2_font_cu12_tr);		
  
  u8g2.setCursor(0,15);
  u8g2.print(F("setFontMode(0);"));
  u8g2.setCursor(0,30);
  u8g2.print(F("setFont(.._hr);"));  
  u8g2.setCursor(0, 55);
  u8g2.print(F("Extra blank --> Ok"));
  
  u8g2.sendBuffer();
  delay(INFO_SCREEN_DELAY);
  
  u8g2.setFontMode(0);
  u8g2.setFont(u8g2_font_cu12_hr);		
  u8g2.clearBuffer();					// clear the internal memory once
  draw(1);
}

// ########## END TEST FONT USAGE FUNCTIONS
