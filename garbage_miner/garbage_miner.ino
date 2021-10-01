#include <Arduboy2Core.h>
#include "ArduboyBeep.h" // (local) "beep" classes
#include <EEPROM.h>

BeepPin2 beep; // class instance for speaker pin 1
Arduboy2Core a;

#define menu 			0		//Game states
#define game 			1
#define edit 			2
#define story 			3
#define levelStart		4
#define gameOver		5
#define levelStartRogue	6

#define empty 	0				//Object values (lower nibble of map bytes)
#define dirt1 	1
#define man   	2
#define door	3
#define wall  	4
#define ball  	5
#define unused	6				//Turn to flammable gas?
#define jar  	7
#define seeker	8
#define jem		9
#define slime 	10
#define monster 11
#define crawler 12
#define exp1  13
#define exp2  14
#define exp3  15

#define moveUp    0x00			//Object attributes (upper nibble of map bytes)
#define moveDown  0x10
#define moveLeft  0x20
#define moveRight 0x30
#define dontMove  0x80				//Change to 0x40?
#define falling	  0x80

#define upBit		0x80		//Bitwise defines for one-shot button presses
#define downBit		0x40
#define leftBit		0x20
#define rightBit 	0x10
#define aBit		0x08
#define bBit		0x04

#define pushPower 5				//How many reps to push a boulder

uint8_t level = 1;						//Which game level we are on
uint8_t lives = 3;						//How many men are left
uint32_t score = 0;						//Player score

uint16_t levelPointer = 0;				//The FLASH pointer of what we're reading

uint16_t levelSize[] = {0, 408, 278, 154, 247, 424, 65535};	//Relative offset pointers to start of flash memory for all levels (use level-1 for this because of zero offset) 65535 = end of level data

const uint8_t levelData[] PROGMEM = {    //The built in levels data

0xF, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x1F, 0x1F, 0x12, 0x41, 0x14, 0x7, 0x51, 0x1, 0x1F, 0x1F, 0x12, 0x41, 0x13, 0x52, 0x5, 0x11, 0x51, 0x91, 0x1F, 0x1F, 0x12, 0x41, 0x13, 0x53, 0x2, 0x31, 0x1, 0x11, 0x52, 0x1F, 0x1F, 0x12, 0x41, 0x12, 0x55, 0x3, 0x11, 0x42, 0x1F, 0x1E, 0x1, 0x12, 0x41, 0x1, 0x12, 0x58, 0x41, 0x51, 0x1F, 0x1E, 0x2, 0x11, 0x41, 0x2, 0x12, 0x56, 0x11, 0x41, 0x1F, 0x1F, 0x3, 0x51, 0x4, 0x11, 0x54, 0x12, 0x41, 0x1F, 0x1F, 0x2, 0x11, 0x41, 0x4, 0x13, 0x52, 0x12, 0x41, 0x1F, 0x1F, 0x1, 0x12, 0x41, 0x7, 0x13, 0x42, 0x1F, 0x1F, 0x13, 0x41, 0x8, 0x12, 0x41, 0x2, 0x1F, 0x1F, 0x12, 0x41, 0x9, 0x11, 0x42, 0x1, 0x1F, 0x1F, 0x12, 0x41, 0x9, 0x12, 0x41, 0x1, 0x56, 0x1F, 0x1B, 0x41, 0x9, 0x12, 0x41, 0x11, 0x55, 0x1F, 0x17, 0x4F, 0x43, 0x11, 0x54, 0x1F, 0x18, 0x41, 0x1F, 
0x13, 0x42, 0x51, 0x4F, 0x4A, 0x94, 0x4D, 0x1, 0x17, 0x43, 0x1D, 0x52, 0x11, 0x42, 0x94, 0x41, 0x11, 0x91, 0x2, 0x51, 0x3, 0x13, 0x42, 0x4, 0x14, 0x51, 0x13, 0x51, 0x13, 0x52, 0x13, 0x91, 0x11, 0x52, 0x11, 0x42, 0x54, 0x41, 0x12, 0x1, 0x11, 0x41, 0x4, 0x13, 0x51, 0x6, 0x12, 0x51, 0x12, 0x52, 0x11, 0x91, 0x12, 0x51, 0x1F, 0x12, 0x2, 0x41, 0x5, 0x12, 0x51, 0x6, 0x12, 0x51, 0x13, 0x51, 0x14, 0x51, 0x12, 0x52, 0x16, 0x4, 0x13, 0x1, 0x52, 0x5, 0x91, 0x11, 0x51, 0x2, 0x91, 0x3, 0x12, 0x51, 0x11, 0x91, 0x1A, 0x52, 0x15, 0x4, 0x13, 0x1, 0x11, 0x41, 0x17, 0x51, 0x4F, 0x4F, 0x4A, 0x51, 0x14, 0x52, 0x18, 0x91, 0x12, 0x51, 0x4, 0x12, 0x41, 0x92, 0x13, 0x59, 0x41, 0x91, 0x51, 0x12, 0x41, 0x53, 0x91, 0x52, 0x12, 0x3, 0x12, 0x91, 0x14, 0x41, 0x4, 0x12, 
0x41, 0x92, 0x1A, 0x52, 0x41, 0x91, 0x51, 0x12, 0x41, 0x53, 0x11, 0x52, 0x11, 0x54, 0x1, 0x15, 0x51, 0x41, 0x51, 0x3, 0x12, 0x41, 0x92, 0x17, 0x1, 0x13, 0x51, 0x41, 0x91, 0x51, 0x12, 0x41, 0x53, 0x11, 0x58, 0x14, 0x52, 0x41, 0x52, 0x2, 0x11, 0x51, 0x41, 0x92, 0x16, 0x41, 0x2, 0x11, 0x1, 0x51, 0x41, 0x91, 0x13, 0x41, 0x53, 0x11, 0x52, 0x45, 0x51, 0x47, 0x14, 0x92, 0x18, 0x51, 0x41, 0x3, 0x12, 0x1, 0x11, 0x1, 0x12, 0x41, 0x51, 0x11, 0x51, 0x11, 0x51, 0x11, 0x5, 0x51, 0x1, 0x15, 0x41, 0x16, 0x41, 0x2, 0x15, 0x51, 0x41, 0x4, 0x51, 0x41, 0x3, 0x11, 0x41, 0x11, 0x1, 0x51, 0x11, 0x51, 0x2, 0x21, 0x2, 0x11, 0x91, 0x16, 0x41, 0x15, 0x51, 0x41, 0x3, 0x14, 0x51, 0x41, 0x3, 0x52, 0x41, 0x4, 0x11, 0x91, 0x1, 0x11, 0x1, 0x11, 0x4, 0x11, 0x1, 0x11, 
0x2, 0x14, 0x41, 0x14, 0x52, 0x41, 0x4, 0x12, 0x52, 0x41, 0x2, 0x53, 0x41, 0x5, 0x11, 0x5, 

0xF, 0x0, 0x2C, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x11, 0x3, 0x51, 0x2, 0x11, 0x51, 0x4, 0xC1, 0x51, 0x41, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x92, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0xC1, 0x91, 0x42, 0x56, 0x45, 0x2, 0x12, 0x51, 0x2, 0xC1, 0x51, 0x11, 0x41, 0x9F, 0x97, 0x42, 0x56, 0x14, 0x41, 0x71, 0x2, 0x12, 0x53, 0x12, 0x41, 0x9F, 0x97, 0x43, 0x55, 0x11, 0x1, 0x15, 0x1, 0x17, 0x41, 0x1F, 0x17, 0x41, 0x11, 0x41, 0x55, 0x11, 0x1, 0x11, 0x1, 0x13, 0x1, 0x17, 0x41, 0xF, 0x7, 0x41, 0x11, 0x42, 0x54, 0x11, 0x1, 0x11, 0x1, 0x11, 0x1, 0x11, 0x1, 0x17, 0x41, 0xF, 0x7, 0x41, 0x12, 0x41, 0x54, 0x11, 0x1, 0x11, 0x1, 0x11, 0x1, 0x11, 0xF, 0xF, 0x1, 0x13, 0x42, 0x53, 0x11, 0x1, 0x11, 0x1, 0x11, 0x1, 0x17, 0x2, 0x4F, 0x49, 0x13, 0x41, 0x53, 0x11, 0x1, 
0x11, 0xC1, 0x11, 0x1, 0x17, 0x2, 0x1F, 0x1C, 0x42, 0x52, 0x11, 0xC1, 0x11, 0x1, 0x11, 0xC1, 0x17, 0x2, 0x1F, 0x1D, 0x41, 0x52, 0x11, 0x1, 0x11, 0x1, 0x11, 0x1, 0x18, 0xF, 0xC, 0x44, 0x51, 0x1F, 0x1F, 0x17, 0x41, 0x1F, 0x1F, 0x1F, 0xF1, 0x13, 0x44, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 0x1, 0x1F, 0x1F, 0x1C, 0x43, 0x1, 0x1F, 0x17, 0x1, 0x1F, 0x11, 0x44, 0x12, 0x1, 0x1F, 0x1F, 0x19, 0x41, 0x15, 0x1, 0x1F, 0x1F, 0x18, 0x42, 0x14, 0x2, 0x1F, 0x1F, 0x18, 0x41, 0x14, 0x3, 0x1F, 0x1F, 0x18, 0x41, 0x13, 0x4, 0x1F, 0x1F, 0x17, 0x42, 0x12, 0x5, 0x1F, 0x1F, 0x17, 0x41, 0x12, 0x6, 0x1F, 0x1F, 0x17, 0x41, 0x11, 0x7, 0x1F, 0x1F, 0x17, 0x41, 
0x11, 0x7, 0x1F, 0x1F, 0x17, 0x41, 0x8, 0x1F, 0x1F, 0x17, 0x41, 0x71, 0x6, 0x31, 

0xF, 0x0, 0x2C, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x21, 0x2, 0x41, 0x81, 0x41, 0x81, 0x41, 0x81, 0x41, 0x2, 0x1F, 0x1F, 0x14, 0x2, 0x11, 0x41, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x1F, 0x1F, 0x14, 0x1, 0x11, 0x1, 0x41, 0x51, 0x41, 0x51, 0x41, 0x51, 0x41, 0x2, 0x51, 0x1F, 0x1F, 0x14, 0x2, 0x17, 0x2, 0x92, 0x1F, 0x1F, 0x12, 0x44, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x1F, 0x1F, 0x14, 0x71, 0x12, 0x47, 0x52, 0x41, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1E, 0x2, 0x4F, 0x4F, 0x44, 0x1A, 0x2, 0x1F, 0x1F, 0x1E, 0x2, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 
0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x18, 0x31, 0x1F, 0x1F, 0x1F, 0x10, 

0xF, 0x0, 0x2C, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x21, 0x2, 0x41, 0x81, 0x41, 0x81, 0x41, 0x81, 0x41, 0x2, 0x11, 0x41, 0x13, 0x41, 0x6, 0x11, 0x81, 0x1F, 0x15, 0x2, 0x11, 0x41, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x11, 0x41, 0x11, 0x41, 0x11, 0x41, 0x6, 0x11, 0x81, 0x1F, 0x15, 0x1, 0x11, 0x1, 0x41, 0x51, 0x41, 0x51, 0x41, 0x51, 0x41, 0x2, 0x51, 0x41, 0x11, 0x41, 0x11, 0x41, 0x6, 0x11, 0x51, 0x1F, 0x16, 0x2, 0x17, 0x2, 0x11, 0x92, 0x41, 0x11, 0x41, 0x6, 0x1F, 0x17, 0x44, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x44, 0x11, 0x41, 0x6, 0x11, 0x1, 0x1F, 0x15, 0x31, 0x12, 0x47, 0x52, 0x41, 0x14, 0x42, 0x5, 0x11, 0x1, 0x1F, 0x1B, 0x41, 0x15, 0x5, 0x51, 0x41, 0x5, 0x11, 0x1, 0x1F, 0x1B, 0x41, 0x12, 0x41, 0x2, 0x45, 0x51, 0x47, 0x1, 0x4F, 0x45, 0x16, 0x41, 0x12, 0x41, 0x18, 0x8, 0x41, 
0x11, 0x41, 0x1F, 0x18, 0x41, 0x12, 0x41, 0x12, 0x45, 0x9, 0x41, 0x1, 0x41, 0x1F, 0x18, 0x41, 0x12, 0x41, 0x12, 0x41, 0x91, 0xC, 0x41, 0x1, 0x41, 0x1F, 0x18, 0x41, 0x12, 0x41, 0x12, 0x41, 0x92, 0xB, 0x41, 0x1, 0x41, 0x1F, 0x18, 0x41, 0x12, 0x41, 0x12, 0x41, 0x93, 0xA, 0x41, 0x1, 0x41, 0x1F, 0x18, 0x41, 0x15, 0x41, 0x94, 0x9, 0x41, 0x1, 0x41, 0x1F, 0x18, 0x4F, 0x46, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x14, 

0xF, 0x0, 0x2C, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x3, 0x41, 0x81, 0x41, 0x81, 0x41, 0x81, 0x41, 0x2, 0x11, 0x41, 0x13, 0x41, 0x92, 0x11, 0x3, 0x11, 0x81, 0x1A, 0x51, 0x12, 0x51, 0x12, 0x51, 0x11, 0x41, 0x51, 0x2, 0x11, 0x41, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x11, 0x41, 0x11, 0x41, 0x11, 0x41, 0x91, 0x11, 0x4, 0x11, 0x81, 0x1F, 0x13, 0x41, 0x51, 0x1, 0x11, 0x1, 0x41, 0x51, 0x41, 0x51, 0x41, 0x51, 0x41, 0x2, 0x51, 0x41, 0x11, 0x41, 0x11, 0x41, 0x11, 0x5, 0x11, 0x51, 0x15, 0x51, 0x1C, 0x41, 0x51, 0x11, 0x2, 0x17, 0x2, 0x11, 0x92, 0x41, 0x11, 0x41, 0x6, 0x13, 0x2, 0x1F, 0x41, 0x51, 0x44, 0x1, 0x41, 0x1, 0x41, 0x1, 0x41, 0x2, 0x44, 0x11, 0x41, 0x6, 0x11, 0x1, 0x13, 0x51, 0x1E, 0x41, 0x51, 0x31, 0x12, 0x41, 0x51, 0x45, 0x52, 0x41, 0x14, 0x42, 0x5, 0x11, 0x1, 0x1F, 0x13, 0x41, 0x51, 
0x16, 0x41, 0x15, 0x5, 0x51, 0x41, 0x5, 0x11, 0x1, 0x1F, 0x13, 0x41, 0x51, 0x14, 0x1, 0x11, 0x41, 0x12, 0x41, 0x2, 0x45, 0x51, 0x47, 0x1, 0x4F, 0x44, 0x51, 0x14, 0x1, 0x11, 0x41, 0x12, 0x41, 0x18, 0x8, 0x41, 0x1F, 0x12, 0x52, 0x16, 0x41, 0x12, 0x41, 0x12, 0x45, 0x9, 0x41, 0x1F, 0x12, 0x52, 0x15, 0x1, 0x41, 0x12, 0x41, 0x12, 0x41, 0x91, 0x41, 0x1, 0x51, 0x9, 0x41, 0x1F, 0x12, 0x51, 0x16, 0x1, 0x41, 0x12, 0x41, 0x12, 0x41, 0x91, 0x41, 0x1, 0x41, 0x1, 0x11, 0x7, 0x41, 0x1F, 0x11, 0x1, 0x11, 0x1, 0x16, 0x41, 0x12, 0x41, 0x12, 0x41, 0x91, 0x41, 0x1, 0x41, 0x1, 0x12, 0x6, 0x41, 0x1F, 0x1A, 0x41, 0x15, 0x41, 0x91, 0x41, 0x1, 0x41, 0x14, 0x5, 0x41, 0x1F, 0x1A, 0x47, 0x51, 0x41, 0x1, 0x4B, 0x1F, 0x1F, 0x13, 0x41, 0x1, 0x1F, 0x1F, 0x1E, 0x41, 
0x1, 0x4B, 0x1F, 0x1E, 0x42, 0x51, 0x44, 0x5, 0x41, 0x3, 0x41, 0x1F, 0x1E, 0x41, 0x1, 0x11, 0x1, 0x51, 0x5, 0x52, 0x81, 0x3, 0x41, 0x1F, 0x1E, 0x41, 0x1, 0x41, 0x1, 0x41, 0x11, 0x41, 0x55, 0x45, 0x1F, 0x1E, 0x41, 0x1, 0x41, 0x51, 0x41, 0x11, 0x41, 0x55, 0x41, 0x91, 0x4, 0x1F, 0x1D, 0x41, 0x2, 0x11, 0x41, 0x11, 0x41, 0x55, 0x41, 0x92, 0x3, 0x1F, 0x1D, 0x43, 0x11, 0x41, 0x11, 0x42, 0x51, 0x48, 0x1F, 0x1F, 0x11, 0x41, 0x11, 0x41, 0x17, 0x4, 0x41, 0x1F, 0x1F, 0x11, 0x41, 0x11, 0x44, 0x1, 0x46, 0x1, 0x41, 0x1F, 0x14, 0x1, 0x11, 0x53, 0x11, 0x1, 0x15, 0x41, 0x3, 0x41, 0x7, 0x41, 0x1, 0x41, 0x1F, 0x14, 0x1, 0x12, 0x52, 0x17, 0x41, 0x1, 0x41, 0x1, 0x41, 0x7, 0x41, 0x1, 0x41, 0x1F, 0x14, 0x1, 0x12, 0x52, 0x11, 0x1, 0x15, 0x41, 0x1, 0x41, 
0x1, 0x41, 0x7, 0x41, 0x1, 0x41, 0x19, 0x51, 0x19, 0x1, 0x12, 0x71, 0x18, 0x43, 0x1, 0x41, 0x7, 0x41, 0x1, 0x49, 0x11, 0x51, 0x19, 0x3, 0x1C, 0x1, 0x41, 0x7, 0x41, 0x21, 0x8, 0x1C, 


};

