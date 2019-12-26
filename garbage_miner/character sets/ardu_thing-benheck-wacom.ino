#include <Arduboy2.h>
Arduboy2 a;

#define SMEM a.getBuffer()

//#include <Arduboy2Core.h>
//Arduboy2Core a;

int playerx = 0;
int playery = 9;

int timer = 0;

int8_t scrollX = 0;
uint8_t scrollTile = 0;                       //Course scroll tile counter. Counts actual tiles, not their nibbles in memory

int scrollSpeed = 1;

uint8_t tileMap[256];                         //1 byte per tile, (2) screens at 16x8 each

int fineScroll[4];                            //Fine scrolling # for each of the 4 scrolling "layers"
int courseScroll[4];                          //Course scrolling amount

uint8_t scrollRow[8];

int scrollCounter = 0;

int spriteX[8];
int spriteY[8];

int subFire = 0;
uint16_t fire = 768;

int jumpV = 0;
int jumpD = 0;

int running = 0;

unsigned char sprite[4][2] = {
  
  {'A', 'B'},
  {'C', 'D'},
  {'E', 'F'},
  {'G', 'H'}
  
}; 

const unsigned char anim[] PROGMEM = {

0x00, 0x00, 0x38, 0x1c, 0xc, 0xc, 0x18, 0xff, 0xff, 0x37, 0x70, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1c, 0xc, 0xc, 0x18, 0xff, 0xff, 0x77, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x38, 0x18, 0x18, 0xff, 0xff, 0x77, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x38, 0x18, 0x18, 0xff, 0xff, 0x67, 0x60, 0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xf0, 0x18, 0x18, 0xff, 0xff, 0x67, 0x60, 0x30, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xf0, 0x18, 0xff, 0xff, 0xc7, 0x60, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xf8, 0xff, 0xff, 0xc7, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xff, 0xff, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x78, 0xff, 0xff, 0xc7, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x78, 0xff, 0xff, 0x67, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xf8, 0x18, 0xff, 0xff, 0x67, 0x60, 0x60, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1c, 0xc, 0xc, 0xff, 0xff, 0x77, 0x60, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x70, 0x30, 0x38, 0x18, 0x1e, 0xf, 0x3, 0x1, 0x3, 0xf, 0x3e, 0xf8, 0xb0, 0x80, 0x00, 0x00, 0x00, 0x70, 0x30, 0x38, 0x18, 0x1e, 0xf, 0x3, 0xf, 0x3e, 0xfc, 0xb0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x18, 0x18, 0x18, 0x1e, 0xf, 0x3f, 0xfc, 0xb0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x18, 0x18, 0x1f, 0x3f, 0xff, 0xb8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x18, 0x3f, 0xff, 0xbe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x3e, 0xff, 0xbf, 0x8e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xfc, 0xbf, 0xdf, 0x1e, 0xc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xf8, 0xbf, 0xbf, 0x5f, 0x1e, 0xc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xf8, 0xb8, 0x9f, 0x1f, 0x1f, 0x2e, 0xc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xf8, 0xb8, 0x9c, 0xf, 0x7, 0x3, 0x13, 0x1b, 0x2e, 0xc, 0x00, 0x00, 0x00, 0x00, 0x20, 0xf0, 0xb8, 0x9c, 0xe, 0x7, 0x3, 0x1, 0x3, 0x7, 0x3e, 0x3c, 0x20, 0x00, 0x00, 0x20, 0x60, 0xf0, 0xb8, 0x1c, 0xe, 0x7, 0x3, 0x1, 0x3, 0x7, 0xe, 0x1c, 0x78, 0x60, 0x40, 
};


