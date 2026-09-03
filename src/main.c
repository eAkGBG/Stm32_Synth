//file main.c
#include <stdlib.h>
#include <math.h>
#include "main.h"
#include "lvgl.h"
#include "ui.h"
#include "hardware_init.h"
#include "synth.h"
#include "vl53l0x_api.h"

DMA_HandleTypeDef hdma_spi1_tx; // Global handle för din SPI1 DMA
SPI_HandleTypeDef hspi1; //global handle for spi1
I2C_HandleTypeDef hi2c1; //Global handle for i2c1
DMA_HandleTypeDef hdma_i2c1_rx;

VL53L0X_Dev_t MyDevice;
VL53L0X_DEV Dev = &MyDevice; // Pointer that the st lib uses for the sensor.

#ifdef STM32F401
    SPI_HandleTypeDef hspi2;
    DMA_HandleTypeDef hdma_spi2_tx;
#endif
#ifdef STM32F411
    I2S_HandleTypeDef hi2s2; //For pcm5100a i2s dac module
    DMA_HandleTypeDef hdma_i2s2_tx;
#endif
lv_display_t *my_display; // Global display handle 

//for EXTI interupts we have to define these global.
synth_ui_t ui = {0};
synth_data_t synth = {0};
volatile bool system_initialized = false;


uint32_t transfer_count = 0;
uint16_t phase = 0;

//buffer for dma i2c1
volatile uint16_t tof_distance = 220;

void tof_dwt_init(void);
void my_lcd_send_cmd(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, const uint8_t *param, size_t param_size);
void my_lcd_send_color(lv_display_t *disp, const uint8_t *cmd, size_t cmd_size, uint8_t *param, size_t param_size);
void start_vl53l0x_background_mode(void);



int main(void){
    //init MCU
    HAL_Init();
    SystemClock_Config();
    my_hardware_init();
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(50);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(150);
    
    //init the dac
    audio_init();
    start_vl53l0x_background_mode();
    HAL_Delay(50);
                                  
 


#ifdef STM32F411
    HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)sine_buf, 512);  // runs forever via circular DMA