uint8_t tiles[128] {			//RAM used for animated game tiles

//0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa, 0x51, 0xaa, 0x55, 0xaa, 0x15, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x18, 0x84, 0xe4, 0x3f, 0x3f, 0xe4, 0x84, 0x18, 0x77, 0x07, 0x77, 0x70, 0x77, 0x07, 0x77, 0x70, 0x1c, 0x32, 0x7d, 0x7d, 0x6f, 0x3e, 0x1c, 0x00, 0x3a, 0x7d, 0x75, 0x45, 0x75, 0x7d, 0x3a, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x63, 0x7f, 0x00, 0x00, 0x42, 0x3c, 0x24, 0x24, 0x3c, 0x42, 0x00, 0x00, 0x18, 0x66, 0x81, 0x81, 0x66, 0x18, 0x00, 0xf9, 0xff, 0x6f, 0x46, 0x6e, 0x7f, 0xff, 0xdd, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, 0x7c, 0xc0, 0x98, 0x2c, 0x34, 0x19, 0x03, 0x3e, 0x00, 0x10, 0x14, 0x68, 0x16, 0x28, 0x08, 0x00, 0x58, 0x82, 0x09, 0x20, 0x04, 0x91, 0x81, 0x6a, 0x08, 0x80, 0x01, 0x00, 0x00, 0x01, 0x80, 0x04, 


0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa, 0x51, 0xaa, 0x55, 0xaa, 0x15, 0xaa, 0x18, 0x84, 0xe4, 0x3f, 0x3f, 0xe4, 0x84, 0x18, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x63, 0x7f, 0x00, 0x77, 0x07, 0x77, 0x70, 0x77, 0x07, 0x77, 0x70, 0x1c, 0x32, 0x7d, 0x7d, 0x6f, 0x3e, 0x1c, 0x00, 0x00, 0x0e, 0x03, 0xb1, 0xb1, 0x1b, 0x0e, 0x00, 0x3a, 0x7d, 0x75, 0x45, 0x75, 0x7d, 0x3a, 0x00, 0x00, 0x42, 0x3c, 0x24, 0x24, 0x3c, 0x42, 0x00, 0x00, 0x18, 0x66, 0x81, 0x81, 0x66, 0x18, 0x00, 0xf9, 0xff, 0x6f, 0x46, 0x6e, 0x7f, 0xff, 0xdd, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, 0x7c, 0xc0, 0x98, 0x2c, 0x34, 0x19, 0x03, 0x3e, 0x00, 0x10, 0x14, 0x68, 0x16, 0x28, 0x08, 0x00, 0x58, 0x82, 0x09, 0x20, 0x04, 0x91, 0x81, 0x6a, 0x08, 0x80, 0x01, 0x00, 0x00, 0x01, 0x80, 0x04, 

};

const uint8_t font[] PROGMEM = {    //The text font

0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xde, 0xde, 0x00, 0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x1e, 0x1e, 0x00, 0x00, 0x6c, 0xfe, 0xfe, 0x6c, 0xfe, 0xfe, 0x6c, 0x00, 0x48, 0x5c, 0x54, 0xfe, 0x54, 0x74, 0x24, 0x00, 0xc6, 0xe6, 0x70, 0x38, 0x1c, 0xce, 0xc6, 0x00, 0x60, 0xf4, 0x9e, 0xba, 0x6e, 0xe4, 0xa0, 0x00, 0x20, 0x18, 0x18, 0x0c, 0x18, 0x18, 0x20, 0x00, 0x00, 0x00, 0x7c, 0xfe, 0xc6, 0x82, 0x00, 0x00, 0x00, 0x82, 0xc6, 0xfe, 0x7c, 0x00, 0x00, 0x10, 0x54, 0x7c, 0x38, 0x38, 0x7c, 0x54, 0x10, 0x00, 0x18, 0x18, 0x7e, 0x7e, 0x18, 0x18, 0x00, 0x00, 0x00, 0x80, 0xe0, 0xe0, 0x60, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x04, 0x00, 0x00, 0x7c, 0xfe, 0xfe, 0xfe, 0x82, 0xfe, 0x7c, 0x00, 0x00, 0x0c, 0xfc, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0xc4, 0xe6, 0xf2, 0xde, 0xde, 0xde, 0xcc, 0x00, 0x44, 0xd6, 0x92, 0xfe, 0xfe, 0xfe, 0x7c, 0x00, 0x3e, 0x3e, 0x30, 0x3e, 0xfe, 0xfe, 0x30, 0x00, 0x5e, 0xde, 0xda, 0xda, 0xda, 0xfa, 0x72, 0x00, 0x7c, 0xfe, 0x8a, 0xfa, 0xfa, 0xfa, 0x72, 0x00, 0x02, 0x02, 0xf2, 0xfa, 0xfe, 0x0e, 0x06, 0x00, 0x6c, 0xfe, 0x92, 0xfe, 0xfe, 0xfe, 0x6c, 0x00, 0x1c, 0xbe, 0xa2, 0xfe, 0xfe, 0xfe, 0x7c, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x80, 0xec, 0xec, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x10, 0x38, 0x7c, 0xfe, 0x00, 0x00, 0x00, 0x6c, 0x6c, 0x6c, 0x6c, 0x6c, 0x6c, 0x00, 0x00, 0x00, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x0c, 0x06, 0xd2, 0xda, 0xde, 0x0e, 0x04, 0x00, 0x7c, 0xfe, 0xc6, 0xba, 0xaa, 0xb6, 0xbc, 0x00, 0xfc, 0xfe, 0x12, 0xfe, 0xfe, 0xfe, 0xfc, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x92, 0xfe, 0x6c, 0x00, 0x7c, 0xfe, 0xfe, 0xfe, 0x82, 0xc6, 0x44, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x82, 0xfe, 0x7c, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x92, 0x92, 0x82, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x12, 0x12, 0x02, 0x00, 0x7c, 0xfe, 0xfe, 0xfe, 0x82, 0xa6, 0xe4, 0x00, 0xfe, 0xfe, 0x10, 0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x82, 0xfe, 0xfe, 0xfe, 0xfe, 0x82, 0x00, 0x00, 0x40, 0xc2, 0x82, 0xfe, 0xfe, 0x7e, 0x02, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x38, 0xee, 0xc6, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x80, 0x80, 0x80, 0x00, 0xfe, 0xfe, 0x1e, 0xfc, 0x1e, 0xfe, 0xfe, 0x00, 0xfe, 0xfe, 0xfe, 0x1c, 0x38, 0xfe, 0xfe, 0x00, 0x7c, 0xfe, 0x82, 0xfe, 0xfe, 0xfe, 0x7c, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x22, 0x3e, 0x1c, 0x00, 0x7c, 0xfe, 0xa2, 0xfe, 0xfe, 0xfe, 0x7c, 0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0x22, 0xfe, 0xdc, 0x00, 0x4c, 0xde, 0x9e, 0xfe, 0xf2, 0xf6, 0x64, 0x00, 0x02, 0x02, 0xfe, 0xfe, 0xfe, 0x02, 0x02, 0x00, 0x7e, 0xfe, 0x80, 0xfe, 0xfe, 0xfe, 0x7e, 0x00, 0x3e, 0x7e, 0xe0, 0xfe, 0xfe, 0x7e, 0x3e, 0x00, 0x7e, 0xfe, 0xe0, 0x7c, 0xe0, 0xfe, 0x7e, 0x00, 0xc6, 0xee, 0x7c, 0x38, 0x7c, 0xee, 0xc6, 0x00, 0x0e, 0xfe, 0xfe, 0xf8, 0xfe, 0xfe, 0x0e, 0x00, 0xe2, 0xf2, 0xfa, 0xfe, 0xbe, 0x9e, 0x8e, 0x00, 0x00, 0x00, 0x10, 0x38, 0x7c, 0xfe, 0x00, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0x81, 0xc3, 0x00, 0xfe, 0x7c, 0x38, 0x10, 0x00, 0x00, 0x00, 0x00, 0x24, 0x66, 0xe7, 0xe7, 0x66, 0x24, 0x00, 0x00, 0x18, 0x3c, 0x7e, 0x7e, 0x3c, 0x18, 0x00, 

};

