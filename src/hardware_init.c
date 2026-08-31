#include "hardware_init.h"
#include "main.h"

void my_hardware_init(void) {
    // Enable peripheral clocks for GPIO ports A and B, SPI1, and DMA2
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE(); // <--- 1. TURN ON DMA2 HARDWARE CLOCK

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //SPI DISPLAY CONTROLS (Outputs)
    GPIO_InitStruct.Pin   = LCD_DC_Pin | LCD_CS_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = LCD_RST_Pin;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


    //SPI1 HARDWARE DATA PINS (SCL -> PA5, SDA -> PA7)
    GPIO_InitStruct.Pin       = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1; 
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    // DMA2 CONFIGURATION FOR SPI1 TX (hardware locked to stream 3 channel 3 ?)
    hdma_spi1_tx.Instance                 = DMA2_Stream3;         // SPI1_TX uses Stream 3
    hdma_spi1_tx.Init.Channel             = DMA_CHANNEL_3;         // channel 3 is mapped to SPI1
    hdma_spi1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;  // Send RAM to screen
    hdma_spi1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;      // SPI-register is not auto incrementing
    hdma_spi1_tx.Init.MemInc              = DMA_MINC_ENABLE;       // RAM-adress are to auto increase pixel by pixel
    hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;   // 8-bit transfer
    hdma_spi1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;    // 8-bit transfer
    hdma_spi1_tx.Init.Mode                = DMA_NORMAL;            // Normal mode (not circualr)
    hdma_spi1_tx.Init.Priority            = DMA_PRIORITY_HIGH;     // Highest priority
    hdma_spi1_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;  // direct mode no fifo buffering
    HAL_DMA_Init(&hdma_spi1_tx);                                   // initialize the dma stream

    //Connect the dma stream to the spi interface
    __HAL_LINKDMA(&hspi1, hdmatx, hdma_spi1_tx);

    //SPI1 PROTOCOL SETTINGS (SPI Mode 3)
    hspi1.Instance               = SPI1;
    hspi1.Init.Mode              = SPI_MODE_MASTER;
    hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity       = SPI_POLARITY_HIGH; 
    hspi1.Init.CLKPhase          = SPI_PHASE_2EDGE;   
    hspi1.Init.NSS               = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; 
    hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    HAL_SPI_Init(&hspi1); 
    HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0); //Highest priority on the interupt
    HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);        //Activate interupts in the mcu


    //BUTTONS (Inputs with Internal Pull-Up resistors)
    GPIO_InitStruct.Pin   = BTN_RED_Pin | BTN_BLUE_Pin | BTN_DOWN_Pin | BTN_UP_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //LED
    __HAL_RCC_GPIOC_CLK_ENABLE();
    //HAL_GPIO_WritePin(LED1_GPIO_port, LED1_Pin, GPIO_PIN_SET);

    GPIO_InitStruct.Pin   = LED1_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;   
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED1_GPIO_port, &GPIO_InitStruct);
    

}
#ifdef STM32F411
void audio_init(void)
{
    __HAL_RCC_SPI2_CLK_ENABLE();  // I2S2 lives on the SPI2 peripheral (APB1)
    __HAL_RCC_DMA1_CLK_ENABLE();  // I2S2_TX uses DMA1 Stream4

    hdma_i2s2_tx.Instance                 = DMA1_Stream4;
    hdma_i2s2_tx.Init.Channel             = DMA_CHANNEL_0;       // SPI2_TX request
    hdma_i2s2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_i2s2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_i2s2_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_i2s2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; // 16-bit samples
    hdma_i2s2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    hdma_i2s2_tx.Init.Mode                = DMA_CIRCULAR;        // loops forever
    hdma_i2s2_tx.Init.Priority            = DMA_PRIORITY_HIGH;
    hdma_i2s2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_i2s2_tx);
    __HAL_LINKDMA(&hi2s2, hdmatx, hdma_i2s2_tx);

    /*
    hi2s2.Instance        = SPI2;
    hi2s2.Init.Mode           = I2S_MODE_MASTER_TX;
    hi2s2.Init.Standard       = I2S_STANDARD_MSB;
    //hi2s2.Init.Standard       = I2S_STANDARD_PHILIPS;
    hi2s2.Init.DataFormat     = I2S_DATAFORMAT_16B;
    hi2s2.Init.MCLKOutput     = I2S_MCLKOUTPUT_DISABLE;  // SCK is grounded, internal PLL
    hi2s2.Init.AudioFreq      = I2S_AUDIOFREQ_44K;
    hi2s2.Init.CPOL           = I2S_CPOL_LOW;
    hi2s2.Init.ClockSource    = I2S_CLOCK_PLL;
    hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    HAL_I2S_Init(&hi2s2);
*/
    hi2s2.Instance            = SPI2;
    hi2s2.Init.Mode           = I2S_MODE_MASTER_TX;
    hi2s2.Init.Standard       = I2S_STANDARD_PHILLIPS;  // PCM5100A = I2S standard
    hi2s2.Init.DataFormat     = I2S_DATAFORMAT_16B_EXTENDED;
    hi2s2.Init.MCLKOutput     = I2S_MCLKOUTPUT_DISABLE; // module MCLK pulldown'd -> internal PLL
    hi2s2.Init.AudioFreq      = I2S_AUDIOFREQ_44K;
    hi2s2.Init.CPOL           = I2S_CPOL_LOW;           // required for Philips
    hi2s2.Init.ClockSource    = I2S_CLOCK_PLL;
    hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
    HAL_I2S_Init(&hi2s2);

    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}
