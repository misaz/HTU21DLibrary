#include "HTU21D_PlatformSpecific.h"
#include "HTU21D.h"

uint8_t _HTU21D_WaitForStatusChangeAndGetStatus() {
	while (!(TWCR & (1 << TWINT))) {
	}
	return TWSR;
}

void HTU21D_PlatformSpecificInit() {
	// exit TWI from power saving mode (=stopped). 
	// It is default but for case when user change that.
	PRR0 |= PRTWI;

	// if F_CPU=16MHz, than SCL freq is 400 kHz
	// if F_CPU=8MHz,  than SCL freq is 200 kHz
	// if F_CPU=1MHz,  than SCL freq is 25 kHz
	TWBR = 12;

	// set ports mode
	HTU21D_SDA_DDRx &= ~HTU21D_SDA_PIN_MASK;
	HTU21D_SCL_DDRx &= ~HTU21D_SCL_PIN_MASK;

	// enable internal pullups
	HTU21D_SDA_PORTx |= HTU21D_SDA_PIN_MASK;
	HTU21D_SCL_PORTx |= HTU21D_SCL_PIN_MASK;
}

int HTU21D_I2CBeginWrite() {
	uint8_t status;

	// generate start condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	status = _HTU21D_WaitForStatusChangeAndGetStatus();
	if (status != HTU21D_TWI_STATUS_START_TRANSMITTED &&
		status != HTU21D_TWI_STATUS_REPEATED_START_TRANSMITTED) {

		return HTU21D_E_BUS_ERROR;
	}

	// send address
	TWDR = (HTU21D_I2C_ADDRESS << 1);
	TWCR = (1 << TWINT) | (1 << TWEN);
	status = _HTU21D_WaitForStatusChangeAndGetStatus();
	if (status == HTU21D_TWI_STATUS_ADDR_WRITE_ACK) {
		return HTU21D_E_OK;
	} else if (status == HTU21D_TWI_STATUS_ADDR_WRITE_NACK) {
		return HTU21D_E_ADDR_NACKED;
	} else {
		return HTU21D_E_BUS_ERROR;
	}
}

int HTU21D_I2CBeginRead() {
	uint8_t status;

	// generate start condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	status = _HTU21D_WaitForStatusChangeAndGetStatus();
	if (status != HTU21D_TWI_STATUS_START_TRANSMITTED &&
		status != HTU21D_TWI_STATUS_REPEATED_START_TRANSMITTED) {

		return HTU21D_E_BUS_ERROR;
	}

	// send address
	TWDR = (HTU21D_I2C_ADDRESS << 1) | 0x01;
	TWCR = (1 << TWINT) | (1 << TWEN);
	status = _HTU21D_WaitForStatusChangeAndGetStatus();
	if (status == HTU21D_TWI_STATUS_ADDR_READ_ACK) {
		return HTU21D_E_OK;
	} else if (status == HTU21D_TWI_STATUS_ADDR_READ_NACK) {
		return HTU21D_E_ADDR_NACKED;
	} else {
		return HTU21D_E_BUS_ERROR;
	}
}

int HTU21D_I2CRead(uint8_t* buffer, size_t size) {
	uint8_t status;

	while (size--) {
		int isAcked = size != 0; // here is zero because value is already decremented
		uint8_t expectedStatus = isAcked ? HTU21D_TWI_STATUS_DATA_READ_ACKED : HTU21D_TWI_STATUS_DATA_READ_NACKED;

		// read byte
		TWCR = (1 << TWINT) | (1 << TWEN) | ((isAcked ? 1 : 0) << TWEA);
		status = _HTU21D_WaitForStatusChangeAndGetStatus();
		if (status != expectedStatus) {
			return HTU21D_E_BUS_ERROR;
		}

		*buffer++ = TWDR;
	}

	return HTU21D_E_OK;
}

int HTU21D_I2CWrite(uint8_t* buffer, size_t size) {
	uint8_t status;

	while (size--) {
		TWDR = *buffer++;
		TWCR = (1 << TWINT) | (1 << TWEN);
		status = _HTU21D_WaitForStatusChangeAndGetStatus();
		if (status != HTU21D_TWI_STATUS_DATA_WRITE_ACK) {
			return HTU21D_E_BUS_ERROR;
		}
	}

	return HTU21D_E_OK;
}

int HTU21D_I2CStop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO)) {
	}
	return HTU21D_E_OK;
}