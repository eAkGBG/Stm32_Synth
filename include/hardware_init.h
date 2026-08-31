#ifndef HARDWARE_INIT_H
#define HARDWARE_INIT_H

#include "stm32f4xx_hal.h"
#include "main.h"

extern SPI_HandleTypeDef  hspi1;
extern DMA_HandleTypeDef  hdma_spi1_tx;
extern SPI_HandleTypeDef  hspi2;
extern DMA_HandleTypeDef  hdma_spi2_tx;
extern I2S_HandleTypeDef  hi2s2;
extern DMA_HandleTypeDef  hdma_i2s2_tx;

void SystemClock_Config(void);
void my_hardware_init(void);
void audio_init(void);

#endif