const unsigned char charSet[] PROGMEM = {

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8, 0x8, 0xc, 0x1c, 0x1c, 0x1c, 0x2c, 0x2e, 0x2e, 0x2e, 0x36, 0x16, 0x16, 0x16, 0xe, 0xe, 0x6, 0x24, 0x4, 0x20, 0x24, 0x30, 0x50, 0x50, 0x50, 0x52, 0x50, 0x52, 0x62, 0x42, 0x42, 0x2, 0x42, 0x2, 0x6, 0x7, 0x5, 0x5, 0x5, 0x5, 0x5, 0x5, 0x7, 0x6, 0x2, 0x2, 0x2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7, 0xf9, 0x7, 0xfd, 0x55, 0xa9, 0xa9, 0xa9, 0xa9, 0x21, 0x89, 0x21, 0x1, 0x49, 0xf9, 0x7, 0x80, 0x40, 0x20, 0x50, 0x28, 0x1c, 0x2e, 0x5f, 0x2e, 0x1c, 0x28, 0x70, 0x40, 0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x40, 0x40, 0x40, 0x20, 0x20, 0x10, 0x50, 0x38, 0x1c, 0x3e, 0x5c, 0x30, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0x80, 0x80, 0x80, 0xc0, 0x80, 0x8f, 0xf5, 0x6a, 0xb5, 0x5a, 0x2a, 0x2a, 0x2a, 0x2a, 0x22, 0x8, 0x22, 0x00, 0x40, 0xe4, 0x9f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5e, 0x5e, 0x00, 0x00, 0x00, 0x00, 0xe, 0xe, 0x00, 0x00, 0xe, 0xe, 0x00, 0x24, 0x7e, 0x7e, 0x24, 0x24, 0x7e, 0x7e, 0x24, 0x00, 0x24, 0x2e, 0x6b, 0x6b, 0x3a, 0x12, 0x00, 0x00, 0x66, 0x36, 0x18, 0xc, 0x66, 0x62, 0x00, 0x00, 0x30, 0x7a, 0x4f, 0x5d, 0x37, 0x72, 0x50, 0x00, 0x30, 0x7a, 0x4f, 0x5d, 0x37, 0x72, 0x50, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x66, 0x42, 0x00, 0x00, 0x00, 0x42, 0x66, 0x7e, 0x3c, 0x00, 0x00, 0x8, 0x2a, 0x3e, 0x1c, 0x1c, 0x3e, 0x2a, 0x8, 0x00, 0x8, 0x8, 0x3e, 0x3e, 0x8, 0x8, 0x00, 0x00, 0x00, 0x80, 0xe0, 0x60, 0x00, 0x00, 0x00, 0x00, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x60, 0x30, 0x18, 0xc, 0x6, 0x2, 0x00, 0x00, 0x3c, 0x7e, 0x52, 0x4a, 0x7e, 0x3c, 0x00, 0x00, 0x40, 0x44, 0x7e, 0x7e, 0x40, 0x40, 0x00, 0x00, 0x44, 0x66, 0x72, 0x5a, 0x4e, 0x44, 0x00, 0x00, 0x22, 0x62, 0x4a, 0x5e, 0x76, 0x22, 0x00, 0x00, 0x30, 0x38, 0x2c, 0x7e, 0x7e, 0x20, 0x00, 0x00, 0x2e, 0x6e, 0x4a, 0x4a, 0x7a, 0x32, 0x00, 0x00, 0x3c, 0x7e, 0x4a, 0x4a, 0x7a, 0x30, 0x00, 0x00, 0x2, 0x62, 0x72, 0x1a, 0xe, 0x6, 0x00, 0x00, 0x34, 0x7e, 0x4a, 0x4a, 0x7e, 0x34, 0x00, 0x00, 0x4, 0x4e, 0x4a, 0x6a, 0x3e, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xec, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8, 0x1c, 0x36, 0x63, 0x41, 0x00, 0x00, 0x24, 0x24, 0x24, 0x24, 0x24, 0x24, 0x00, 0x00, 0x41, 0x63, 0x36, 0x1c, 0x8, 0x00, 0x00, 0x00, 0x4, 0x6, 0x52, 0x5a, 0xe, 0x4, 0x00, 0x00, 0x3c, 0x7e, 0x42, 0x5a, 0x5e, 0x5c, 0x00, 0x00, 0x78, 0x7c, 0x26, 0x26, 0x7c, 0x78, 0x00, 0x00, 0x7e, 0x7e, 0x4a, 0x4a, 0x7e, 0x34, 0x00, 0x00, 0x3c, 0x7e, 0x42, 0x42, 0x66, 0x24, 0x00, 0x00, 0x7e, 0x7e, 0x42, 0x66, 0x3c, 0x18, 0x00, 0x00, 0x7e, 0x7e, 0x4a, 0x4a, 0x4a, 0x42, 0x00, 0x00, 0x7e, 0x7e, 0xa, 0xa, 0xa, 0x2, 0x00, 0x00, 0x3c, 0x7e, 0x42, 0x52, 0x72, 0x72, 0x00, 0x00, 0x7e, 0x7e, 0x8, 0x8, 0x7e, 0x7e, 0x00, 0x00, 0x42, 0x42, 0x7e, 0x7e, 0x42, 0x42, 0x00, 0x00, 0x20, 0x60, 0x40, 0x42, 0x7e, 0x3e, 0x00, 0x00, 0x7e, 0x7e, 0x18, 0x3c, 0x66, 0x42, 0x00, 0x00, 0x7e, 0x7e, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x7e, 0x7e, 0xc, 0x18, 0xc, 0x7e, 0x7e, 0x00, 0x7e, 0x7e, 0x1c, 0x38, 0x7e, 0x7e, 0x00, 0x00, 0x3c, 0x7e, 0x42, 0x42, 0x7e, 0x3c, 0x00, 0x00, 0x7e, 0x7e, 0x12, 0x12, 0x1e, 0xc, 0x00, 0x00, 0x3c, 0x7e, 0x42, 0x22, 0x7e, 0x5c, 0x00, 0x00, 0x7e, 0x7e, 0x12, 0x32, 0x7e, 0x4c, 0x00, 0x00, 0x4, 0x4e, 0x4a, 0x4a, 0x7a, 0x30, 0x00, 0x00, 0x2, 0x2, 0x7e, 0x7e, 0x2, 0x2, 0x00, 0x00, 0x7e, 0x7e, 0x40, 0x40, 0x7e, 0x7e, 0x00, 0x00, 0x1e, 0x3e, 0x60, 0x60, 0x3e, 0x1e, 0x00, 0x00, 0x7e, 0x7e, 0x30, 0x18, 0x30, 0x7e, 0x7e, 0x00, 0x66, 0x7e, 0x18, 0x18, 0x7e, 0x66, 0x00, 0x00, 0x6, 0xe, 0x78, 0x78, 0xe, 0x6, 0x00, 0x00, 0x62, 0x72, 0x5a, 0x4e, 0x46, 0x42, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e, 0x42, 0x42, 0x00, 0x00, 0x6, 0xc, 0x18, 0x30, 0x60, 0x40, 0x00, 0x00, 0x00, 0x42, 0x42, 0x7e, 0x7e, 0x00, 0x00, 0x00, 0x10, 0xc, 0xc, 0x6, 0xc, 0xc, 0x10, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0xff, 0xfe, 0x7c, 0x3c, 0x14, 0x00, 0x8, 0x00, 0xff, 0xfe, 0x7e, 0xbc, 0x91, 0x4, 0x00, 0x8, 0xff, 0x7f, 0xfe, 0x46, 0x52, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

};

