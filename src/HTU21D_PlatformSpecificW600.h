#ifndef HTU21D_PLATFORMSPECIFIC_H_
#define HTU21D_PLATFORMSPECIFIC_H_

#include <stdlib.h>
#include <stdint.h>

#define HTU21D_SDA_PIN		WM_IO_PB_14
#define HTU21D_SCL_PIN		WM_IO_PB_13
#define HTU21D_I2C_SPEED	100000

void HTU21D_PlatformSpecificInit();
int HTU21D_I2CBeginWrite();
int HTU21D_I2CBeginRead();
int HTU21D_I2CRead(uint8_t* buffer, size_t size);
int HTU21D_I2CWrite(uint8_t* buffer, size_t size);
int HTU21D_I2CStop();

#endif
