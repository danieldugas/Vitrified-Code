#include "Shortcuts.h"


//If method returns 0xFF, then no pin with the desired index was found
uint8_t GetPinSource(uint16_t GPIOPins, int index)
{
//#define GPIO_Pin_0                 ((uint16_t)0x0001)  /* Pin 0 selected */
//#define GPIO_Pin_1                 ((uint16_t)0x0002)  /* Pin 1 selected */
//#define GPIO_Pin_2                 ((uint16_t)0x0004)  /* Pin 2 selected */
//#define GPIO_Pin_3                 ((uint16_t)0x0008)  /* Pin 3 selected */
//#define GPIO_Pin_4                 ((uint16_t)0x0010)  /* Pin 4 selected */
//#define GPIO_Pin_5                 ((uint16_t)0x0020)  /* Pin 5 selected */
//#define GPIO_Pin_6                 ((uint16_t)0x0040)  /* Pin 6 selected */
//#define GPIO_Pin_7                 ((uint16_t)0x0080)  /* Pin 7 selected */
//#define GPIO_Pin_8                 ((uint16_t)0x0100)  /* Pin 8 selected */
//#define GPIO_Pin_9                 ((uint16_t)0x0200)  /* Pin 9 selected */
//#define GPIO_Pin_10                ((uint16_t)0x0400)  /* Pin 10 selected */
//#define GPIO_Pin_11                ((uint16_t)0x0800)  /* Pin 11 selected */
//#define GPIO_Pin_12                ((uint16_t)0x1000)  /* Pin 12 selected */
//#define GPIO_Pin_13                ((uint16_t)0x2000)  /* Pin 13 selected */
//#define GPIO_Pin_14                ((uint16_t)0x4000)  /* Pin 14 selected */
//#define GPIO_Pin_15                ((uint16_t)0x8000)  /* Pin 15 selected */
//#define GPIO_Pin_All               ((uint16_t)0xFFFF)  /* All pins selected */

//#define GPIO_PinSource0            ((uint8_t)0x00)
//#define GPIO_PinSource1            ((uint8_t)0x01)
//#define GPIO_PinSource2            ((uint8_t)0x02)
//#define GPIO_PinSource3            ((uint8_t)0x03)
//#define GPIO_PinSource4            ((uint8_t)0x04)
//#define GPIO_PinSource5            ((uint8_t)0x05)
//#define GPIO_PinSource6            ((uint8_t)0x06)
//#define GPIO_PinSource7            ((uint8_t)0x07)
//#define GPIO_PinSource8            ((uint8_t)0x08)
//#define GPIO_PinSource9            ((uint8_t)0x09)
//#define GPIO_PinSource10           ((uint8_t)0x0A)
//#define GPIO_PinSource11           ((uint8_t)0x0B)
//#define GPIO_PinSource12           ((uint8_t)0x0C)
//#define GPIO_PinSource13           ((uint8_t)0x0D)
//#define GPIO_PinSource14           ((uint8_t)0x0E)
//#define GPIO_PinSource15           ((uint8_t)0x0F)

	uint8_t counter = 0;
	uint8_t bitCounter = 0;
	uint16_t i = GPIOPins;
	while(i > 0)
	{
		if(i & 0x0001) // Check if last bit is set
		{
			if(bitCounter == index)
				return counter;
			++bitCounter;
		}
		++counter;
		i = i >> 1;
	}
	//No pin with the desired index was found
	return 0xFF;
}
