#include "HTU21D.h"
#include "HTU21D_PlatformSpecific.h"

void HTU21D_Init() {
	HTU21D_PlatformSpecificInit();
}

int _HTU21D_VerifyChecksum(uint8_t* buffer) {
	// description of CRC algorithm is for example at Wikipedia
	// https://en.wikipedia.org/wiki/Cyclic_redundancy_check

	// My opinion is that Czech wikipedia contains little bit better (and understable)
	// example. But it is only in Czech :-(
	// https://cs.wikipedia.org/wiki/Cyklick%C3%BD_redundantn%C3%AD_sou%C4%8Det

	// this is implementation which calculates CRC only for 2 bytes using one uint32_t 
	// space and compares it with third byte in buffer. This design simplifies operation,
	// memory usage and is simple to develop but It cannot be easily tweeked to support
	// more than 3 bytes.

	// 23 is because 1st bit of polynomial must be aligned to bit 31 in uint32_t and 
	// polynomial is 9bit bumber.
	uint32_t polynomial = (uint32_t)0b100110001 << 23;

	// temp is compostion of:
	// +-------------------------+-----------------------------+-----------+
	// |      bits 31:16         |           bits 15:8         | bits 7:0  |
	// |      input data         | calculated CRC will be here |  unused   |
	// +------------+------------+-----------------------------+-----------+
	// | bits 31:24 | bits 23:16 |           bits 15:8         | bits 7:0  |
	// | buffer[0]  | buffer[1]  | calculated CRC will be here | 0000 0000 |
	// +------------+------------+-----------------------------+-----------+
	uint32_t temp = (((uint32_t) buffer[0]) << 24) | (((uint32_t) buffer[1]) << 16);

	// we are iterating over bits of data from left to right
	for (int i = 31; i >= 16; i--) {
		uint32_t currentBit = temp & ((uint32_t)1 << i);

		// if bit is set => toggle pattern (this effectively clears one more bit and
		// apply pattern to others)
		if (currentBit) {
			temp ^= polynomial;
		}

		// move pattern to match next position
		polynomial >>= 1;
	}

	// extract calculated CRC from temp "structure"
	uint8_t calculatedPolynomial = (temp & 0xFF00) >> 8;

	if (calculatedPolynomial == buffer[2]) {
		return HTU21D_E_OK;
	} else {
		return HTU21D_E_BAD_CRC;
	}
}

void _HTU21D_ParseTemperature(float* outputTemperature, uint8_t* buffer) {
	uint16_t rawValue = (((uint16_t) buffer[0]) << 8) | (((uint16_t) buffer[1]) << 0);

	// remove status from data
	rawValue &= ~(0x03);

	*outputTemperature = -46.85 + 175.72 * (float) rawValue / (float) ((uint32_t)1 << 16);
}

void _HTU21D_ParseHumidity(float* outputTemperature, uint8_t* buffer) {
	uint16_t rawValue = (((uint16_t) buffer[0]) << 8) | (((uint16_t) buffer[1]) << 0);

	// remove status from data
	rawValue &= ~(0x03);

	*outputTemperature = -6.0 + 125.0 * (float) rawValue / (float) ((uint32_t)1 << 16);
}

int _HTU21D_ReadNonBlockingBegin(uint8_t command) {
	int status;

	status = HTU21D_I2CBeginWrite();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CWrite(&command, 1);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	return HTU21D_I2CStop();
}

int _HTU21D_ReadNonBlockingPoll(float* outputValue, void (*parseValueFunction)(float*, uint8_t*)) {
	int status;

	status = HTU21D_I2CBeginRead();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	uint8_t buffer[3];
	status = HTU21D_I2CRead(buffer, 3);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	HTU21D_I2CStop();

	status = _HTU21D_VerifyChecksum(buffer);
	if (status) {
		return status;
	}

	parseValueFunction(outputValue, buffer);
	return HTU21D_E_OK;
}