const uint8_t aniTiles[] PROGMEM = {	//The data for animating enemies in the tiles RAM

0x3a, 0x7d, 0x75, 0x45, 0x75, 0x7d, 0x3a, 0x00, 0x00, 0x42, 0x3c, 0x24, 0x24, 0x3c, 0x42, 0x00, 0x00, 0x18, 0x66, 0x81, 0x81, 0x66, 0x18, 0x00, 0xf9, 0xff, 0x6f, 0x46, 0x6e, 0x7b, 0xff, 0xdd, 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81, 0x7c, 0xc0, 0x98, 0x2c, 0x34, 0x19, 0x03, 0x3e, 

0x3a, 0x7d, 0x75, 0x45, 0x76, 0x7d, 0x3a, 0x00, 0x00, 0x00, 0x7e, 0x24, 0x24, 0x7e, 0x00, 0x00, 0x00, 0x18, 0x7e, 0x87, 0x81, 0x66, 0x18, 0x00, 0x78, 0xef, 0xef, 0xfe, 0x7e, 0xfa, 0xff, 0x57, 0x42, 0x66, 0x24, 0x18, 0x18, 0x24, 0x66, 0x42, 0x40, 0x81, 0x9b, 0xad, 0xb5, 0xd9, 0x81, 0x02, 

0x3a, 0x7d, 0x75, 0x45, 0x75, 0x7d, 0x3a, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0x18, 0x66, 0xf1, 0x8f, 0x66, 0x18, 0x00, 0xf9, 0xef, 0x7f, 0x7a, 0x7a, 0x51, 0xfb, 0xdd, 0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x08, 0x06, 0x03, 0x99, 0xb5, 0xad, 0x99, 0xc0, 0x60, 

0x3a, 0x7d, 0x76, 0x45, 0x75, 0x7d, 0x3a, 0x00, 0x00, 0x00, 0x7e, 0x24, 0x24, 0x7e, 0x00, 0x00, 0x00, 0x18, 0x66, 0x81, 0xe1, 0x7e, 0x18, 0x00, 0x52, 0xfd, 0x7f, 0xee, 0xfa, 0xf1, 0x7b, 0x9d, 0x42, 0x66, 0x24, 0x18, 0x18, 0x24, 0x66, 0x42, 0x7e, 0x21, 0x18, 0x34, 0x2c, 0x18, 0x84, 0x7e, 
	
};

const uint8_t mani[] PROGMEM = {	//The data for animatin the main character
	
0x18, 0x84, 0xe4, 0x3f, 0x3f, 0xe4, 0x84, 0x18, 0x10, 0x08, 0x04, 0xff, 0x87, 0x08, 0x08, 0x00, 0x00, 0xd8, 0xa4, 0x1f, 0x2b, 0xc8, 0x80, 0x00, 0xc0, 0xa0, 0x1c, 0x1f, 0x1b, 0x20, 0xc0, 0x80, 0x00, 0x83, 0xe4, 0x1f, 0x1f, 0x24, 0x24, 0x00, 0x00, 0x42, 0x64, 0x1f, 0x1f, 0x64, 0x42, 0x00, 0x00, 0x24, 0x24, 0x1f, 0x1f, 0xe4, 0x83, 0x00, 0x10, 0x88, 0xe4, 0x1f, 0x1f, 0x34, 0x24, 0x00, 0x00, 0x48, 0x64, 0x1f, 0x1f, 0x64, 0x48, 0x00, 0x00, 0x24, 0x34, 0x1f, 0x1f, 0xe4, 0x88, 0x10, 0x40, 0x20, 0x40, 0x3f, 0x1b, 0x28, 0xc8, 0x88, 0x00, 0x00, 0x00, 0xfc, 0xbf, 0x8b, 0x08, 0x0c, 0xc0, 0xa0, 0x10, 0x1c, 0x1f, 0x6b, 0x48, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 

};

