#include "HTU21D_PlatformSpecific.h"
#include "HTU21D.h"

void HTU21D_PlatformSpecificInit() {
	RCC_AHB1PeriphClockCmd(HTU21D_SDA_GPIO_AHB1Periph, ENABLE);
	RCC_AHB1PeriphClockCmd(HTU21D_SCL_GPIO_AHB1Periph, ENABLE);

	GPIO_PinAFConfig(HTU21D_SDA_GPIO, HTU21D_SDA_GPIO_PinSource, HTU21D_SDA_GPIO_AF);
	GPIO_PinAFConfig(HTU21D_SCL_GPIO, HTU21D_SCL_GPIO_PinSource, HTU21D_SCL_GPIO_AF);

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_OD;
	gpio.GPIO_Speed = GPIO_Speed_25MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;

	gpio.GPIO_Pin = HTU21D_SDA_GPIO_Pin;
	GPIO_Init(HTU21D_SDA_GPIO, &gpio);

	gpio.GPIO_Pin = HTU21D_SCL_GPIO_Pin;
	GPIO_Init(HTU21D_SCL_GPIO, &gpio);

	RCC_APB1PeriphClockCmd(HTU21D_I2C_RCC_APB1Periph, ENABLE);

	I2C_InitTypeDef i2c;
	i2c.I2C_Ack = I2C_Ack_Disable;
	i2c.I2C_AcknowledgedAddress = 0;
	i2c.I2C_ClockSpeed = 100000;
	i2c.I2C_DutyCycle = I2C_DutyCycle_2;
	i2c.I2C_Mode = I2C_Mode_I2C;
	i2c.I2C_OwnAddress1 = 0;

	I2C_Cmd(HTU21D_I2C, ENABLE);
	I2C_Init(HTU21D_I2C, &i2c);
}

int _HTU21D_WaitForEvent(uint32_t eventMask) {
	while (1) {
		if (I2C_CheckEvent(HTU21D_I2C, eventMask)) {
			return HTU21D_E_OK;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_BERR)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_ARLO)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_AF)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_OVR)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_PECERR)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_TIMEOUT)) {
			return HTU21D_E_BUS_ERROR;
		}

		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_SMBALERT)) {
			return HTU21D_E_BUS_ERROR;
		}
	}
}

int _HTU21D_BeginI2CTransfer(uint16_t direction, uint32_t eventMask) {
	int status;
	while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)) {
	}

	I2C_GenerateSTART(HTU21D_I2C, ENABLE);
	status = _HTU21D_WaitForEvent(I2C_EVENT_MASTER_MODE_SELECT);
	if (status) {
		return HTU21D_E_BUS_ERROR;
	}

	I2C_Send7bitAddress(HTU21D_I2C, HTU21D_I2C_ADDRESS << 1, direction);
	status = _HTU21D_WaitForEvent(eventMask);
	if (status) {
		if (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_AF)) {
			return HTU21D_E_ADDR_NACKED;
		} else {
			return HTU21D_E_BUS_ERROR;
		}
	}

	return HTU21D_E_OK;
}

int HTU21D_I2CBeginWrite() {
	return _HTU21D_BeginI2CTransfer(I2C_Direction_Transmitter, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
}

int HTU21D_I2CBeginRead() {
	I2C_AcknowledgeConfig(HTU21D_I2C, ENABLE);
	return _HTU21D_BeginI2CTransfer(I2C_Direction_Receiver, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
}

int HTU21D_I2CRead(uint8_t* buffer, size_t size) {
	while (size--) {
		if (!size) {
			I2C_AcknowledgeConfig(HTU21D_I2C, DISABLE);
		}

		if (_HTU21D_WaitForEvent(I2C_EVENT_MASTER_BYTE_RECEIVED)) {
			return HTU21D_E_BUS_ERROR;
		}

		*buffer++ = I2C_ReceiveData(HTU21D_I2C);
	}
	return HTU21D_E_OK;
}

int HTU21D_I2CWrite(uint8_t* buffer, size_t size) {
	while (size--) {
		I2C_SendData(HTU21D_I2C, *buffer++);
		if (_HTU21D_WaitForEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
			return HTU21D_E_BUS_ERROR;
		}
	}
	return HTU21D_E_OK;
}

int HTU21D_I2CStop() {
	I2C_GenerateSTOP(HTU21D_I2C, ENABLE);
	while (I2C_GetFlagStatus(HTU21D_I2C, I2C_FLAG_BUSY)) {
	}

	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_BERR);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_ARLO);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_AF);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_OVR);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_PECERR);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_TIMEOUT);
	I2C_ClearFlag(HTU21D_I2C, I2C_FLAG_SMBALERT);

	return HTU21D_E_OK;
}
