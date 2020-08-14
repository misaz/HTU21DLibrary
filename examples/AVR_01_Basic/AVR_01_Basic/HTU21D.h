#ifndef HTU21D_H_
#define HTU21D_H_

#include <stdlib.h>
#include <stdint.h>

#define HTU21D_I2C_ADDRESS 0x40

#define HTU21D_CMD_TEMPERATURE_HOLD 0xE3
#define HTU21D_CMD_TEMPERATURE_NOHOLD 0xF3
#define HTU21D_CMD_HUMIDITY_HOLD 0xE5
#define HTU21D_CMD_HUMIDITY_NOHOLD 0xF5
#define HTU21D_CMD_WRITE_REG 0xE6
#define HTU21D_CMD_READ_REG 0xE7
#define HTU21D_CMD_SOFT_RESET 0xFE

#define HTU21D_REG_RESOLUTION_MASK 0x81
#define HTU21D_REG_RESOLUTION_T14_H12 0x00
#define HTU21D_REG_RESOLUTION_T13_H10 0x80
#define HTU21D_REG_RESOLUTION_T12_H08 0x01
#define HTU21D_REG_RESOLUTION_T11_H11 0x81
#define HTU21D_REG_END_OF_BATERY 0x40
#define HTU21D_REG_ENABLE_ONCHIP_HEATER 0x04
#define HTU21D_REG_DISABLE_OTP_PRELOAD 0x02

#define HTU21D_E_OK 0
#define HTU21D_E_TIMEOUT -1
#define HTU21D_E_ADDR_NACKED -2
#define HTU21D_E_BUS_ERROR -3
#define HTU21D_E_BAD_CRC -4

void HTU21D_Init();

int HTU21D_ReadTemperature(float* outputTemperature);
int HTU21D_ReadTemperatureBlocking(float* outputTemperature);
int HTU21D_ReadTemperatureNonBlocking(float* outputTemperature, size_t pollingInterval, size_t maxAttemps);
int HTU21D_ReadTemperatureNonBlockingBegin();
int HTU21D_ReadTemperatureNonBlockingPoll(float* outputTemperature);

int HTU21D_ReadHumidity(float* outputHumidity);
int HTU21D_ReadHumidityBlocking(float* outputHumidity);
int HTU21D_ReadHumidityNonBlocking(float* outputHumidity, size_t pollingInterval, size_t maxAttemps);
int HTU21D_ReadHumidityNonBlockingBegin();
int HTU21D_ReadHumidityNonBlockingPoll(float* outputHumidity);

int HTU21D_GetResolution(uint8_t* currentResolution);
int HTU21D_SetResolution(uint8_t resolution);

int HTU21D_GetEndOfBattery(int* isEndOfBattery);

int HTU21D_EnableOnChipHeater();
int HTU21D_DisableOnChipHeadere();
int HTU21D_IsOnChipHeaterEnabled(int* onChipHeaterStatus);

int HTU21D_EnableOtpPreload();
int HTU21D_DisableOtpPreload();
int HTU21D_IsOtpPreloadDisabled(int* otpPreoloadDisabled);

int HTU21D_SoftReset();

#endif
