//file main.c
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "lvgl.h"
#include "ui.h"
#include "hardware_init.h"
#include "synth.h"

DMA_HandleTypeDef hdma_spi1_tx; // Global handle för din SPI1 DMA
SPI_HandleTypeDef hspi1;
lv_display_t *my_display; // Global display handle 
I2S_HandleTypeDef hi2s2; //For pcm5100a i2s dac module
DMA_HandleTypeDef hdma_i2s2_tx;

//some globals for the audio
#define AUDIO_FS     44100
#define SINE_FREQ    1000
#define SINE_SAMPLES 441//(AUDIO_FS / SINE_FREQ * 10)  // 480 samples = exactly 10 periods
#define SINE_AMP     10000                        // ~ -12 dBFS, no clipping

static uint16_t sine_buf[SINE_SAMPLES];

void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);

void Audio_SineFill(void);
void DMA1_Stream4_IRQHandler(void);

int main(void){
    //init MCU
    HAL_Init();
    SystemClock_Config();
    My_Hardware_Init();
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(150);
    
    //init the dac
    Audio_Init();
    Audio_SineFill();
    HAL_I2S_Transmit_DMA(&hi2s2, sine_buf, SINE_SAMPLES);  // runs forever via circular DMA

    lv_init();
    my_display = lv_st7789_create(240, 280, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);
    lv_st7789_set_gap(my_display, 0, 20);
    lv_st7789_set_invert(my_display, true); //fix colors so they are not inverted ? on my display.
    lv_display_set_color_format(my_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    static uint8_t buf[240 * 10 * 2]; 
    lv_display_set_buffers(my_display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    synth_ui_init();

    //experementing with function pointers inside a struct.
    int32_t synth_test_get = 0;
    synth_data_t synth = {0};
    synth_data_init(&synth);
    synth.set_attack(&synth, 25);
    synth_test_get = synth.get_attack(&synth);
    synth_ui_test_print(synth_test_get);
    //need to make a note how this works so i don't forget. it's usefull!! I think this is pretty standard
    //stuff to know by heart. ToDo: build the synth struct to work like this. I think it's good for cleaner
    //execution of code.
    while (1)
    {
        lv_timer_handler();
        HAL_Delay(100);/* code */
    }
    

}

void SysTick_Handler(void)
{
  HAL_IncTick();
  //lv_tick_inc(1); //documentation says to use lv_tick_set_cb
  lv_tick_set_cb(HAL_GetTick);
}
// New ISR: just chain to HAL
void DMA2_Stream3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void DMA1_Stream4_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_i2s2_tx);
}

// HAL callback: this is where you do the CS de-assert + flush_ready
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        lv_display_flush_ready(my_display);
    }
}

// Send short command to the LCD (Polling transfer)
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size)
{
    // DC Low = Command
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    
    HAL_SPI_Transmit(&hspi1, (uint8_t *)cmd, cmd_size, HAL_MAX_DELAY);
    
    // If command has parameters, DC High = Data
    if (param_size > 0) {
        HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
        HAL_SPI_Transmit(&hspi1, (uint8_t *)param, param_size, HAL_MAX_DELAY);
    }
    
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

    //HAL_Delay(1);
}

// Send large array of pixel data to the LCD (DMA transfer)
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size)
{
    // DC Low for the write command (usually 0x2C RAMWR)
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, (uint8_t *)cmd, cmd_size, HAL_MAX_DELAY);
    
    // DC High for raw pixel data stream
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
    
    // Blast the pixel buffer in the background using DMA2
    HAL_SPI_Transmit_DMA(&hspi1, param, param_size);
}

void Audio_SineFill(void)
{
    for (uint32_t i = 0; i < SINE_SAMPLES; i++)
    {
        float angle = 6.2831853f * (float)i * (float)SINE_FREQ / (float)AUDIO_FS;
        sine_buf[i] = (uint16_t)(SINE_AMP * sinf(angle));
    }
}