const uint8_t doori[] PROGMEM = {0x00, 0x7f, 0x41, 0x41, 0x41, 0x41, 0x7f, 0x00, 0x00, 0x7f, 0x7f, 0x7f, 0x7f, 0x63, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const char hexValue[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

const uint8_t title[] PROGMEM = {
0x00, 0xff, 0x01, 0x01, 0x81, 0xc1, 0xe1, 0xf1, 0x31, 0x19, 0x19, 0x09, 0x09, 0x09, 0x19, 0x39, 0x01, 0x01, 0x01, 0x81, 0xe1, 0x39, 0x39, 0xf1, 0xc1, 0x01, 0x09, 0xf9, 0xf9, 0x09, 0x09, 0x99, 0xf9, 0xf9, 0x71, 0x01, 0x09, 0xf9, 0xf9, 0x09, 0x09, 0x99, 0xf9, 0x71, 0x01, 0x01, 0x01, 0x81, 0xe1, 0x39, 0x39, 0xf9, 0xc1, 0x01, 0x01, 0xc1, 0xe1, 0xf1, 0x31, 0x19, 0x09, 0x09, 0x09, 0x19, 0x31, 0x01, 0x09, 0xf9, 0xf9, 0x89, 0xc9, 0x19, 0x01, 0x01, 0x01, 0x01, 0x09, 0xf9, 0xf9, 0x79, 0xf1, 0xc1, 0x81, 0xe1, 0x39, 0xf9, 0xf9, 0x09, 0x01, 0x09, 0xf9, 0xf9, 0x09, 0x01, 0x09, 0xf9, 0xf9, 0x79, 0xf1, 0xe1, 0x09, 0xf9, 0xf9, 0x09, 0x01, 0x09, 0xf9, 0xf9, 0x89, 0xc9, 0x19, 0x01, 0x09, 0xf9, 0xf9, 0xf9, 0x09, 0x09, 0x09, 0x19, 0xf9, 0xf1, 0xe1, 0x01, 0x01, 0x01, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x0f, 0x3f, 0x7f, 0x70, 0xc0, 0xc0, 0x80, 0x82, 0xc2, 0xfe, 0xfe, 0x7e, 0x02, 0x18, 0x1e, 0x17, 0x11, 0x01, 0x01, 0x13, 0x1f, 0x1e, 0x18, 0x1f, 0x1f, 0x11, 0x03, 0x0f, 0x1f, 0x1e, 0x1c, 0x10, 0x10, 0x1f, 0x1f, 0x11, 0x11, 0x11, 0x1f, 0x0e, 0x14, 0x18, 0x1e, 0x17, 0x11, 0x01, 0x01, 0x13, 0x1f, 0x1f, 0x18, 0x13, 0x07, 0x0f, 0x1c, 0x18, 0x10, 0x11, 0x1f, 0x1f, 0x0f, 0x01, 0x10, 0x1f, 0x1f, 0x10, 0x11, 0x10, 0x18, 0x00, 0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x01, 0x0f, 0x07, 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x03, 0x0f, 0x1e, 0x1f, 0x1f, 0x00, 0x00, 0x10, 0x1f, 0x1f, 0x10, 0x11, 0x10, 0x18, 0x80, 0xff, 0xff, 0xff, 0x84, 0x0c, 0x3c, 0x7e, 0xff, 0xf3, 0xc1, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8e, 0xcf, 0xcf, 0xc7, 0xc7, 0xe7, 0xe7, 0xe7, 0xe7, 0x67, 0x77, 0x7f, 0x63, 0x3e, 0x00, 0xfe, 0xfe, 0xff, 0x0f, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0f, 0xff, 0x02, 0xfe, 0x00, 0xe0, 0xf0, 0xf8, 0xf8, 0xbc, 0x9e, 0x9e, 0x8f, 0x87, 0x87, 0xff, 0x01, 0xff, 0x80, 0x80, 0x00, 0x3e, 0x7e, 0x7f, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0x67, 0xff, 0x06, 0xfc, 0x00, 0x80, 0x40, 0x40, 0x40, 0x40, 0x80, 0x00, 0xc0, 0x80, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x7f, 0x40, 0x40, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x40, 0x40, 0x40, 0x4f, 0x4f, 0x4b, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4e, 0x40, 0x47, 0x47, 0x4f, 0x4b, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4b, 0x4f, 0x44, 0x47, 0x40, 0x43, 0x43, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x4f, 0x48, 0x4f, 0x43, 0x43, 0x40, 0x42, 0x4e, 0x4e, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4a, 0x4f, 0x46, 0x43, 0x40, 0x47, 0x49, 0x49, 0x49, 0x49, 0x49, 0x40, 0x4f, 0x40, 0x40, 0x40, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x48, 0x44, 0x40, 0x40, 0x7f, 0x00, 
};

//Arduboy system specific variables
uint16_t frameCount;
uint8_t eachFrameMillis;
unsigned long lastFrameStart;
unsigned long nextFrameStart;
bool justRendered;
uint8_t lastFrameDurationMs;

//Game world RAM
uint8_t tileMap[48][32];                         //1 byte per tile, (2) screens at 16x8 each
uint8_t scratch[16];;			//Scratch line of RAM. Used for the edit menu and status bar (when active)

//Player movement and positioning
int8_t winX = 0;              //X window position
int8_t winY = 0;              //Y window position
int8_t winXFine = 0;
int8_t winYFine = 0; 
uint8_t scrollLeft = 0;
uint8_t scrollRight = 0;
uint8_t scrollUp = 0;
uint8_t scrollDown = 0;
uint8_t scrollFlag = 0;
uint8_t doorX;					//XY position of exit door
uint8_t doorY;
uint8_t manStartX;				//XY position of where man starts
uint8_t manStartY;
uint8_t x;						//General-purpose X Y registers
uint8_t y;
uint8_t pX = 8;                 //Player XY position
uint8_t pY = 4;
uint8_t manDir = 0;				//Which direction the man is facing
uint8_t manFrame = 0;			//Which frame of man animation we're on
uint8_t push = 0;				//Counter for pushing rocks
uint8_t playerState = 0;		//0 = waiting for spawn 1 = alive 2 = dead
uint16_t screenTimer = 0;		//Timer for changing screen events

uint8_t godMode = 0;			//Set to 1 and player can't be killed (debug)
uint8_t debug = 0;				//Enable debug mode (default = off)
uint8_t editMode = 0;			//If we're editing or not (changes the action of B button during game)

int g;							//A stock variable
int gg;							//A stock variable

uint16_t ePoint;				//Current pointer for EEPROM access. Must start at 16 or higher because of Arduboy reserved functions
uint8_t eeByte;					//Used for sending bytes to and fro EEPROM
uint8_t *ringPt[8];				//Pointer to the bytes that surround the current tile we're evaluating
uint8_t eventCounter = 0;       //When to trigger events such as player movement and world physics
uint8_t aniPointer = 0;			//Pointer for current block of tile animation. 40 byte increments, max 120 (160 trigger reset back to 0)
uint8_t rollToggle = 0;			//Which way balls should roll off each other this frame
uint8_t slimeExist = 0;			//Scan the tiles and see if slime exists before advancing the slime timer

uint8_t debounce = 0;

//World object variables
uint8_t slimeTimer = 0;			//How many slime tiles 
uint8_t slimeGrow = 250;
uint8_t slimeExpand = 0;		//When slime expands, on which side?
uint16_t gemsNeeded = 50;
uint16_t gemsCollected = 0;

uint8_t sysState = game;
uint8_t dirtSound = 100;

uint8_t *cP;         //Pointer to character data

//RGB LED variables
uint8_t red = 51;
uint8_t green = 0;
uint8_t blue = 0;
uint8_t colorFrame = 0;
uint8_t flash = 0;

uint8_t dPadBounceEnable = 0xFC;		//At the start all buttons require re-trigger
uint8_t dPadBounce = 0;			

//Editor variables
int8_t tilePlace = 0;			//The tile to place
uint8_t tileBackup;				//The tile we're hovering over
uint8_t menuSelect = 0;			//Which menu to display when holding B (starts at 0)
uint8_t moveRamp = 10;
uint8_t moveRampStart = 10;
uint8_t exportTarget = 0;		//Writing - 0 == EEPROM 1 = Serial Port  Reading - 0 = EEPROM 1 = Indexed flash level data
uint8_t itemsPerLine = 0;
uint16_t totalCount = 0;		//How many bytes of level data were written to the record (includes the 16 byte header)
uint8_t arrows[2];

void setup() {
	
	Serial.begin(115200);

	a.boot();
	eachFrameMillis = 16;     //a.setFrameRate(60);  
	beep.begin(); // initialise the hardware


	pinMode(RED_LED, 1);				//PB6
	pinMode(GREEN_LED, 1);			//PB7
	pinMode(BLUE_LED, 1);				//PB5

	eventCounter = 0;

	walls();

	menuSetup();			//Start in the Menu state
	
}

void loop() {

  if (!(nextFrame())) {         //Lock to 60 FPS
    return;
  } 

  beep.timer(); // handle tone duration  
	
	switch(sysState) {
	
		case(menu):
			mainMenu();
		break;
		case(story):
		
		break;
		case(game):
			gameLoop();
		break;
		case(edit):
			editLoop();
		break;
		case(levelStart):
			levelStartLoop();
		break;
	
	}

}

void menuSetup() {

	fill(0);
	
	text(2,1, "GO LEVEL -00-");
	text(3,2, "ROGUELIKE");
	text(4,3, "EDITOR");

	menuSelect = 0;			//Starts with Option #1

	g = 0;					//Flag to exit menu when button pressed
	
	winX = 0;
	winY = 0;
	winXFine = 0;
	winYFine = 0; 	
	
	dPadBounceEnable = 0xFC;	//Retrigger debounce on all buttons
	
	editMode = 0;
	sysState = menu;
	
}

void mainMenu() {

	drawTitle();		//Draw the 4 line title graphic

	sendTiles(4);		//Then send 4 lines of text (from top of tilemap buffer)
	
	eventCounter++;

	if (eventCounter & 0x08) {
		arrows[0] = '<';
		arrows[1] = '>';		
	}
	else {
		arrows[0] = '[';
		arrows[1] = ']';		
	}
	
	tileMap[11][1] = arrows[0];
	tileMap[14][1] = arrows[1];

	switch(menuSelect) {
	
		case 0:
			tileMap[0][1] = arrows[1];
			tileMap[1][2] = 0;		
			tileMap[2][3] = 0;
			if (level < 10) {
				tileMap[12][1] = '0';
				drawNumberMap(level, 13, 1);
			}
			else {
				drawNumberMap(level, 12, 1);	
			}
			if (dLeft()) {
				if (--level == 0) {
					level = 25;
				}
			}
			if (dRight()) {
				if (++level == 26) {
					level = 1;
				}
			}			
			if (g) {			//Start the game?
					eventCounter = 0;
					lives = 3;
					score = 0;
					levelStartSetup();					
			}
		break;
		
		case 1:
			tileMap[0][1] = 0;
			tileMap[1][2] = arrows[1];		
			tileMap[2][3] = 0;			
		break; 
		
		case 2:
			tileMap[0][1] = 0;
			tileMap[1][2] = 0;		
			tileMap[2][3] = arrows[1];
			if (g) {
				walls();
				editSetup(1);
			}
		break;
	
	}
	
	if (buttonA()) {
		g = 1;
	} 

	if (dUp()) { 		//Navigate menus up and down

		menuSelect--;

		if (menuSelect == 255) {	//Did we roll over below 0?
			menuSelect = 2;
		}

	}
	
	if (dDown()) {

		menuSelect++;

		if (menuSelect == 3) {
			menuSelect = 0;
		}

	}	

}

void editSetup(uint8_t resetCursor) {

	if (resetCursor) {
		pX = 1;
		pY = 1;
		winX = 0;
		winY = 0;
	}
	
	tileBackup = tileMap[pX][pY];
	resetAnimation();
	menuSelect = 0;
	editMode = 1;
	sysState = edit;
	
}

void editLoop() {
	
	sendTiles(7);						//Draw the first 7 rows
	sendMenuLine();						//Draw the menu row #8

	eventCounter++;
	
	if (pressed(B_BUTTON)) {

		if (menuSelect == 0) {			//Did we just arrive? Set menu # and fix cursor tile
			tileMap[pX][pY] = tileBackup;
			menuSelect = 1;
		}
	
	  if (dUp()) { 		//Navigate menus up and down
		
		menuSelect--;		
		if (menuSelect < 1) {
			menuSelect = 10;
		}
		
		beep.tone(beep.freq(100 + (menuSelect * 100)), 20);
		
	  }
	  if (dDown()) {
		
		menuSelect++;		
		if (menuSelect > 10) {
			menuSelect = 1;
		}
		
		beep.tone(beep.freq(100 + (menuSelect * 100)), 20);
		
	  }	
	
		for (int x = 0 ; x < 15 ; x++) {	//Clear the menu display
			scratch[x] = 0;
		}

		if (eventCounter & 0x08) {		//Blink the up/down arrow prompt on lower right		
			scratch[15] = 94;
		}
		else {		
			scratch[15] = 95;		
		}
			
		switch(menuSelect) {
		
			case 1:
			
				if (tilePlace == 0) {
					gg = 12;
				}
				else {
					gg = tilePlace - 1;	
				}

				for (int x = 2 ; x >= 0 ; x--) {
					scratch[x] = gg;
					if (--gg < 0) {
						gg = 12;
					}			
				}

				if (eventCounter & 0x08) {		//Blink the current tile		
					scratch[3] = '<';
					scratch[5] = '>';
				}
				else {		
					scratch[3] = '[';
					scratch[5] = ']';	
				}				
								
				scratch[4] = tilePlace;
				
				if (tilePlace == 12) {
					gg = 0;
				}
				else {
					gg = tilePlace + 1;	
				}		
				
				for (int x = 6 ; x < 9 ; x++) {
					scratch[x] = gg;
					if (++gg > 12) {
						gg = 0;
					}			
				}				
								
				textMenu(10, "TILE");	
			break;
			case 2:
				textMenu(3, "TEST LEVEL");
			break;
			case 3:
				textMenu(3, "SAVE EEPROM");
			break;
			case 4:
				textMenu(3, "LOAD EEPROM");
			break;						
			case 5:
				textMenu(3, "EXPORT UART");
			break;
			case 6:
				textMenu(6, "NEW FILE");
			break;
			case 7:			
				if (eventCounter & 0x08) {		//Blink the current tile		
					textMenu(0, "<    >   GEMS?");
				}
				else {		
					textMenu(0, "[    ]   GEMS?");
				}			
				drawNumberMenu(gemsNeeded, 1);
			break;
			case 8:
				if (eventCounter & 0x08) {		//Blink the current tile		
					textMenu(0, "<   >   SLIME?");
				}
				else {		
					textMenu(0, "[   ]   SLIME?");
				}	
				drawNumberMenu(slimeGrow, 1);
			break;				
			case 9:
				if (eventCounter & 0x08) {		//Blink the current tile		
					textMenu(0, "<  >  LOAD LVL");
				}
				else {		
					textMenu(0, "[  ]  LOAD LVL");
				}			
				drawNumberMenu(level, 1);
			break;
			case 10:
				//drawHexMenu(tileMap[pX][pY], 0);
				textMenu(10, "EXIT");
			break;		
		}
		
		if (dLeft()) {
		
			beep.tone(beep.freq(2000), 10); // 1000Hz for 2 seconds
			
			switch(menuSelect) {
			
				case(1):
					tilePlace--;
					
					if (tilePlace < 0) {
						tilePlace = 12;
					}
				break;
				case(7):				
					if (gemsNeeded > 1) {
						gemsNeeded -= 10;
					}	
				break;
				case(8):
					if (slimeGrow > 10) {
						slimeGrow -= 25;
					}									
				break;				
				case(9):
					if (level > 1) {
						level--;
					}	
				break;	

				
			}

		}
		if (dRight()) {
			
			beep.tone(beep.freq(2500), 10); // 1000Hz for 2 seconds

			switch(menuSelect) {
			
				case(1):
	
					tilePlace++;
					
					if (tilePlace > 12) {
						tilePlace = 0;
					}
												
				break;
				case(7):				
					if (gemsNeeded < 9999) {
						gemsNeeded += 10;
					}	
				break;
				case(8):
					if (slimeGrow < 250) {
						slimeGrow += 25;
					}									
				break;				
				case(9):
					if (level < 20) {
						level++;
					}				
				break;					
			}		
		}							
	}
	else {

		//Check if we released B with anything special selected (immediate action)

		if (menuSelect) {
			
			switch(menuSelect) {			
				case 2:						//Test level
					gameSetup();
					return;
				break;
				case 3:						//Save to EEPROM
					exportTarget = 0;
					ePoint = 16;			//EEPROM offset to start
					compressLevel();
					editSetup(0);
				break;
				case 4:						//Load from EEPROM
					exportTarget = 0;
					ePoint = 16;			//EEPROM offset to start
					decompressLevel();
					editSetup(1);
				break;		
				case 5:
					Serial.println("Level data:");
					itemsPerLine = 120;					//Start this 8 below the limit of 128 so the first 8 items (metadata) appear as their own line
					exportTarget =  1;
					compressLevel();	
					Serial.println("");
					Serial.print("Data pointer:");
					Serial.println(totalCount - 1);
				break;
				case 6:
					walls();
				break;
				case 9:
					exportTarget = 1;
					gg = 0;					//Clear index pointer
					ePoint = 0;				//Clear ePoint pointer
					while (gg < level) {	//Find beginning of level memory in FLASH
						ePoint += levelSize[gg++];	//Add up ePoint with the lengths of all levels in front of the one we want to load
					}
					decompressLevel();
					editSetup(1);
				break;		
				case 10:
					menuSetup();
					return;
				break;
			}
			
			menuSelect = 0;			//Clear flag so we only check this once
			dPadBounce = 0;			//Clear d-pad bounce
			
		}
		
		if (eventCounter & 0x08) {		//Blink the current tile		
			tileMap[pX][pY] = tileBackup;			//Show original tile
			textMenu(0, "A=DROP>X< B=MENU");
		}
		else {		
			tileMap[pX][pY] = 92;			//Show crosshair
			textMenu(0, "A=DROP]X[ B=MENU");
		}
		
		scratch[7] = tilePlace;			//Draw the active tile

		if (buttonA()) {
			
			switch(tilePlace) {
			
				case man:
					if (manStartX) {		//If man start has already been placed, erase the old position and store new in RAM
						tileMap[manStartX][manStartY] = empty;
					}
					manStartX = pX;
					manStartY = pY;
				break;
				case door:
					if (doorX) {		//If door has already been placed, erase the old position and store new in RAM
						tileMap[doorX][doorY] = dirt1;
					}
					doorX = pX;
					doorY = pY;				
				break;
			}

			if (tileBackup != man && tileBackup !=  door) {	//Don't allow tiles to be dropped on the man start or door end tiles
				tileBackup = tilePlace;
				tileMap[pX][pY] = tileBackup;
			}
		
		}
		
		uint8_t moved = 0;
		
		if (moveRamp) {			
			moveRamp--;		
		}
		
		if (moveRamp == 0) {				//Limit movement speed with ramp up when held down
		
			if (pressed(RIGHT_BUTTON)) {

				if (pX < 46) {
					tileMap[pX][pY] = tileBackup;
					pX++;
					tileBackup = tileMap[pX][pY];
					moved = 1;
					
				}

			}
			if (pressed(LEFT_BUTTON)) {

				if (pX > 1) {
				
					tileMap[pX][pY] = tileBackup;
					pX--;
					tileBackup = tileMap[pX][pY];
					moved = 1;
				}		

			}
			if (pressed(UP_BUTTON)) { 

				if (pY > 1) {
				
					tileMap[pX][pY] = tileBackup;
					pY--;
					tileBackup = tileMap[pX][pY];
					moved = 1;
				
				}		
				
			}
			if (pressed(DOWN_BUTTON)) {

				if (pY < 30) {
				
					tileMap[pX][pY] = tileBackup;
					pY++;
					tileBackup = tileMap[pX][pY];
					moved = 1;
				
				}			

			}	

			if (moved) {
				dPadBounce &= 0xF0;			//Clear the debounce on A and B
				moveRamp = moveRampStart--;
				if (moveRampStart < 1) {
					moveRampStart = 1;
				}		
			}
			else {
				moveRampStart = 10;
				moveRamp = moveRampStart;
			}
		
		}
		
		if (pX < winX + 7 and winX > 0) {
			winX--;
		}

		if (pX > winX + 7 and winX < 32) {
			winX++;
		}

		if (pY < winY + 3 and winY > 0) {
			winY--;
		}

		if (pY > winY + 3 and winY < 25) {
			winY++;
		}		

	}
	
}

void levelStartSetup() {
	
	fill(0);							//Clear display
	
	text(5,0, "LEVEL");
	drawNumberMap(level, 7, 1);
	
	text(5,3, "LIVES");
	drawNumberMap(lives, 7, 4);	
	
	text(5,6, "SCORE");
	drawNumberMap(score, 7, 7);

	eventCounter = 120;				//2 seconds of this
	
	winX = 0;
	winY = 0;
	winXFine = 0;
	winYFine = 0; 	
	
	sysState = levelStart;
	
}

void levelStartLoop() {

	sendTiles(8);		//Send the screen data

	if (--eventCounter == 0) {
		
		walls();				//Draw the walls
		
		exportTarget = 1;
		gg = 0;					//Clear index pointer
		ePoint = 0;				//Clear ePoint pointer
		while (gg < level) {	//Find beginning of level memory in FLASH
			ePoint += levelSize[gg++];	//Add up ePoint with the lengths of all levels in front of the one we want to load
		}
		decompressLevel();	
		gameSetup();
		
	}

}

void gameSetup() {

	pX = manStartX;
	pY = manStartY;
	
	for (int x = 0 ; x < 16 ; x++) {
		scratch[x] = 0;								//Clear scratch RAM
	}
	
	for (int x = 16 ; x < 24 ; x++) {				//Erase man graphics so we can scroll him in later
		tiles[x] = 0;
	}	
				
	tileMap[pX][pY] = man;			//Place man (though it will start as blank since we just erased that memory)	
	manDir = 0;
	manFrame = 16;					//Use this to animate the door closing
	
	winXFine = 0;
	winYFine = 0; 	
	winX = doorX - 7;				//Center window on door
	winY = doorY - 3;

	for (int x = 0 ; x < 8 ; x++) {				//Erase door graphic
		tiles[24 + x] = 0;
	}	
	
	//Try to start the window with the door as centered as possible (unless it's on an edge)
	
	if (winX < 0) {
		winX = 0;
	}
	
	if (winX > 32) {
		winX = 32;
	}
	
	if (winY < 0) {
		winY = 0;
	}
	
	if (winY > 24) {
		winY = 24;
	}

	eventCounter = 0;
	gemsCollected = 0;
	red = gemsNeeded;
	
	playerState = 0;			//Player not spawned
	screenTimer = 0;			//Timer to move camera
	aniPointer = 0;
	
	sysState = game;
	
}

void gameLoop() {			//The main game loop
	
	if (flash) {
		PORTB &= 0x1F;				//White LED flash		
		if (--flash == 0) {
			
			if (gemsCollected >= gemsNeeded) {
				PORTB &= 0x1F;			//Switch to green right away
				PORTB |= 0x60;			//Other colors off
				colorFrame = 0;			
			}
			else {
				PORTB |= 0xE0;			//All colors off
				colorFrame = 0;				
			}			

		}
		
	}
	else {

		if (gemsCollected >= gemsNeeded) {
			if (++colorFrame == slimeGrow) {	//Turn off the LED's when value reached
				PORTB ^= (1 << 7);
				PORTB |= (1 << 6);
				colorFrame = 0;
			}				
		}
		else {
			if (++colorFrame == red) {	//Turn off the LED's when value reached
				PORTB ^= (1 << 6);
				colorFrame = 0;
			}				
		}
	}

  switch(eventCounter++) {      //We divide up frame time by 8

    case 1:						//Player movement state
      moveGuy();
    break;
	
	case 2:						//World object animation state
	
		for (int x = 0 ; x < 48 ; x++) {				//Copy 40 bytes from flash pointer to tile RAM
			tiles[56 + x] = pgm_read_byte_near(aniTiles + aniPointer + x);
		}
		aniPointer += 48;
		
		if (aniPointer == 192) {
			aniPointer = 0;
		}
	
	break;
	
	case 3:						//Man animation state
	
		if (manDir) {

			uint8_t manPointer;

			switch(manDir) {
			
				case 1:	//Up?
				
				if (manFrame == 3) {
						manPointer = 40;
				}
				else {
					manPointer = 32 + (manFrame << 3);
				}
			
				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + x);
				}
	
				break;
				case 2:	//Down?
				
				if (manFrame == 3) {
						manPointer = 64;
				}
				else {
					manPointer = 56 + (manFrame << 3);
				}
			
				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + x);
				}				
				
				break;			
				case 3:	//Left?
				
				if (manFrame == 3) {
						manPointer = 16;
				}
				else {
					manPointer = 8 + (manFrame << 3);
				}
			
				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + (7 - x));		//Mirror the image as we load it in
				}				
				
				break;		
				case 4:	//Right?
				
				if (manFrame == 3) {
						manPointer = 16;
				}
				else {
					manPointer = 8 + (manFrame << 3);
				}
			
				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + x);
				}					
				
				break;		
				case 5:	//Pushing left?
				
				if (manFrame > 2) {
					manFrame = 0;
				}
				
				manPointer = 80 + (manFrame << 3);

				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + (7 - x));		//Mirror the image as we load it in
				}	

	
				
				break;		
				case 6:	//Pushing right?
				
				if (manFrame > 2) {
					manFrame = 0;
				}				

				manPointer = 80 + (manFrame << 3);

				for (int x = 0 ; x < 8 ; x++) {				//Man just standing there
					tiles[16 + x] = pgm_read_byte_near(mani + manPointer + x);
				}					
				
				break;				
			
			}
			
			if (++manFrame == 4) {
				manFrame = 0;
			}
		
		}
		else {

			switch(playerState) {
			
				case 2:
					for (int x = 0 ; x < 8 ; x++) {				//Man emerges from ground (bitshifting)
						tiles[16 + x] = pgm_read_byte_near(mani + x) << screenTimer;
					}	

					if (--screenTimer == 0) {					//Done emerging? Give player control
						playerState = 3;
						manFrame = 0;
					}
			
				break;
		
				case 3:

					for (int x = 0 ; x < 8 ; x++) {				//Man in active gameplay
						tiles[16 + x] = pgm_read_byte_near(mani + x);
					}					
				
				break;
	
			}
		
		}

	break;
	
	case 4:						//Door animation state
	
		if (playerState == 0) {

			g = 8;
			
			if (manFrame == 4) { //Showing the door location? Animate it closing
						
				g = 7;
				
				manFrame = 0;
			
			}	
			
			if (manFrame > 6) { //Showing the door location? Animate it closing
						
				g = manFrame;
				
				manFrame -= 4;
			
			}
			
			for (int x = 0 ; x < 8 ; x++) {		//Update door graphic in RAM
				tiles[24 + x] = pgm_read_byte_near(doori + g + x);
			}
			
		}
		
		if (gemsCollected >= gemsNeeded and playerState != 5) {		//Ready to exit? (but player hasn't exited yet)
			
			g = (screenTimer++ & 1) << 3;		//Blink the graphic

			for (int x = 0 ; x < 8 ; x++) {		//Update door graphic in RAM
				tiles[24 + x] = pgm_read_byte_near(doori + g + x);
			}					

		}
		
	break;

    case 7:						//Do gravity physics and enemy movement
      physics();
      eventCounter = 0;
    break;
 
  }
  
  if (buttonA()) {				//Toggle the info panel
    debug++;
  }  

	if (playerState == 0) {				//Showing door?

		if (screenTimer++ == 40) {
			playerState = 1;			
		}
	
	}
	else {

		scrollFlag = 0;				//Flag that's set if scrolling happens

		if (scrollLeft) {
			scrollLeft--;
			scrollFlag = 1;
			if (--winXFine < 0) {
			  winXFine += 8;
			  winX--;
			}
		}

		if (scrollRight) {
			scrollRight--;
			scrollFlag = 1;
			if (++winXFine > 7) {
			  winXFine -= 8;
			  winX++;
			}
		}

		if (scrollUp) {
			scrollUp--;
			scrollFlag = 1;
			if (--winYFine < 0) {
			  winYFine += 8;
			  winY--;
			}
		}

		if (scrollDown) {
			scrollDown--;
			scrollFlag = 1;
			if (++winYFine > 7) {
			  winYFine -= 8;
			  winY++;
			}
		}

		if (scrollFlag == 0 and playerState == 1) { //Scrolling map to reveal player, and scrolling is complete?

			manFrame = 0;
			playerState = 2;				//Player will now appear from the ground (rise from a grave?)
			screenTimer = 8;				//Used as a bit shifter

		}

	}

	if (debug & 1) {								//Draw debugger display?
		drawNumberMenu(gemsCollected, 0);
    drawNumberMenu(cpuLoad(), 7);
		drawNumberMenu(playerState, 15);
		scrollTiles(7);
		sendMenuLine();
	}
	else {										//Else normal
		scrollTiles(8);
	}

	//Only do this if slime exists
	slimeExpand++;

	if (pressed(B_BUTTON)) { 

		if (editMode) {
			
			while(pressed(B_BUTTON)) {}

			stripAttributes();
			tileMap[pX][pY] = 0;					//Erase current player position	
			tileMap[manStartX][manStartY] = man;	//Redraw man in original position
			editSetup(1);
		
			//tileMap[manStartX][manStartY] = man;
			//sysState = edit;			
		}
		else {
			if (playerState == 3) {
				killGuy();		//Give up!
			}
		}

	}

	if (playerState == 4) {		//Player exploded?

		if (--screenTimer == 0) {
			
			if (editMode) {
				stripAttributes();
				tileMap[pX][pY] = 0;					//Erase current player position	
				tileMap[manStartX][manStartY] = man;	//Redraw man in original position
				editSetup(1);	
			}
			else {
				if (--lives) {
					levelStartSetup();
				}
				else {
					menuSetup();			//Game ovah
				}				
			}
		}
				
	}
	
	if (playerState == 5) {		//Player escaped?

		tileMap[doorX][doorY] = door;	//Redraw door

		if (--screenTimer == 0) { 
			level++;
			levelStartSetup();
		}
					
	}	
}

