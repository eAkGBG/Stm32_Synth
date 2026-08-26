//file main.h
#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx_hal.h"

/*
Spi display. ST7789v2 240x280 rgb
	scl -> pa5
	sda -> pa7
	res -> pb2
	dc -> pa6
	cs -> pa4
	blk -> Pull up kepp the display backlight on for now.

Buttons micro switches connected to GND
	red -> pa0
	blue -> pa1
	small value down -> pa2
	small value up -> pa3
	(perhaps i add spinning encoder/s)

Sound DAC pcm5100A
	lck -> pb12
	bck -> pb13
	din -> pb15
	sck -> gnd (connected to gnd with pulldown activates the dac internal clock)

Sensor (Not decided yet probably TOF with either multizone, single, or a few single)
*/

//Defines for lcd driver.
//Reset pin pb2
#define LCD_RST_GPIO_Port		GPIOB
#define LCD_RST_Pin			GPIO_PIN_2
//Data Command dc pin pa6
#define LCD_DC_GPIO_Port		GPIOA
#define LCD_DC_Pin				GPIO_PIN_6
//Chip select cd pa4
#define LCD_CS_GPIO_Port		GPIOA
#define LCD_CS_Pin				GPIO_PIN_4
//

//Buttons
// Red -> pa0
#define BTN_RED_GPIO_Port     GPIOA
#define BTN_RED_Pin           GPIO_PIN_0

//Blue -> pa1
#define BTN_BLUE_GPIO_Port    GPIOA
#define BTN_BLUE_Pin          GPIO_PIN_1

//Value down -> pa2
#define BTN_DOWN_GPIO_Port    GPIOA
#define BTN_DOWN_Pin          GPIO_PIN_2

//Value up -> pa3
#define BTN_UP_GPIO_Port      GPIOA
#define BTN_UP_Pin            GPIO_PIN_3


//Sound I2S DEFINES (PCM5100A)
//lck -> pb12
#define AUDIO_LCK_GPIO_Port   GPIOB
#define AUDIO_LCK_Pin         GPIO_PIN_12

//bck -> pb13
#define AUDIO_BCK_GPIO_Port   GPIOB
#define AUDIO_BCK_Pin         GPIO_PIN_13

//din -> pb15
#define AUDIO_DIN_GPIO_Port   GPIOB
#define AUDIO_DIN_Pin         GPIO_PIN_15

#endif //MAIN_H