#endif

#ifdef STM32F401
void audio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_SPI2_CLK_ENABLE();  // I2S2 lives on the SPI2 peripheral (APB1)
    __HAL_RCC_DMA1_CLK_ENABLE();  // I2S2_TX uses DMA1 Stream4
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = GPIO_PIN_8;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING; 
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;      
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // exti interupt on pin pa8
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0); // priority 1. need to look these over later for now it works. did not make that much happen in ui or sound yet.
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    //AUDIO SPI PIN MAPPING (PCM5100A)
    GPIO_InitStruct.Pin         = SPI2_SCK_Pin | SPI2_TX_Pin | SPI2_RX_Pin;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate   = GPIO_AF5_SPI2; 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    //Handle the CS pin
    GPIO_InitStruct.Pin         = SPI2_CS_Pin;
    GPIO_InitStruct.Mode        = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate   = 0;
    HAL_GPIO_Init(SPI2_CS_GPIO_Port, &GPIO_InitStruct);

    hdma_spi2_tx.Instance                 = DMA1_Stream4;
    hdma_spi2_tx.Init.Channel             = DMA_CHANNEL_0;       // SPI2_TX request
    hdma_spi2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    hdma_spi2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
    hdma_spi2_tx.Init.MemInc              = DMA_MINC_ENABLE;
    hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD; // 16-bit samples
    hdma_spi2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
    hdma_spi2_tx.Init.Mode                = DMA_NORMAL;
    hdma_spi2_tx.Init.Priority            = DMA_PRIORITY_LOW;
    hdma_spi2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_spi2_tx);
    __HAL_LINKDMA(&hspi2, hdmatx, hdma_spi2_tx);

    //som setup probably learn this some day. now main task code ui and synth when the hardware is running.
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
    //hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    //hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH; 
    hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    
    //hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; // 8 is 5.25mhz
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; //perhaps its not 5.25 mhz maybe its 10.5 well it works for now. or something different.
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    
    HAL_SPI_Init(&hspi2);
    hspi2.Instance->CR1 |= SPI_CR1_SSI; 

    HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}
#endif



//ned to try to learn this clocks setup some day.
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    //Regulator: VOS scale 2 (needed for 84 MHz)
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    //HSI 16 MHz -> PLL: /8 -> x84 -> /2 -> 84 MHz SYSCLK
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 84;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                       RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   // 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;   // 84 MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

    //PLLI2S: 16/8 x 88 = 176 MHz VCO -> /4 = 44 MHz I2S clock
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2S;
    PeriphClkInit.PLLI2S.PLLI2SN = 192; //PeriphClkInit.PLLI2S.PLLI2SN = 88; 
    PeriphClkInit.PLLI2S.PLLI2SR = 2; //PeriphClkInit.PLLI2S.PLLI2SR = 4;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}
