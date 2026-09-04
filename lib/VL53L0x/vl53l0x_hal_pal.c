//ST API library hooks by LLM to use the API on stm32f4
#include "vl53l0x_platform.h"
#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_api.h"
#include "stm32f4xx_hal.h"

#ifndef STATUS_OK
#define STATUS_OK   0
#define STATUS_FAIL 1
#endif

#ifndef VL53L0X_MAX_I2C_XFER_SIZE
#define VL53L0X_MAX_I2C_XFER_SIZE 31
#endif

extern I2C_HandleTypeDef hi2c1;          /* hardware_init.h */
#define TOF_XSHUT_PORT  GPIOB
#define TOF_XSHUT_PIN   GPIO_PIN_6       /* XSHUT -> PB6 */
#define I2C_TIMEOUT_MS  200              /* sensor stretches SCL during calibration */

#define VL53L0X_I2C_USER_VAR
#define VL53L0X_GetI2CAccess(Dev)
#define VL53L0X_DoneI2CAcces(Dev)

/* ---- raw I2C ---- */
int32_t VL53L0X_read_multi(uint8_t address, uint8_t reg, uint8_t *pdata, int32_t count)
{
    return (HAL_I2C_Mem_Read(&hi2c1, address, reg, I2C_MEMADD_SIZE_8BIT,
                             pdata, count, I2C_TIMEOUT_MS) == HAL_OK) ? STATUS_OK : STATUS_FAIL;
}

int32_t VL53L0X_write_multi(uint8_t address, uint8_t reg, uint8_t *pdata, int32_t count)
{
    return (HAL_I2C_Mem_Write(&hi2c1, address, reg, I2C_MEMADD_SIZE_8BIT,
                              pdata, count, I2C_TIMEOUT_MS) == HAL_OK) ? STATUS_OK : STATUS_FAIL;
}

int32_t VL53L0X_read_byte(uint8_t a, uint8_t r, uint8_t *d) { return VL53L0X_read_multi(a, r, d, 1); }
int32_t VL53L0X_write_byte(uint8_t a, uint8_t r, uint8_t d) { return VL53L0X_write_multi(a, r, &d, 1); }

int32_t VL53L0X_read_word(uint8_t a, uint8_t r, uint16_t *d)
{
    uint8_t b[2];
    int32_t s = VL53L0X_read_multi(a, r, b, 2);
    *d = (uint16_t)((b[0] << 8) | b[1]);          /* MSB first */
    return s;
}

int32_t VL53L0X_write_word(uint8_t a, uint8_t r, uint16_t d)
{
    uint8_t b[2] = { (uint8_t)(d >> 8), (uint8_t)d };
    return VL53L0X_write_multi(a, r, b, 2);
}

/* ---- Dev wrappers (mirrors vl53l0x_platform.c) ---- */
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
    if (count >= VL53L0X_MAX_I2C_XFER_SIZE) return VL53L0X_ERROR_INVALID_PARAMS;
    return (VL53L0X_write_multi(Dev->I2cDevAddr, index, pdata, (int32_t)count) == STATUS_OK)
           ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count)
{
    if (count >= VL53L0X_MAX_I2C_XFER_SIZE) return VL53L0X_ERROR_INVALID_PARAMS;
    return (VL53L0X_read_multi(Dev->I2cDevAddr, index, pdata, (int32_t)count) == STATUS_OK)
           ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data)
{ return (VL53L0X_write_byte(Dev->I2cDevAddr, index, data) == STATUS_OK)
       ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE; }

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data)
{ return (VL53L0X_read_byte(Dev->I2cDevAddr, index, data) == STATUS_OK)
       ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE; }

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data)
{ return (VL53L0X_read_word(Dev->I2cDevAddr, index, data) == STATUS_OK)
       ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE; }

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data)
{ return (VL53L0X_write_word(Dev->I2cDevAddr, index, data) == STATUS_OK)
       ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE; }

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev)   { (void)Dev; return VL53L0X_ERROR_NONE; }
VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev) { (void)Dev; return VL53L0X_ERROR_NONE; }

/* ---- GPIO (XSHUT) ---- */
int32_t VL53L0X_set_gpio(uint8_t level)
{ HAL_GPIO_WritePin(TOF_XSHUT_PORT, TOF_XSHUT_PIN, level ? GPIO_PIN_SET : GPIO_PIN_RESET); return STATUS_OK; }
int32_t VL53L0X_get_gpio(uint8_t *plevel)
{ *plevel = (uint8_t)HAL_GPIO_ReadPin(TOF_XSHUT_PORT, TOF_XSHUT_PIN); return STATUS_OK; }
int32_t VL53L0X_release_gpio(void) { return STATUS_OK; }
int32_t VL53L0X_cycle_power(void)  { return STATUS_OK; }  /* power handled via set_gpio */

/* ---- timing (DWT) ---- */
void tof_dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}

int32_t VL53L0X_platform_wait_us(int32_t wait_us)
{
    uint32_t start = DWT->CYCCNT;
    int32_t cycles = wait_us * (SystemCoreClock / 1000000u);
    while ((int32_t)(DWT->CYCCNT - start) < cycles) ;
    return STATUS_OK;
}

int32_t VL53L0X_wait_ms(int32_t wait_ms)
{ if (wait_ms > 0) HAL_Delay(wait_ms); return STATUS_OK; }

int32_t VL53L0X_get_timer_frequency(int32_t *hz) { *hz = (int32_t)SystemCoreClock; return STATUS_OK; }
int32_t VL53L0X_get_timer_value(int32_t *v)      { *v = (int32_t)DWT->CYCCNT; return STATUS_OK; }

/* ---- log stub (replaces vl53l0x_platform_log.c) ---- */
int32_t VL53L0X_trace_config(char *fn, uint32_t m, uint32_t l, uint32_t f)
{ (void)fn; (void)m; (void)l; (void)f; return 0; }

/* ---- 32-bit register access (was in the deleted Windows PAL) ---- */
VL53L0X_Error VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data)
{
    uint8_t b[4];
    if (VL53L0X_read_multi(Dev->I2cDevAddr, index, b, 4) != STATUS_OK)
        return VL53L0X_ERROR_CONTROL_INTERFACE;
    *data = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
            ((uint32_t)b[2] << 8)  |  b[3];
    return VL53L0X_ERROR_NONE;
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data)
{
    uint8_t b[4] = { (uint8_t)(data >> 24), (uint8_t)(data >> 16),
                     (uint8_t)(data >> 8),  (uint8_t)data };
    return (VL53L0X_write_multi(Dev->I2cDevAddr, index, b, 4) == STATUS_OK)
        ? VL53L0X_ERROR_NONE : VL53L0X_ERROR_CONTROL_INTERFACE;
}

/* read / modify / write single byte [1] */
VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index,
                                 uint8_t AndData, uint8_t OrData)
{
    uint8_t cur;
    VL53L0X_Error Status = VL53L0X_RdByte(Dev, index, &cur);
    if (Status == VL53L0X_ERROR_NONE)
        Status = VL53L0X_WrByte(Dev, index, (cur & AndData) | OrData);
    return Status;
}

/* short delay used inside the polling loops [1] */
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev)
{
    (void)Dev;
    HAL_Delay(1);
    return VL53L0X_ERROR_NONE;
}