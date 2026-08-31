//file main.h
#ifndef MAIN_H
#define MAIN_H

//#define STM32F411
#define STM32F401
#include "stm32f4xx_hal.h"

/*
LED
	Lets use the board LED on pc13

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
//i keep this in here, ordered an stm32f411 board. acording to google ai that board has hardware i2s clocking
//that will make the dac work.
Sound DAC pcm5100A
	lck -> pb12
	bck -> pb13
	din -> pb15
	sck -> gnd (connected to gnd with pulldown activates the dac internal clock)

Sound rpi pico w acting as a doubble buffered sound card. it will take 128k sterio buffer over spi at 10mhz
ToDo:Not connected like this to rpi anny more so update this.
from the pico defines.	#define SPI_SCK_PIN  18   // PB13
						#define SPI_RX_PIN   19   // PB15  (STM32 MOSI -> Pico)
						#define SPI_TX_PIN   16   // PB14  (Pico MISO, not sampled by STM32)
						#define SPI_CS_PIN   17   // PB12  (decorative in slave mode)
						#define READY_PIN    20   // PA8   (Pico OUTPUT: "send 128 frames now")
so that is how they are connected. pi will set gp20 high connected to pa8 on the stm. then stm will send 128 * uint16_t * 2, so uint16 for L and uint16 for R
So what wee need to do now is to fix this behaviour.

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
#ifdef STM32F411
//lck -> pb12
#define AUDIO_LCK_GPIO_Port   GPIOB
#define AUDIO_LCK_Pin         GPIO_PIN_12

//bck -> pb13
#define AUDIO_BCK_GPIO_Port   GPIOB
#define AUDIO_BCK_Pin         GPIO_PIN_13

//din -> pb15
#define AUDIO_DIN_GPIO_Port   GPIOB
#define AUDIO_DIN_Pin         GPIO_PIN_15
#endif
//Sound spi
#ifdef STM32F401
//CS -> pb12
#define SPI2_CS_GPIO_Port   GPIOB
#define SPI2_CS_Pin         GPIO_PIN_12

//SCK -> pb13
#define SPI2_SCK_GPIO_Port   GPIOB
#define SPI2_SCK_Pin        GPIO_PIN_13

//MOSI TX -> pb15
#define SPI2_TX_GPIO_Port   GPIOB
#define SPI2_TX_Pin         GPIO_PIN_15

//MISO RX -> pb14
#define SPI2_RX_GPIO_Port   GPIOB
#define SPI2_RX_Pin         GPIO_PIN_14
#endif

//The blue led on black pill board
#define LED1_GPIO_port		GPIOC
#define LED1_Pin			GPIO_PIN_13

#endif //MAIN_H