void moveGuy() {

	if (playerState == 0) {				//Animation of door closing? Don't test for scroll condition
		return;
	}

	//See if window needs to be scrolled to catch up to our guy
	if (pX < winX + 6 and winX > 0) {
		scrollLeft = 8;
	}

	if (pX > winX + 10 and winX < 32) {
		scrollRight = 8;
	}

	if (pY < winY + 3 and winY > 0) {
		scrollUp = 8;
	}

	if (pY > winY + 4 and winY < 24) {
		scrollDown = 8;
	}

	//Get input and move guy  
  
	if (playerState == 3) {				//Is player allowed to move?

	  if (pressed(RIGHT_BUTTON)) {

		if (pX < 46 and eat(pX+1, pY, 4)) {
		  tileMap[pX][pY] = 0;
		  tileMap[++pX][pY] = man;
		  manDir = 4;
		  return;    
		}
		if (manDir == 6) {
			return;
		}
	  }
	  if (pressed(LEFT_BUTTON)) {

		if (pX > 1 and eat(pX-1, pY, 0)) {
		  tileMap[pX][pY] = 0;
		  tileMap[--pX][pY] = man;
		  manDir = 3;
		  return;
		}
		if (manDir == 5) {
			return;
		}		
	  }

	  if (pressed(UP_BUTTON)) { 
		
		if (pY > 1 and eat(pX, pY-1, 2)) {
		  tileMap[pX][pY] = 0;
		  tileMap[pX][--pY] = man;
		  manDir = 1;
		  return;
		}   
	  }
	  if (pressed(DOWN_BUTTON)) {
		
		if (pY < 30 and eat(pX, pY+1, 6)) {
		  tileMap[pX][pY] = 0;
		  tileMap[pX][++pY] = man;
		  manDir = 2;
		  return;
		}   
	  }

	manDir = 0;			//Not moving? Reset man animation
	manFrame = 0;
	push = 0;			//Also reset push power

	}

}