int _HTU21D_ReadNonBlocking(uint8_t command, float* outputValue, size_t pollingInterval, size_t maxAttemps, void (*parseValueFunction)(float*, uint8_t*)) {
	int status = _HTU21D_ReadNonBlockingBegin(command);
	if (status) {
		return status;
	}

	for (size_t i = 0; i < maxAttemps; i++) {
		int status = _HTU21D_ReadNonBlockingPoll(outputValue, parseValueFunction);

		if (status == HTU21D_E_OK) {
			return HTU21D_E_OK;
		} else if (status == HTU21D_E_ADDR_NACKED) {
			continue;
		} else {
			return status;
		}
	}

	return HTU21D_E_TIMEOUT;
}

int _HTU21D_ReadBlocking(uint8_t command, float* outputValue, void (*parseValueFunction)(float*, uint8_t*)) {
	int status;

	status = HTU21D_I2CBeginWrite();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CWrite(&command, 1);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	return _HTU21D_ReadNonBlockingPoll(outputValue, parseValueFunction);
}

int _HTU21D_ReadRegister(uint8_t* outputRegisterValue) {
	int status;
	uint8_t command = HTU21D_CMD_READ_REG;

	status = HTU21D_I2CBeginWrite();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CWrite(&command, 1);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CBeginRead();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CRead(outputRegisterValue, 1);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	return HTU21D_I2CStop();
}

int _HTU21D_WriteRegister(uint8_t newValue) {
	int status;
	uint8_t buffer[2];
	buffer[0] = HTU21D_CMD_WRITE_REG;
	buffer[1] = newValue;

	status = HTU21D_I2CBeginWrite();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CWrite(buffer, 2);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	return HTU21D_I2CStop();
}

int _HTU21D_RegisterSetBit(uint8_t bitMask) {
	int status;
	uint8_t registerValue;
	uint8_t originalValue;

	status = _HTU21D_ReadRegister(&originalValue);
	if (status) {
		return status;
	}

	registerValue = originalValue;

	registerValue |= bitMask;

	if (originalValue == registerValue) {
		return HTU21D_E_OK;
	}

	return _HTU21D_WriteRegister(registerValue);
}

int _HTU21D_RegisterClearBit(uint8_t bitMask) {
	int status;
	uint8_t registerValue;
	uint8_t originalValue;

	status = _HTU21D_ReadRegister(&originalValue);
	if (status) {
		return status;
	}

	registerValue = originalValue;

	registerValue &= ~bitMask;

	if (originalValue == registerValue) {
		return HTU21D_E_OK;
	}

	return _HTU21D_WriteRegister(registerValue);
}

int _HTU21D_IsBitSet(uint8_t bitMask, int* bitStatus) {
	uint8_t registerValue;
	int status = _HTU21D_ReadRegister(&registerValue);
	if (status) {
		return status;
	}

	*bitStatus = !!(registerValue & bitMask);
	return HTU21D_E_OK;
}

int HTU21D_ReadTemperature(float* outputTemperature) {
	return HTU21D_ReadTemperatureBlocking(outputTemperature);
}

int HTU21D_ReadTemperatureBlocking(float* outputTemperature) {
	return _HTU21D_ReadBlocking(HTU21D_CMD_TEMPERATURE_HOLD, outputTemperature, _HTU21D_ParseTemperature);
}

int HTU21D_ReadTemperatureNonBlocking(float* outputTemperature, size_t pollingInterval, size_t maxAttemps) {
	return _HTU21D_ReadNonBlocking(HTU21D_CMD_TEMPERATURE_NOHOLD, outputTemperature, pollingInterval, maxAttemps, _HTU21D_ParseTemperature);
}

int HTU21D_ReadTemperatureNonBlockingBegin() {
	return _HTU21D_ReadNonBlockingBegin(HTU21D_CMD_TEMPERATURE_NOHOLD);
}