void setup() {

  a.boot();
  a.setFrameRate(60);  

  int clouds = 1;

  for (int i = 0 ; i < 32 ; i++) {
    tileMap[i] = clouds;
    if (++clouds > 8) {
      clouds = 1;
    }
  }

  clouds = 16;

  for (int i = 0 ; i < 32 ; i++) {
    tileMap[i + 32] = clouds;
    if (++clouds > 19) {
      clouds = 16;
    }
  }

  for (int i = 0 ; i < 32 ; i++) {
    tileMap[i + (6 << 5)] = 29;
  }

  tileMap[(3 << 5) + 0] = 14;
  tileMap[(3 << 5) + 1] = 15;

  for (int i = 4 ; i < 7 ; i++) {

    tileMap[(i << 5) + 0] = 30;
    tileMap[(i << 5) + 1] = 31;
  
  }

  tileMap[(4 << 5) + 20] = 14;
  tileMap[(4 << 5) + 21] = 15;

  for (int i = 5 ; i < 7 ; i++) {

    tileMap[(i << 5) + 20] = 30;
    tileMap[(i << 5) + 21] = 31;
  
  }

  tileMap[(4 << 5) + 10] = 14;
  tileMap[(4 << 5) + 11] = 15;

  for (int i = 5 ; i < 7 ; i++) {

    tileMap[(i << 5) + 10] = 30;
    tileMap[(i << 5) + 11] = 31;
  
  }
  
  scrollRow[0] = 0;         //Set what scroller will be used for this line
  scrollRow[1] = 1;
  scrollRow[2] = 2;
  scrollRow[3] = 2;
  scrollRow[4] = 2;
  scrollRow[5] = 2;
  scrollRow[6] = 2;
  scrollRow[7] = 3;        //Static scroller, use for score etc

  spriteX[0] = 4;
  spriteY[0] = 48;

}