int killGuy() {

	if (!godMode) {	//Unless player is set invincible, blow him up

		explosion(pX, pY, empty);
		playerState = 4;
		screenTimer = 150;			//2.5 seconds of aftermath!
		return 1;
		
	}
	
}	

int eat(uint8_t x, uint8_t y, uint8_t dir) {            //When moving into a tile this processes what's there. Includes escape door

	uint8_t tile = tileMap[x][y] & 0x0F;	//Mask off any control bits


  if (tile == 0) {     //Empty tile
    return 1;
  }
  if (tile < 3) {      //Must be dirt    
	beep.tone(beep.freq(dirtSound), 5); // 1000Hz for 2 seconds    
	dirtSound += 100;
	if (dirtSound > 500) {
		dirtSound = 200;
	}
	return 1;
  }
  if (tile == jem) {   //A gem?
	beep.tone(beep.freq(500), 15); // 1000Hz for 2 seconds
	gemsCollected++;
	flash = 6;
	red -= 1;
	
	if (gemsCollected == gemsNeeded) {
		slimeGrow = 64;		
	}	
	
	if (gemsCollected > gemsNeeded and slimeGrow > 1) {
		slimeGrow--;		
	}
	
    return 1;
  }
  if (tile == ball) {		//A ball rock?

	if (dir == 0) {								//Trying to push left?
		if (tileMap[x-1][y] == 0) {				//Is area to left of rock empty?
			if (++push > pushPower) {
				push = 0;
				tileMap[x-1][y] = ball;			//Move ball and return that player can move left
				tileMap[x][y] = empty;
				manDir = 0;
				return 1;
			}
			else {
				manDir = 5;
			}
		}
	}
	
	if (dir == 4) {								//Trying to push right?
		if (tileMap[x+1][y] == 0) {				//Is area to right of rock empty?
			if (++push > pushPower) {
				push = 0;
				tileMap[x+1][y] = ball;			//Move ball and return that player can move right
				tileMap[x][y] = empty;
				manDir = 0;
				return 1;
			}
			else {
				manDir = 6;
			}			
		}		
	}

	  
  }

  if (tile == door and gemsCollected >= gemsNeeded) {

	if (editMode) {
		menuSelect = 0;
		stripAttributes();
		tileMap[pX][pY] = 0;
		tileMap[manStartX][manStartY] = man;
		winX = 0;
		winY = 0;
		sysState = edit;
		return 1;	  				
	}
	else {
		
		for (int x = 0 ; x < 8 ; x++) {		//Update door graphic in RAM
			tiles[24 + x] = pgm_read_byte_near(doori + 8 + x);
		}	
		
		playerState = 5;			//Set state to escaped! 
		screenTimer = 120;
		return 1;					//Allow man to move into door (he will get erased but we'll have to re-draw door)
	
	}


  }
    
  return 0;       //Else you can't move
  
}

void physics() {

	uint8_t xt;											//Temp target value
	uint8_t yt;

  for (int x = 1 ; x < 47 ; x++) {        				//Don't check the physics of the outer walls (sub 1 on all sides)

    for (int y = 30 ; y > 0 ; y--) {

      uint8_t object = tileMap[x][y];					//Get the value of this tile along with any flags

      switch(object & 0x0F) {          					 //Mask off the top 4 control bits of current tile to make decisions
		case(jar):
			if (object & falling) {						//Is this a falling jar? See if something is under it				
				if (tileMap[x][y+1]) { 					//If a falling jar hits anything, it breaks into slime
					explosion(x, y, slime);
					break;
				}		
			}	
		case(jem):                      				//Jems and balls do same stuff
        case(ball):                     				//Ball or Jem?
          switch(tileMap[x][y+1] & 0x0F) {     			//Decide what to do based off what's under the ball. AND out the upper flags (such as falling or movement patterns)		
			case(empty):                  				//Nothing? Ball falls
              tileMap[x][y] = 0;
              tileMap[x][y + 1] = object | falling;     //Erase ball and move it one space down. Add the FALLING bit to object
            break;
			case(jar):									//Was it falling and hit a jar? Jar breaks and releases deadly SLIME!
				if (object & falling) {					//The item must be falling for this to happen. If it spawns simply sitting above jar no explosion
					explosion(x, y + 1, slime);
				}
			break;
			case(man):									//Was it falling and hit a jar? Jar breaks and releases deadly SLIME!
				if (object & falling) {					//The item must be falling for this to happen. If it spawns simply sitting above jar no explosion
					killGuy();
				}
			break;			
			case(seeker):
			case(crawler):
            case(monster):                				//Did we land on a monster? Blow it up! (explosion centered on monster)
              explosion(x, y + 1, jem);
            break;
            case(jem):                    				//Roll off jems and balls same way
            case(ball):                   				//If possible balls roll off balls
				tileMap[x][y] &= 0x0F;					//Mask off falling bit. If item rolls left or right this may get re-enabled	
              if (tileMap[x-1][y] == 0 and tileMap[x-1][y+1] == 0 and tileMap[x+1][y] == 0 and tileMap[x+1][y+1] == 0) {		//Ball can roll either way?
                if (rollToggle & 1) {
                    tileMap[x][y] = 0;  
                    tileMap[x+1][y] = object;                                     //Roll ball right
                    x++;                                                        //Skip checking the next tile since we just put a ball there (checks from left to right)
                    rollToggle++;
                    //break;        
                }
                else {
                    tileMap[x][y] = 0;  
                    tileMap[x-1][y] = object;                                     //Roll ball left
                    rollToggle++;
                    //break;            
                }     
              }                                                                 //If we make it this far it means at least one side of the ball is blocked
              if (tileMap[x-1][y] == 0 and tileMap[x-1][y+1] == 0) {            //Space to the left and down? Roll left
                tileMap[x][y] = 0; 
                tileMap[x-1][y] = object;
                //break;
              }
              if (tileMap[x+1][y] == 0 and tileMap[x+1][y+1] == 0) {            //Space to the right? Roll right.
                tileMap[x][y] = 0; 
                tileMap[x+1][y] = object;
                x++;                                                            //Skip checking the next tile since we just put a ball there (checks from left to right)
                //break;
              }
			break;
			default:									//Landed inert on dirt or wall?		
				tileMap[x][y] &= 0x0F;					//Mask off falling bit	(if any)
			break;
            }

        break;

		case(slime):
			canSlimeGrow(x, y);		
		break;
		
		case(seeker):	
			if (object & dontMove) {          //Was this monster moved up in a previous lower row scan?
				tileMap[x][y] &= 0x7F;                 //Remove the bit and BREAK out to move onto next tile
			}
			else {

			if (radialCheck(x, y, man)) {
				if (killGuy()) {				//If player can be killed, abort all other checks
					break;
				}
			}
				
				if (pX < x && tileMap[x - 1][y] == 0) {		//Is the player to the left of seeker, and a slot to left is open?
					tileMap[x][y] = 0;							//Clear old tile
					tileMap[x - 1][y] = seeker;
				}
				else if (pX > x && tileMap[x + 1][y] == 0) {		//Is the player to the right of seeker, and a slot to right is open?
					tileMap[x][y] = 0;
					tileMap[x + 1][y] = seeker;
					x++;											//Since we moved to the right skip the check of the tile we just filled
				}	
				else if (pY < y && tileMap[x][y - 1] == 0) {		//Is the player above seeker and space open?
					tileMap[x][y] = 0;
					tileMap[x][y - 1] = seeker | dontMove;			//Move it and set flag skip next scan (so it doesn't move twice)
				}
				else if (pY > y && tileMap[x][y + 1] == 0) {		//Is the player below seeker and space open?
					tileMap[x][y] = 0;				
					tileMap[x][y +1] = seeker;
				}	
				
			}
		break;
        case(crawler):
		
			if (radialCheck(x, y, slime)) {
				explosion(x, y, jem);
				break;
			}
			if (radialCheck(x, y, man)) {
				if (killGuy()) {				//If player can be killed, abort all other checks
					break;
				}
			}
	
          if (object & dontMove) {          //Was this monster moved up in a previous lower row scan?
            tileMap[x][y] &= 0x7F;                 //Remove the bit and BREAK out to move onto next tile
          }
          else {                             //Normal operation check for available movement
           switch(object & 0xF0) {           //Mask off everything but the 2 bit direction indicator (UDLR)

            case(moveUp):                    //Moving up along a wall to the right?
              if (tileMap[x+1][y] == 0) {    //A gap to our right?
                tileMap[x][y] = 0;
                tileMap[x+1][y] = crawler | moveRight;   //Move crawler to right and skip next tile check
                x++;
              }
              else {                        //OK solid object to right. What about up?
                if (tileMap[x][y-1] == 0) { //Gap above us?
                    tileMap[x][y] = 0;
					tileMap[x][y-1] = object | dontMove;	//Move into it! (and skip next scan)
                }
				else {
					tileMap[x][y] = crawler | moveLeft; //Solid above? Turn left
				}
              }
            break;
            case(moveLeft):                  //Moving left with a wall above?
              if (tileMap[x][y-1] == 0) {    //A gap above us?
                tileMap[x][y] = 0;
                tileMap[x][y-1] = crawler | moveUp | dontMove;   //Move crawler up and change direction
              }
              else {                        //OK solid object above. What about left?
                if (tileMap[x-1][y] == 0) { //Gap to left?
                    tileMap[x][y] = 0;
					tileMap[x-1][y] = object;	//Move into it! (no change of control bits)
                }
				else {
					tileMap[x][y] = crawler | moveDown; //Solid to left? Turn down
				}
              }
            break;
            case(moveDown):                  //Moving down with a wall to left?
              if (tileMap[x-1][y] == 0) {    //A gap to our left?
                tileMap[x][y] = 0;
                tileMap[x-1][y] = crawler | moveLeft;   //Move crawler left and change direction
              }
              else {                        //OK solid object to left. What about down?
                if (tileMap[x][y+1] == 0) { //Gap below us?
                    tileMap[x][y] = 0;
					tileMap[x][y + 1] = object;	//Move into it! (no change of control bits)
                }
				else {
					tileMap[x][y] = crawler | moveRight; //Solid below us? Turn right
				}
              }
            break;
            case(moveRight):                  //Moving right with a wall below?
              if (tileMap[x][y+1] == 0) {    //A gap below us?
                tileMap[x][y] = 0;
                tileMap[x][y+1] = crawler| moveDown;   //Move crawler down and change direction
              }
              else {                        //OK solid object below. What about right?
                if (tileMap[x+1][y] == 0) { //Gap to right?
                    tileMap[x][y] = 0;
					tileMap[x+1][y] = object;	//Move into it! (no change of control bits)
					x++;
                }
				else {
					tileMap[x][y] = crawler | moveUp; //Solid to left? Turn up
				}
              }
            break;			
		   }
			
		}
        break;

        case(monster):
		
			if (radialCheck(x, y, slime)) {
				explosion(x, y, jem);
				break;
			}		
		
          if (object & dontMove) {          //Was this monster moved up in a previous lower row scan?
            tileMap[x][y] &= 0x7F;                 //Remove the bit and BREAK out to move onto next tile
          }
          else {                             //Normal operation check for available movement
           switch(object & 0xF0) {           //Mask off everything but the 2 bit direction indicator (UDLR)

            case(moveUp):                    //Up?
              if (tileMap[x][y-1] != 0) {    //Can't go up?
                tileMap[x][y] = monster | (random(4) << 4);
              }
              else {                        //Else move up
                tileMap[x][y] = 0;
                tileMap[x][y-1] = object | dontMove;            //Add the Don't Move bit so this object won't get moved when the row above is scanned
              }
            break;
            case(moveDown):                  //Down?
              if (tileMap[x][y+1] != 0) {   //Can't go down?
                tileMap[x][y] = monster | (random(4) << 4);
              }
              else {                        //Else move down
                tileMap[x][y] = 0;
                tileMap[x][y+1] = object;
              }
            break;
            case(moveLeft):                 //Left?
              if (tileMap[x-1][y] != 0) {   //Can't go left?
                object &= 0x0F;
                tileMap[x][y] = monster | (random(4) << 4);
              }
              else {                        //Else move left
                tileMap[x][y] = 0;
                tileMap[x-1][y] = object;
              }
            break;
            case(moveRight):                //Right?
              if (tileMap[x+1][y] != 0) {   //Can't go right?
                object &= 0x0F;
                tileMap[x][y] = monster | (random(4) << 4);
              }
              else {                        //Else move right
                tileMap[x][y] = 0;
                tileMap[x+1][y] = object;
                x++;
              }
            break;
			}
          }           
        break;
        case(exp1):       //First frame of explosion?
          tileMap[x][y]++;      //Advance to next frame
        break;
        case(exp2):       //Second frame of explosion?
          tileMap[x][y]++;      //Advance to next frame
        break;
        case(exp3):       //Final frame of explosion?         
          tileMap[x][y] = object >> 4;    //Upper nibble says what to leave behind. Can also be nothing
        break;       
        default:
        break;     
      } 
    }
  }
  	  
  
}