int HTU21D_ReadTemperatureNonBlockingPoll(float* outputTemperature) {
	return _HTU21D_ReadNonBlockingPoll(outputTemperature, _HTU21D_ParseTemperature);
}

int HTU21D_ReadHumidity(float* outputHumidity) {
	return HTU21D_ReadHumidityBlocking(outputHumidity);
}

int HTU21D_ReadHumidityBlocking(float* outputHumidity) {
	return _HTU21D_ReadBlocking(HTU21D_CMD_HUMIDITY_HOLD, outputHumidity, _HTU21D_ParseHumidity);
}

int HTU21D_ReadHumidityNonBlocking(float* outputHumidity, size_t pollingInterval, size_t maxAttemps) {
	return _HTU21D_ReadNonBlocking(HTU21D_CMD_HUMIDITY_NOHOLD, outputHumidity, pollingInterval, maxAttemps, _HTU21D_ParseHumidity);
}

int HTU21D_ReadHumidityNonBlockingBegin() {
	return _HTU21D_ReadNonBlockingBegin(HTU21D_CMD_HUMIDITY_NOHOLD);
}

int HTU21D_ReadHumidityNonBlockingPoll(float* outputHumidity) {
	return _HTU21D_ReadNonBlockingPoll(outputHumidity, _HTU21D_ParseHumidity);
}

int HTU21D_GetResolution(uint8_t* currentResolution) {
	int status;
	uint8_t registerValue;

	status = _HTU21D_ReadRegister(&registerValue);
	if (status) {
		return status;
	}

	*currentResolution = registerValue & HTU21D_REG_RESOLUTION_MASK;


	return HTU21D_E_OK;
}

int HTU21D_SetResolution(uint8_t resolution) {
	int status;
	uint8_t registerValue;
	uint8_t originalValue;

	status = _HTU21D_ReadRegister(&originalValue);
	if (status) {
		return status;
	}

	registerValue = originalValue;

	registerValue &= ~HTU21D_REG_RESOLUTION_MASK;
	registerValue |= (resolution & HTU21D_REG_RESOLUTION_MASK);

	if (originalValue == registerValue) {
		return HTU21D_E_OK;
	}

	return _HTU21D_WriteRegister(registerValue);
}

int HTU21D_GetEndOfBattery(int* isEndOfBattery) {
	return _HTU21D_IsBitSet(HTU21D_REG_END_OF_BATERY, isEndOfBattery);
}

int HTU21D_EnableOnChipHeater() {
	return _HTU21D_RegisterSetBit(HTU21D_REG_ENABLE_ONCHIP_HEATER);
}

int HTU21D_DisableOnChipHeadere() {
	return _HTU21D_RegisterClearBit(HTU21D_REG_ENABLE_ONCHIP_HEATER);
}

int HTU21D_IsOnChipHeaterEnabled(int* onChipHeaterStatus) {
	return _HTU21D_IsBitSet(HTU21D_REG_ENABLE_ONCHIP_HEATER, onChipHeaterStatus);
}

int HTU21D_EnableOtpPreload() {
	return _HTU21D_RegisterClearBit(HTU21D_REG_DISABLE_OTP_PRELOAD);
}

int HTU21D_DisableOtpPreload() {
	return _HTU21D_RegisterSetBit(HTU21D_REG_DISABLE_OTP_PRELOAD);
}

int HTU21D_IsOtpPreloadDisabled(int* otpPreoloadDisabled) {
	return _HTU21D_IsBitSet(HTU21D_REG_DISABLE_OTP_PRELOAD, otpPreoloadDisabled);
}

int HTU21D_SoftReset() {
	int status;
	uint8_t command = HTU21D_CMD_SOFT_RESET;

	status = HTU21D_I2CBeginWrite();
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	status = HTU21D_I2CWrite(&command, 1);
	if (status) {
		HTU21D_I2CStop();
		return status;
	}

	return HTU21D_I2CStop();
}