void loop() {

    //delay(15);

    if (!(a.nextFrame())) {       //Lock to 60 FPS
      return;
    } 

    scrollCounter++;

    scroll(2, 1);

    if ((scrollCounter & 0x02) == 0x02) {
      scroll(1, 1);
    }

    if ((scrollCounter & 0x03) == 0x03) {
      scroll(0, 1);
      scrollCounter = 0;
    }

    if (a.pressed(UP_BUTTON) && spriteY[0] > 0) {
        spriteY[0] -= 1;
    }
    if (a.pressed(DOWN_BUTTON) && spriteY[0] < 48) {
        spriteY[0] += 1;
    }

	if (jumpD) {
		jumpLogic();
	}
	
    if (a.pressed(B_BUTTON) && jumpD == 0) {
        jumpD = 1;
		jumpV = 8;
    }
        
    drawNumber(a.cpuLoad(), 13, 7);

	if (running++ == 11) {
		running = 0;
	}	
	
    scrollFire();
	
	
	if (++subFire == 4) {
		subFire = 0;
		fire += 8;
	}

	if (fire > 784) {
		fire = 768;
	}
	


    //scrollSPI();

    //scrollFB();      

}

void jumpLogic() {


	switch(jumpD) {
		
		case 1:
		
			spriteY[0] -= jumpV;
			if (--jumpV == 0) {
				jumpD = 2;
			}
			break;
			
		case 10:
			spriteY[0] += jumpV;
			if (++jumpV == 9) {
				jumpD = 0;
			}
			break;
			
		default:			//Floatiness
			jumpD++;
			break;

	}
	
}	

void scroll(int layer, int amount) {

  fineScroll[layer] += amount;

  if (fineScroll[layer] > 7) {
    fineScroll[layer] -= 8;
    courseScroll[layer]++;
    if (courseScroll[layer] > 31) {
      courseScroll[layer] = 0;
    }
    return;                         //Don't bother checking the other
  }

  if (fineScroll[layer] < 0) {
    fineScroll[layer] += 8;
    courseScroll[layer]--;
    if (courseScroll[layer] < 0) {
      courseScroll[layer] = 31;
    }
  }  


  
}

void text(uint8_t xPos, uint8_t yPos, const char *str) {

  xPos += (yPos << 5);    //yPos * 32

  while (*str) {
    tileMap[xPos++] = *str++; 
  }

  
}

