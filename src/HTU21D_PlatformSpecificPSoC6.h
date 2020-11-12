#ifndef HTU21D_PLATFORMSPECIFIC_H_
#define HTU21D_PLATFORMSPECIFIC_H_

#include <stdlib.h>
#include <stdint.h>

#include "cy_pdl.h"

#define HTU21D_SCB             						SCB2
#define HTU21D_SCB_CLOCK       						PCLK_SCB2_CLOCK
#define HTU21D_SCB_CLOCK_DIVIDER_TYPE				CY_SYSCLK_DIV_8_BIT
#define HTU21D_SCB_CLOCK_DIVIDER_NUM				1
#define HTU21D_SCB_CLOCK_DIVIDER_VALUE				64
#define HTU21D_SCB_CLICK_DIVIDER_DO_CONFIGURATION	1

#define HTU21D_SDA_PORT								P9_1_PORT
#define HTU21D_SDA_NUM								P9_1_NUM
#define HTU21D_SDA_ALT 								P9_1_SCB2_I2C_SDA

#define HTU21D_SCL_PORT								P9_0_PORT
#define HTU21D_SCL_NUM								P9_0_NUM
#define HTU21D_SCL_ALT 								P9_0_SCB2_I2C_SCL

void HTU21D_PlatformSpecificInit();
int HTU21D_I2CBeginWrite();
int HTU21D_I2CBeginRead();
int HTU21D_I2CRead(uint8_t* buffer, size_t size);
int HTU21D_I2CWrite(uint8_t* buffer, size_t size);
int HTU21D_I2CStop();

#endif