#endif
    HAL_Delay(50);

    lv_init();
    lv_tick_set_cb(HAL_GetTick);
    my_display = lv_st7789_create(240, 280, LV_LCD_FLAG_NONE, my_lcd_send_cmd, my_lcd_send_color);
    lv_st7789_set_gap(my_display, 0, 20);
    lv_st7789_set_invert(my_display, true); //fix colors so they are not inverted ? on my display.
    lv_display_set_color_format(my_display, LV_COLOR_FORMAT_RGB565_SWAPPED);
    static uint8_t buf[240 * 10 * 2]; 
    lv_display_set_buffers(my_display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    synth_data_init(&synth);
    //Ok here is the new ui struct.
    synth_ui_init(&ui, &synth);


    system_initialized = true;
    //Test the ui function pointer.
        //ui.set_value(&ui, (int32_t)10);
    //test menu update.
        //ui.active_menu_item = 1;
        //ui.set_value(&ui, (int32_t)10);
        //ui.update_menu(&ui);
        //ui.nav_next(&ui);
        //it works. selection changes. and values print

    //Ui is now creating a struct with the menu and items hooked to the synth struct.
    //will let me code functions to change synth values and navigate the menu.

    //ToDo: hook the synth struct inside the buffer update IRQ function.
    //use it to keep time? It need to work on the f411 also if i casn get the timer on that chip to match
    //So don't make it only compattible with spi sound card protocol.
    
    //this forces interupt to check if the code is activated.
    //EXTI->SWIER |= GPIO_PIN_8;
    
    while (1){
        uint8_t ready = 0;

        //check if there is a measurement.
        if (VL53L0X_GetMeasurementDataReady(Dev, &ready) == VL53L0X_ERROR_NONE && ready) {
            VL53L0X_RangingMeasurementData_t r;

            if (VL53L0X_GetRangingMeasurementData(Dev, &r) == VL53L0X_ERROR_NONE) {
                if (r.RangeStatus == 0) {
                    //Update the global
                    tof_distance = r.RangeMilliMeter; 
                }
            }
            //Tell the sensor to do next measurement.
            VL53L0X_ClearInterruptMask(Dev, VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
        }
        lv_timer_handler();
        
        ui.update_menu(&ui);

        HAL_Delay(50);
    }
    

}

void SysTick_Handler(void)
{
  HAL_IncTick();
  //lv_tick_inc(1); //documentation says to use lv_tick_set_cb
  //lv_tick_set_cb(HAL_GetTick); //code seems to run without this on tic. it's set in main. Perhaps i misunderstood the docs
    //lv_timer_handler(); is connected som way so i guess we only need update in main while loop.
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI9_5_IRQHandler(void)
{
    // Here scheck what pinn was triggered
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

// New ISR: just chain to HAL
void DMA2_Stream3_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hdma_spi1_tx);
}

void I2C1_EV_IRQHandler(void) {
    HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler(void) {
    HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Stream0_IRQHandler(void) {
    HAL_DMA_IRQHandler(hi2c1.hdmarx);
}




void DMA1_Stream4_IRQHandler(void)
{
    #ifdef STM32F411
    HAL_DMA_IRQHandler(&hdma_i2s2_tx);
    #endif
    #ifdef STM32F401
    HAL_DMA_IRQHandler(&hdma_spi2_tx);
    #endif
}

// HAL callback: this is where you do the CS de-assert + flush_ready
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
        lv_display_flush_ready(my_display);
    }
    #ifdef STM32F401
    else if (hspi->Instance == SPI2) {
        // De-assert your SPI2 Chip Select pin
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET); 
        
        // Add any other post-transfer logic for SPI2 here
        //if the led is turned off. the irq is working propperly.
        //HAL_GPIO_WritePin(LED1_GPIO_port, LED1_Pin, GPIO_PIN_SET);
        /* transfer_count++;
        if (transfer_count >= 375) { //once per sek at 48kHz
            HAL_GPIO_TogglePin(LED1_GPIO_port, LED1_Pin);
            transfer_count = 0;
        } */
       
    }
    #endif
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (!system_initialized) {
        return; 
    }

    static uint32_t last_press_time_blue = 0;
    static uint32_t last_press_time_red = 0;
    static uint32_t last_press_time_up = 0;
    static uint32_t last_press_time_down = 0;
    uint32_t current_time = HAL_GetTick();

#ifdef STM32F401
    if (GPIO_Pin == GPIO_PIN_8)
    {
        // Pi Pico requested data.
       
        // pull cs pin low
        HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);

        //test to reset the spi busy state before send.
            //hspi2.State = HAL_SPI_STATE_READY;
            //__HAL_SPI_CLEAR_OVRFLAG(&hspi2); //After testing seems we do not need the reset.


        //do DMA pulling se if we have a new note value from the TOF sensor.
        /* if(__HAL_DMA_GET_FLAG(&hdma_i2c1_rx, DMA_FLAG_TCIF0_4)){
            //Clear the dma flag manually.
            __HAL_DMA_CLEAR_FLAG(&hdma_i2c1_rx, DMA_FLAG_TCIF0_4); //This flag TCIF0_4 is the hal macro to read the transfer compleate on stream 0

            //this is how to get the data. bit shift the 8 byte packets into one 16bit variable.
            uint16_t distance = (tof_data_buffer[0] << 8) | tof_data_buffer[1];
            synth.tof_distance = distance;

            HAL_I2C_Mem_Read_DMA(&hi2c1, (0x29 << 1), 0x1E, I2C_MEMADD_SIZE_8BIT, tof_data_buffer, 2);
        } */
        //Time to test the synth ADSR
        synth.tof_distance = tof_distance;
        synth.osc1_generator(&synth); //the generator will automatically apply the adsr
        
        //transmitt DMA 256 16-bit R,L,R,L and so on (128 stereo-samples)
        //Catch status from the DMA TX to check for errors.
        HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)synth.buffer, (BUFFER_SIZE * 2));
        
        //lets make the led blink!
        if (status == HAL_OK) {
            //Blink the led once per second if status is success(375 buffers to fill 1 second of 48khz)
            transfer_count++;
            if (transfer_count >= 375) { //once per sek at 48kHz
                HAL_GPIO_TogglePin(LED1_GPIO_port, LED1_Pin);
                transfer_count = 0;
            }
        }
    }