void drawNumber(int theValue, int x, int y) {
	
  int numerals = (y << 5) + x;           //Always going to be at least 1 numeral
  int zPad = 0;             //Flag for zero padding 
  unsigned long divider = 100;     //Divider starts at 100k
    
  for (int xx = 0 ; xx < 3 ; xx++) {    //6 digit number    
    if (theValue >= divider) {
      tileMap[numerals + 1] = '0' + (theValue / divider);
      theValue %= divider;
      zPad = 1;
      numerals += 1;
    }   
    else if (zPad or divider == 1) {
      tileMap[numerals + 1] = '0';
      numerals += 1;
    } 
    divider /= 10;            
  } 	
	
	
}

void scrollFire() {

	uint8_t spY;
	uint8_t shiftL;
	uint8_t shiftR;
			
	for (uint8_t row = 0 ; row < 7 ; row++) {                   //Draw the 8 row high display

		uint8_t finePointer = fineScroll[scrollRow[row]];         //Copy the fine scrolling amount so we can use it as a byte pointer when scanning in graphics
		uint8_t courseLeft = (row << 5);                          //Find the leftmost edge of this row in tileMap memory
		uint8_t courseByte = courseLeft + courseScroll[scrollRow[row]]; //Find the current coarse position
		uint8_t courseRight = courseLeft + 32;                    //Find the rightmost position in case we need to loop

		uint8_t spritePointer = 0;
	
		for (uint8_t colB = 0 ; colB < 16 ; colB++) {             //Draw 16 column wide display (8 pixels / 1 char at a time)

		  switch(colB) {

			case 0:

			  for (uint8_t col = 0 ; col < 8 ; col++) {

				SPDR = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer) | pgm_read_byte_near(charSet + col + fire);                
		  
				if (++finePointer > 7) {               //Done drawing this tile?
				  finePointer = 0;                     //Reset fine scroll
				  courseByte++;                        //...and advance course scroll by 1 byte (2 tiles)
				  if (courseByte == courseRight) {
					courseByte = courseLeft;
				  }
				}
				
			  }

			  break;
	  
			case 1: case 2:

				spY = row - (spriteY[0] >> 3);                	 	//Get course row
				shiftL = spriteY[0] & 0x07;                    		//Get fine remainder
				shiftR = 8 - shiftL;								//Get inverse of shiftL				
			
				for (uint8_t col = 0 ; col < 8 ; col++) {

					int8_t nextByte = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer);		//Get BG tile

					if (!(spY & 0xFC)) {
					
						switch(spY) {							
							case 0:
								nextByte |= pgm_read_byte_near(anim + (running << 4) + spritePointer)  << shiftL;
							break;
							case 1:
								nextByte |= pgm_read_byte_near(anim + 192 + (running << 4) + spritePointer) << shiftL;
								nextByte |= pgm_read_byte_near(anim + (running << 4) + spritePointer) >> shiftR;															
							break;								
							case 2:						
								nextByte |= pgm_read_byte_near(anim + 192 + (running << 4) + spritePointer) >> shiftR;
							break;				
						}

/*

						switch(spY) {							
							case 0:
								nextByte |= pgm_read_byte_near(charSet + (sprite[spY][0] << 3) + spritePointer) << shiftL;
							break;
							case 1:
								nextByte |= pgm_read_byte_near(charSet + (sprite[spY][0] << 3) + spritePointer) << shiftL;
								nextByte |= pgm_read_byte_near(charSet + (sprite[spY - 1][0] << 3) + spritePointer) >> shiftR;																
							break;								
							case 2:						
								nextByte |= pgm_read_byte_near(charSet + (sprite[spY - 1][0] << 3) + spritePointer) >> shiftR;
							break;				
						}
						
						*/

						
					}
					
					spritePointer++;
					
					SPDR = nextByte;             

					if (++finePointer > 7) {               //Done drawing this tile?
					  finePointer = 0;                     //Reset fine scroll
					  courseByte++;                        //...and advance course scroll by 1 byte (2 tiles)
					  if (courseByte == courseRight) {
						courseByte = courseLeft;
					  }
					}

				}

			  break;

			default:

			  for (uint8_t col = 0 ; col < 8 ; col++) {

				SPDR = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer);                
		  
				if (++finePointer > 7) {               //Done drawing this tile?
				  finePointer = 0;                     //Reset fine scroll
				  courseByte++;                        //...and advance course scroll by 1 byte (2 tiles)
				  if (courseByte == courseRight) {
					courseByte = courseLeft;
				  }
				}
				
			  }

			  break;

		  }

		}

	}

  uint8_t finePointer = 0;
  uint8_t courseLeft = (7<< 5);                          //Find the leftmost edge of this row in tileMap memory
  uint8_t courseByte = courseLeft + courseScroll[scrollRow[7]]; //Find the current coarse position
  uint8_t courseRight = courseLeft + 32;                    //Find the rightmost position in case we need to loop
  
  for (uint8_t col = 0 ; col < 128 ; col++) {               //Draw all 128 vertical columns on this row (128x8)

    SPDR = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer);  //Just dump char tile data directly onto SPI bus (no ORing)

    if (++finePointer > 7) {               //Done drawing this tile?
      finePointer = 0;                     //Reset fine scroll
      courseByte++;                        //...and advance course scroll by 1 byte (2 tiles)
    }
  
  }

}

