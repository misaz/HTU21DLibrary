#ifndef HTU21D_PLATFORMSPECIFIC_H_
#define HTU21D_PLATFORMSPECIFIC_H_

#include <stdlib.h>
#include <stdint.h>

void HTU21D_PlatformSpecificInit();
int HTU21D_I2CBeginWrite();
int HTU21D_I2CBeginRead();
int HTU21D_I2CRead(uint8_t* buffer, size_t size);
int HTU21D_I2CWrite(uint8_t* buffer, size_t size);
int HTU21D_I2CStop();

#endif