#endif

    //Handle menu buttons.
    //reading the docs i have set nvic to irq for all my button lines 0-4 so inside here we can call the functions for each button.
    //also added a debounce variable.
    if(GPIO_Pin == BTN_BLUE_Pin){
        if ((current_time - last_press_time_blue) > 200) { 
            ui.nav_next(&ui);
            last_press_time_blue = current_time;
        }
    }
    if(GPIO_Pin == BTN_RED_Pin){
        if ((current_time - last_press_time_red) > 200) {
            ui.nav_prev(&ui);
            last_press_time_red = current_time;
        }
    }
    if(GPIO_Pin == BTN_UP_Pin){
        if ((current_time - last_press_time_up) > 200) { 
            //ui.nav_next(&ui);
            ui.val_up(&ui);
            last_press_time_up = current_time;
        }
    }
    if(GPIO_Pin == BTN_DOWN_Pin){
        if ((current_time - last_press_time_down) > 200) {
            ui.val_down(&ui);
            last_press_time_down = current_time;
        }
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



void start_vl53l0x_background_mode(void) {
    // start the hardware in cyceling mode.
    vl53l0x_init();
    HAL_Delay(10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); 
    HAL_Delay(10);
    
    Dev->I2cDevAddr = (0x29 << 1);//0x52;
    tof_dwt_init();

    //read standard params
    if (VL53L0X_DataInit(Dev) != VL53L0X_ERROR_NONE) { while(1); }

    // basic callibration at startup.
    if (VL53L0X_StaticInit(Dev) != VL53L0X_ERROR_NONE) { while(1); }

    uint8_t VhvSettings, PhaseCal;
    if (VL53L0X_PerformRefCalibration(Dev, &VhvSettings, &PhaseCal) != VL53L0X_ERROR_NONE) { while(1); }

    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    if (VL53L0X_PerformRefSpadManagement(Dev, &refSpadCount, &isApertureSpads) != VL53L0X_ERROR_NONE) { while(1); }

    // set it in cycling mode.
    if (VL53L0X_SetDeviceMode(Dev, VL53L0X_DEVICEMODE_CONTINUOUS_RANGING) != VL53L0X_ERROR_NONE) { while(1); }

    // its now cycling by itself.
    if (VL53L0X_StartMeasurement(Dev) != VL53L0X_ERROR_NONE) { while(1); }
}



//Test function it generates a saw The math is correct.
/* void audio_buffer_fill(void)
{
    //c5 523.25
    //d5 587.33
    //e5 659.25
    //f5 698,46
    //g5 783.99
    //a5 880.00
    //b5 987.77
    //uint16_t phase = 65536*0.5;
    //uint16_t phase = 0;
    uint16_t step = (uint32_t)65536*440/48000;
    //uint16_t step = 654;
    uint32_t sample_index = 0;
    for (uint32_t i = 0; i < BUF_SAMPLES; i++)
    {
        int16_t sample = (int16_t)(phase - 32768);
        sample = sample * 0.01;
        uint16_t convert_sample = (uint16_t)sample; //& 0xFFFF; //Don't think that 0xffff part is needed.

        //sine_buf[sample_index++] = (int16_t)((int16_t)phase * 0.1);
        s_buf[sample_index ++] = convert_sample;
        s_buf[sample_index ++] = convert_sample;
        phase += step;
    }
}  */


