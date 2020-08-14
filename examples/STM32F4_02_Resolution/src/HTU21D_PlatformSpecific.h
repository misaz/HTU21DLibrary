#ifndef HTU21D_PLATFORMSPECIFIC_H_
#define HTU21D_PLATFORMSPECIFIC_H_

#include <stm32f4xx.h>
#include <stdlib.h>
#include <stdint.h>

#define HTU21D_I2C					I2C3
#define HTU21D_I2C_RCC_APB1Periph	RCC_APB1Periph_I2C3

#define HTU21D_SDA_GPIO				GPIOC
#define HTU21D_SDA_GPIO_Pin			GPIO_Pin_9
#define HTU21D_SDA_GPIO_PinSource	GPIO_PinSource9
#define HTU21D_SDA_GPIO_AHB1Periph	RCC_AHB1Periph_GPIOC
#define HTU21D_SDA_GPIO_AF			GPIO_AF_I2C3

#define HTU21D_SCL_GPIO				GPIOA
#define HTU21D_SCL_GPIO_Pin			GPIO_Pin_8
#define HTU21D_SCL_GPIO_PinSource	GPIO_PinSource8
#define HTU21D_SCL_GPIO_AHB1Periph	RCC_AHB1Periph_GPIOA
#define HTU21D_SCL_GPIO_AF			GPIO_AF_I2C3

void HTU21D_PlatformSpecificInit();
int HTU21D_I2CBeginWrite();
int HTU21D_I2CBeginRead();
int HTU21D_I2CRead(uint8_t* buffer, size_t size);
int HTU21D_I2CWrite(uint8_t* buffer, size_t size);
int HTU21D_I2CStop();

#endif