void scrollSPI() {

  for (uint8_t row = 0 ; row < 8 ; row++) {                   //Draw the 8 row high display

    uint8_t finePointer = fineScroll[scrollRow[row]];         //Copy the fine scrolling amount so we can use it as a byte pointer when scanning in graphics
    uint8_t courseLeft = (row << 5);                          //Find the leftmost edge of this row in tileMap memory
    uint8_t courseByte = courseLeft + courseScroll[scrollRow[row]]; //Find the current coarse position
    uint8_t courseRight = courseLeft + 32;                    //Find the rightmost position in case we need to loop

    for (uint8_t col = 0 ; col < 128 ; col++) {               //Draw all 128 vertical columns on this row (128x8)

      if (scrollRow[row] == 3) {                              //Don't draw sprites on static info display lines

        SPDR = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer);  //Just dump char tile data directly onto SPI bus (no ORing)

      }
      else {                                                  //Normal line with sprites

        uint8_t nextByte = pgm_read_byte_near(charSet + (tileMap[courseByte] << 3) + finePointer);  //Get byte from char set, place in temp variable   
        
        for (int sp = 0 ; sp < 2 ; sp++) {
  
          int8_t spX = col - spriteX[sp];
          int8_t spY = row - (spriteY[sp] >> 3);                 //Get course row
          int8_t shiftL = spriteY[sp] & 0x07;                     //Get remainder
          int8_t shiftR = 8 - shiftL;
          
          if (!(spX & 0xE0) && !(spY & 0xF8)) {                   //Lower nibble only (not negative, not over 15)
            
            nextByte |= pgm_read_byte_near(charSet + (sprite[spY][spX >> 3] << 3) + (spX & 0x07)) << shiftL;
  
            if (spY) {
              nextByte |= pgm_read_byte_near(charSet + (sprite[spY - 1][spX >> 3] << 3) + (spX & 0x07)) >> shiftR;  
            }
            
          }

        }

        SPDR = nextByte;                      //Dump onto SPI bus (and hope it's sent before next loop)
           
      }

      if (++finePointer > 7) {               //Done drawing this tile?
        finePointer = 0;                     //Reset fine scroll
        courseByte++;                        //...and advance course scroll by 1 byte (2 tiles)
        if (courseByte == courseRight) {
          courseByte = courseLeft;
        }
      }
  
    }
    
  }
  
}