uint8_t dUp() {

	if (pressed(UP_BUTTON)) {		//Pressed? See if bit is set

		if (dPadBounceEnable & upBit) {	//Checking for retriggers?
			if (dPadBounce & upBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= upBit;			//Bit clear? Set it
			return 1;						//and return status		
		}
		else {
			return 1;
		}
	}
	else {
		dPadBounce &= ~upBit;			//Button not pressed? Clear the bit, allowing a retrigger
		return 0;
	}

}

uint8_t dDown() {

	if (pressed(DOWN_BUTTON)) {		//Pressed? See if bit is set

		if (dPadBounceEnable & downBit) {	//Checking for retriggers?
			if (dPadBounce & downBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= downBit;			//Bit clear? Set it
			return 1;						//and return status
		}
		else {
			return 1;
		}
	}
	else {
		dPadBounce &= ~downBit;			//Button not pressed? Clear the bit, allowing a retrigger
		return 0;
	}

}

uint8_t dLeft() {

	if (pressed(LEFT_BUTTON)) {		//Pressed? See if bit is set

		if (dPadBounceEnable & leftBit) {	//Checking for retriggers?
			if (dPadBounce & leftBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= leftBit;			//Bit clear? Set it
			return 1;						//and return status
		}
		else {
			return 1;
		}
	}
	else {
		dPadBounce &= ~leftBit;			//Button not pressed? Clear the bit, allowing a retrigger
	}

}

uint8_t dRight() {

	if (pressed(RIGHT_BUTTON)) {		//Pressed? See if bit is set

		if (dPadBounceEnable & rightBit) {	//Checking for retriggers?
			if (dPadBounce & rightBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= rightBit;			//Bit clear? Set it
			return 1;						//and return status
		}
		else {
			return 1;
		}

	}
	else {
		dPadBounce &= ~rightBit;			//Button not pressed? Clear the bit, allowing a retrigger
	}

}

uint8_t buttonA() {

	if (pressed(A_BUTTON)) {		//Pressed? See if bit is set

		if (dPadBounceEnable & aBit) {	//Checking for retriggers?
			if (dPadBounce & aBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= aBit;			//Bit clear? Set it
			return 1;						//and return status
		}
		else {
			return 1;
		}
	}
	else {
		dPadBounce &= ~aBit;			//Button not pressed? Clear the bit, allowing a retrigger
	}

}

uint8_t buttonB() {

	if (pressed(B_BUTTON)) {		//Pressed? See if bit is set
	
		if (dPadBounceEnable & bBit) {	//Checking for retriggers?
			if (dPadBounce & bBit) {		//Bit still set? No dice
				return 0;
			}
			dPadBounce |= bBit;			//Bit clear? Set it
			return 1;						//and return status
		}
		else {
			return 1;
		}
	}
	else {
		dPadBounce &= ~bBit;			//Button not pressed? Clear the bit, allowing a retrigger
	}

}

void stripAttributes() {					//Removes the top 4 attribute bits from all tiles on the map
	
	for (winY = 1 ; winY < 30 ; winY++) {
		
		for (winX = 1 ; winX < 47 ; winX++) {
			
			tileMap[winX][winY] &= 0x0F;
		
		}	
	}
	
}

void compressLevel() {			//RLE compresses a level using upper nibble for tile # and lower nibble for # of reps (up to 15) Sends to EEPROM or UART

	tileMap[pX][pY] = tileBackup;

	eeByte = tileMap[1][1];							//Get first tile
	gg = 0;												//Clear the count
	
	totalCount = 0;
	
	exportByte(gemsNeeded & 0x00FF);		//Low byte of value (little endian)
	exportByte(gemsNeeded >> 8);			//High byte of value
	exportByte(slimeGrow);					//How fast the slime grows
	exportByte(0x00);						//Reserved
	exportByte(0x00);						//Reserved	
	exportByte(0x00);						//Reserved
	exportByte(0x00);						//Reserved	
	exportByte(0x00);						//Reserved	
	
	int reloadFlag = 0;
	

	for (y = 1 ; y < 31 ; y++) {
		
		for (x = 1 ; x < 47 ; x++) {
			
			if (reloadFlag) {
				eeByte = tileMap[x][y];			//Get current tile (start of new block)
				reloadFlag = 0;
			}

			if (tileMap[x][y] == eeByte) {			//Does this tile match?		
				if (++gg == 15)	{							//Increment count (this also sets the first count of the first tile)
					exportByte(eeByte << 4 | gg);							//If we reach 15 (nibble limit) store the block
					reloadFlag = 1;
					gg = 0;
				}								
			}
			else {
				exportByte(eeByte << 4 | gg);								//Found different tile? Store previous block
				eeByte = tileMap[x][y];			//Get current tile (start of new block)
				gg = 1;										//And count that there's at least 1			
			}
		
		}	
	}

	totalCount++;
	exportByte(eeByte << 4 | gg);
	
	menuSelect = 0;

}

void decompressLevel() {		//Decompresses level data from EEPROM or FLASH

	tileMap[pX][pY] = tileBackup;

	gemsNeeded = importByte();
	gemsNeeded |= importByte();
	slimeGrow = importByte();
	
	importByte();				//Reserved
	importByte();
	importByte();	
	importByte();
	importByte();

	eeByte = importByte();							//Get first tile
	uint8_t tilesToGo = eeByte & 0x0F;				//Get number of RLE tiles (lower nibble)
	eeByte >>= 4;									//Bitshift over the upper nibble. This is the tile type

	for (y = 1 ; y < 31 ; y++) {
		
		for (x = 1 ; x < 47 ; x++) {

			tileMap[x][y] = eeByte;				//Place current tile on map
			
			if (eeByte == man) {						//Did we place a man? Set its position
				manStartX = x;
				manStartY = y;
			}
			if (eeByte == door) {						//Did we place a door? Set its position
				doorX = x;
				doorY = y;
			}			
			if (--tilesToGo == 0) {							//No more copies of this tile?
				eeByte = importByte();					//Get next tile
				tilesToGo = eeByte & 0x0F;						//and next count
				eeByte >>= 4;								
			}

		}	
	}

	x = 0;
	y = 0;
	
	pX = manStartX;
	pY = manStartY;
	
	winX = 0;
	winY = 0;
	
	menuSelect = 0;

}

uint8_t importByte() {		//Get a byte either from EEPROM or program flash (built in levels)
	
	if (exportTarget == 0) {				//Reading from EEPROM?		
		return EEPROM.read(ePoint++);		//Read byte and increment pointer		
	}
	else {									//Reading from program flash memory?
		return pgm_read_byte_near(levelData + ePoint++);
	}	

}

void exportByte(uint8_t theByte) {
	
	if (exportTarget == 0) {		
		if (ePoint & 0x08) {
			PORTB &= ~(1 << 6);				//Blink LED	
		}
		EEPROM.write(ePoint++, theByte);			//Upper nibble is tile #, lower nibble is # of tiles
		PORTB |= (1 << 6);					//Turn off LED
	}
	else {
		Serial.print("0x");
		Serial.print(theByte, HEX);
		Serial.print(", ");
	
		if (++itemsPerLine == 128) {			//After 128 entries, create a new line							
			Serial.println("");					//New line
			itemsPerLine = 0;
		}
	
	}	

	totalCount++;
	
}

void explosion(uint8_t x, uint8_t y, uint8_t result) {

  result <<= 4;         //Shift this to left. It sets what will remain after explosion is done

  tileMap[x][y] = exp1 | result;

  if (isValid(x - 1, y)) {
    tileMap[x-1][y] = exp1 | result;
  }
  if (isValid(x - 1, y - 1)) {
    tileMap[x-1][y-1] = exp1 | result;
  }
  if (isValid(x , y - 1)) {
    tileMap[x][y-1] = exp1 | result;
  }
  if (isValid(x + 1, y - 1)) {
    tileMap[x+1][y - 1] = exp1 | result;
  }
  if (isValid(x + 1, y)) {
    tileMap[x+1][y] = exp1 | result;
  }
  if (isValid(x + 1, y + 1)) {
    tileMap[x+1][y+1] = exp1 | result;
  }
  if (isValid(x , y + 1)) {
    tileMap[x][y+1] = exp1 | result;
  }
  if (isValid(x - 1, y + 1)) {
    tileMap[x-1][y+1] = exp1 | result;
  }
    
}

int isValid(uint8_t x, uint8_t y) {

	//Check here for hero man death by explosion

	if (tileMap[x][y] != wall) {
		return 1;
	}
	
	return 0;

  if (x > 0 and x < 47 and y > 0 and y < 31) {
    return 1; 
  }

  return 0;  
  
}

int radialCheck(uint8_t x, uint8_t y, uint8_t lookFor) {       //Checks if indicated tile appears next to current tile

	if (tileMap[x-1][y] == lookFor) {
		return 1;
	}
	if (tileMap[x][y-1] == lookFor) {
		return 1;
	}
	if (tileMap[x+1][y] == lookFor) {
		return 1;
	}	
	if (tileMap[x][y+1] == lookFor) {
		return 1;
	}	

	return 0;

}

void radial(uint8_t x, uint8_t y) {       //Fills a pointer array with all the bytes surrounding the tile specified in X,Y

	ringPt[0] = &tileMap[x-1][y];
	ringPt[1] = &tileMap[x-1][y-1];
	ringPt[2] = &tileMap[x][y-1];
	ringPt[3] = &tileMap[x+1][y-1];
	ringPt[4] = &tileMap[x+1][y];
	ringPt[5] = &tileMap[x+1][y+1];
	ringPt[6] = &tileMap[x][y+1];
	ringPt[7] = &tileMap[x-1][y+1];
	  
}

void canSlimeGrow(uint8_t x, uint8_t y) {		//See if any cells above, below or to the side of slime are empty for growth

	//Get the pointers of tiles around slime
	ringPt[0] = &tileMap[x-1][y];
	ringPt[1] = &tileMap[x][y-1];
	ringPt[2] = &tileMap[x+1][y];
	ringPt[3] = &tileMap[x][y+1];

	int count = 0;
		
	for (int x = 0 ; x < 4 ; x++) {					//Check all 4 cardinal directions
	
		if (*ringPt[x] < 2) {						//Cell empty or dirt?
			if (++slimeTimer == slimeGrow) {		//Must evaluate X number of slime tiles before they can grow
				slimeTimer = 0;
				if ((++slimeExpand & 0x03) == x) {				//Also the rolling space counter must match current tile
					*ringPt[x] = slime;
					return;										//Slime can only expand 1 cell at a time
				}
			}
				
		}
	}
	
	return; // count;

}

void showXY() {

  Serial.print("X=");
  Serial.print(winX);
  Serial.write(9);
  Serial.print("Y=");
  Serial.println(winY);
    
}

void scrollTiles(uint8_t howManyRows) {

  uint8_t winYFineINV = 8 - winYFine;				//Get remainder of Y fine scrolling to OR in next line of pixels                        
  uint8_t courseY = winY;                           //Get the course Y value for top line of visible screen

  for (uint8_t row = 0 ; row < howManyRows ; row++) {               //Draw the 8 tile row high display

    uint8_t finePointer = winXFine;         //Copy the fine scrolling amount so we can use it as a byte pointer when scanning in graphics
    uint8_t courseX = winX;            //Find the current coarseX position for this line

    for (uint8_t colB = 0 ; colB < 128 ; colB++) {         //Draw 16 column wide display (8 pixels / 1 char at a time)

      uint8_t upper = tiles[((tileMap[courseX][courseY] & 0x0F) << 3) + finePointer] >> winYFine;
      uint8_t lower = tiles[((tileMap[courseX][courseY + 1] & 0x0F) << 3) + finePointer] << winYFineINV;

      SPDR = upper | lower;                             //Send data to SPI and increment pointer 
      if (++finePointer > 7) {                          //Done drawing this tile?
        finePointer = 0;                                //Reset fine scroll
        courseX++;                                      //...and advance course scroll by 1 byte (1 tile)
      }            
  
    }

    courseY++;                                            //Increment coarse pointer to next row
    
  }

}

void sendTiles(uint8_t howManyRows) {

  uint16_t courseY = winY;                           //Get the course Y value for top line of visible screen

  for (uint8_t row = 0 ; row < howManyRows ; row++) {               //Draw the requested number of rows

    uint8_t courseX = winX;            //Find the current coarseX position for this line

    for (uint8_t colB = 0 ; colB < 16 ; colB++) {         //Draw 16 column wide display (8 pixels / 1 char at a time)

      //getTilePointer(courseX++, courseY);                  //Get the pointer for the next tile and stuff it in cP. Increment coarseX

	  uint8_t thisTile = tileMap[courseX][courseY];					//Get the tile value
	  
	  if (thisTile < 16) {									//Use tile graphics in RAM?
		  cP = &tiles[thisTile << 3];  						//Get the pointer to the first byte of this tile   

		  for (uint8_t col = 0 ; col < 8 ; col++) {           //Send the 8 horizontal lines to the OLED
			SPDR = *cP++;                                     //Send data and increment pointer 
			delayMicroseconds(2);               
		  }		  		  
	  }
	  else {												//Else use font from flash
	
		thisTile -= 32;										//Zero-offset to 32 in ASCII

		for (uint8_t col = 0 ; col < 8 ; col++) {           //Send the 8 horizontal lines to the OLED
			SPDR = pgm_read_byte_near(font + (thisTile << 3) + col);   //Get BG tile
			delayMicroseconds(2);               
		}	

		  
	  }
	  
	  courseX++;
  
    }

    courseY++;                                            //Increment coarse pointer to next row
    
  }

}

void drawTitle() {

	for (int x = 0 ; x < 512 ; x++) {
	
		SPDR = pgm_read_byte_near(title + x);   //Get BG tile
		delayMicroseconds(2);  		
	
	}
	
}

void resetAnimation() {

	for (int x = 0 ; x < 48 ; x++) {				//Copy 40 bytes from flash pointer to tile RAM
		tiles[56 + x] = pgm_read_byte_near(aniTiles + x);
	}
	
}	

void sendMenuLine() {
	
    for (uint8_t colB = 0 ; colB < 16 ; colB++) {         //Draw 16 column wide display (8 pixels / 1 char at a time)

	  uint8_t thisTile = scratch[colB];					//Get the tile value
	  
	  if (thisTile < 16) {									//Use tile graphics in RAM?
		  cP = &tiles[thisTile << 3];  						//Get the pointer to the first byte of this tile   

		  for (uint8_t col = 0 ; col < 8 ; col++) {           //Send the 8 horizontal lines to the OLED
			SPDR = *cP++;                                     //Send data and increment pointer 
			delayMicroseconds(2);               
		  }		  		  
	  }
	  else {												//Else use font from flash
	
		thisTile -= 32;										//Zero-offset to 32 in ASCII

		for (uint8_t col = 0 ; col < 8 ; col++) {           //Send the 8 horizontal lines to the OLED
			SPDR = pgm_read_byte_near(font + (thisTile << 3) + col);   //Get BG tile
			delayMicroseconds(2);               
		}	

		  
	  }
	  
    }	
	
}

void text(uint8_t xPos, uint8_t yPos, const char *str) {

  while (*str) {
    tileMap[xPos++][yPos] = *str++; 
  }

}

void textMenu(uint8_t xPos, const char *str) {

  while (*str) {
    scratch[xPos++] = *str++; 
  }

}

void drawNumberMenu(int theValue, int x) {
  
  int numerals = x;           				//Always going to be at least 1 numeral
  int zPad = 0;             				//Flag for zero padding 
  unsigned long divider = 100;    			//Divider starts at 100k
    
  for (int xx = 0 ; xx < 3 ; xx++) {    	//6 digit number    
    if (theValue >= divider) {
      scratch[numerals++] = '0' + (theValue / divider);
      theValue %= divider;
      zPad = 1;
      //numerals += 1;
    }   
    else if (zPad or divider == 1) {
      scratch[numerals++] = '0';
      //numerals += 1;
    } 
    divider /= 10;            
  }

  //scratch[numerals] = 0;  
   
}

void drawHexMenu(uint8_t theValue, int x) {
  
  scratch[x++] = hexValue[theValue >> 4];
  scratch[x] = hexValue[theValue & 0x0F];

  //scratch[numerals] = 0;  
   
}

void drawNumberMap(int theValue, int x, int y) {
  
  int numerals = x;           				//Always going to be at least 1 numeral
  int zPad = 0;             				//Flag for zero padding 
  unsigned long divider = 100;    			//Divider starts at 100k
    
  for (int xx = 0 ; xx < 3 ; xx++) {    	//6 digit number    
    if (theValue >= divider) {
		tileMap[x++][y] = '0' + (theValue / divider);
      theValue %= divider;
      zPad = 1;
      //numerals += 1;
    }   
    else if (zPad or divider == 1) {
		
      tileMap[x++][y] = '0';
      //numerals += 1;
    } 
    divider /= 10;            
  }

  //scratch[numerals] = 0;  
   
}

int cpuLoad() {
  return lastFrameDurationMs*100 / eachFrameMillis;
} 

bool nextFrame() {
  unsigned long now = millis();
  bool tooSoonForNextFrame = now < nextFrameStart;

  if (justRendered) {
    lastFrameDurationMs = now - lastFrameStart;
    justRendered = false;
    return false;
  }
  else if (tooSoonForNextFrame) {
    // if we have MORE than 1ms to spare (hence our comparison with 2),
    // lets sleep for power savings.  We don't compare against 1 to avoid
    // potential rounding errors - say we're actually 0.5 ms away, but a 1
    // is returned if we go to sleep we might sleep a full 1ms and then
    // we'd be running the frame slighly late.  So the last 1ms we stay
    // awake for perfect timing.

    // This is likely trading power savings for absolute timing precision
    // and the power savings might be the better goal. At 60 FPS trusting
    // chance here might actually achieve a "truer" 60 FPS than the 16ms
    // frame duration we get due to integer math.

    // We should be woken up by timer0 every 1ms, so it's ok to sleep.
    if ((uint8_t)(nextFrameStart - now) >= 2)
      a.idle();

    return false;
  }

  // pre-render
  justRendered = true;
  lastFrameStart = now;
  nextFrameStart = now + eachFrameMillis;
  frameCount++;

  return true;
}

bool pressed(uint8_t buttons) {
  return (buttonsState() & buttons) == buttons;
}

uint8_t buttonsState() {
  
  uint8_t buttons;

  // using ports here is ~100 bytes smaller than digitalRead()
#ifdef AB_DEVKIT
  // down, left, up
  buttons = ((~PINB) & B01110000);
  // right button
  buttons = buttons | (((~PINC) & B01000000) >> 4);
  // A and B
  buttons = buttons | (((~PINF) & B11000000) >> 6);
#elif defined(ARDUBOY_10)
  // down, up, left right
  buttons = ((~PINF) & B11110000);
  // A (left)
  buttons = buttons | (((~PINE) & B01000000) >> 3);
  // B (right)
  buttons = buttons | (((~PINB) & B00010000) >> 2);
#endif

  return buttons;
}

void walls() {			//Fills the map with standard walls and dirt

  for (int i = 0 ; i < 48 ; i++) {
    tileMap[i][0] = wall;
    tileMap[i][31] = wall;
  }
  for (int i = 1 ; i < 31 ; i++) {
    tileMap[0][i] = wall;
    for (int ii = 1 ; ii < 47 ; ii++) {
      tileMap[ii][i] = 1; //Fill with dirt
    }
    tileMap[47][i] = wall;
  }
 
	//Place standard door and man
	manStartX = 1;
	manStartY = 1;
	
	tileMap[1][1] = man;

	doorX = 46;
	doorY = 30;
		
	tileMap[46][30] = door;
  
	
}

void randomLevel() {	//Creates a random level

  for (int i = 0 ; i < 500 ; i++) {
    tileMap[random(45) + 1][random(29) + 1] = empty;
  }

  for (int i = 0 ; i < 200 ; i++) {
    tileMap[random(45) + 1][random(29) + 1] = ball;
  }

  for (int i = 0 ; i < 10 ; i++) {
    tileMap[random(45) + 1][random(29) + 1] = monster | (random(4) << 4);
  }

  for (int i = 0 ; i < 10 ; i++) {
    tileMap[random(45) + 1][random(29) + 1] = crawler | moveUp;
  }
  
  for (int i = 0 ; i < 10 ; i++) {
    tileMap[random(45) + 1][random(29) + 1] = jar;
  }  

	
	
}

void fill(uint8_t withWhat) {

	cP = &tileMap[0][0];
	
	for (g = 0 ; g < 1536 ; g++) {
		*cP++ = withWhat;
	}
	
}


