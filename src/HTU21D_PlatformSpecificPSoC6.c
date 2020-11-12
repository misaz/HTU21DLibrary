#include "HTU21D_PlatformSpecific.h"
#include "HTU21D.h"

static cy_stc_scb_i2c_context_t i2cContext;
static uint8_t i2cReadBuffer[128];
static uint8_t i2cWriteBuffer[128];
static int isCommunicating = 0;

void HTU21D_PlatformSpecificInit() {
    cy_rslt_t status;

    const cy_stc_scb_i2c_config_t i2cConfig =
    {
        .i2cMode   = CY_SCB_I2C_MASTER,
        .useRxFifo = false,
        .useTxFifo = true,
        .slaveAddress     = 0U,
        .slaveAddressMask = 0U,
        .acceptAddrInFifo = false,
        .ackGeneralAddr   = false,
        .enableWakeFromSleep = false,
        .enableDigitalFilter = false,
        .lowPhaseDutyCycle = 8U,
        .highPhaseDutyCycle = 8U,
    };

    status = Cy_SCB_I2C_Init(HTU21D_SCB, &i2cConfig, &i2cContext);

	Cy_SCB_I2C_SlaveConfigReadBuf(HTU21D_SCB, i2cReadBuffer, 128, &i2cContext);
	Cy_SCB_I2C_SlaveConfigWriteBuf(HTU21D_SCB, i2cWriteBuffer, 128, &i2cContext);

	Cy_GPIO_SetHSIOM(HTU21D_SCL_PORT, HTU21D_SCL_NUM, HTU21D_SCL_ALT);
	Cy_GPIO_SetHSIOM(HTU21D_SDA_PORT, HTU21D_SDA_NUM, HTU21D_SDA_ALT);
	Cy_GPIO_SetDrivemode(HTU21D_SCL_PORT, HTU21D_SCL_NUM, CY_GPIO_DM_OD_DRIVESLOW);
	Cy_GPIO_SetDrivemode(HTU21D_SDA_PORT, HTU21D_SDA_NUM, CY_GPIO_DM_OD_DRIVESLOW);

	status = Cy_SysClk_PeriphAssignDivider(HTU21D_SCB_CLOCK, HTU21D_SCB_CLOCK_DIVIDER_TYPE, HTU21D_SCB_CLOCK_DIVIDER_NUM);
	if (status != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

#if HTU21D_SCB_CLICK_DIVIDER_DO_CONFIGURATION == 1
	status = Cy_SysClk_PeriphSetDivider(HTU21D_SCB_CLOCK_DIVIDER_TYPE, HTU21D_SCB_CLOCK_DIVIDER_NUM, HTU21D_SCB_CLOCK_DIVIDER_VALUE - 1);
	if (status != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	status = Cy_SysClk_PeriphEnableDivider(HTU21D_SCB_CLOCK_DIVIDER_TYPE, HTU21D_SCB_CLOCK_DIVIDER_NUM);
	if (status != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}
#endif

	uint32_t inputClockFrequency = Cy_SysClk_PeriphGetFrequency(HTU21D_SCB_CLOCK_DIVIDER_TYPE, HTU21D_SCB_CLOCK_DIVIDER_NUM);

	uint32_t currentDataRate = Cy_SCB_I2C_SetDataRate(HTU21D_SCB, 100000, inputClockFrequency);
	if ((currentDataRate > 100000) || (currentDataRate == 0)) {
	    CY_ASSERT(0);
	}

	Cy_SCB_I2C_Enable(HTU21D_SCB);

}

int HTU21D_I2CBeginWrite() {
    cy_rslt_t status;

	if (!isCommunicating) {
		status = Cy_SCB_I2C_MasterSendStart(HTU21D_SCB, HTU21D_I2C_ADDRESS, CY_SCB_I2C_WRITE_XFER, 1000, &i2cContext);
	} else {
		status = Cy_SCB_I2C_MasterSendReStart(HTU21D_SCB, HTU21D_I2C_ADDRESS, CY_SCB_I2C_WRITE_XFER, 1000, &i2cContext);
	}

	if (status == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK) {
		isCommunicating = 0;
		return HTU21D_E_ADDR_NACKED;
	} else if (status != CY_RSLT_SUCCESS) {
		isCommunicating = 0;
		return HTU21D_E_BUS_ERROR;
	} else {
		isCommunicating = 1;
		return HTU21D_E_OK;
	}
}

int HTU21D_I2CBeginRead() {
	cy_rslt_t status;

	if (!isCommunicating) {
		status = Cy_SCB_I2C_MasterSendStart(HTU21D_SCB, HTU21D_I2C_ADDRESS, CY_SCB_I2C_READ_XFER, 1000, &i2cContext);
	} else {
		status = Cy_SCB_I2C_MasterSendReStart(HTU21D_SCB, HTU21D_I2C_ADDRESS, CY_SCB_I2C_READ_XFER, 1000, &i2cContext);
	}

	if (status == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK) {
		isCommunicating = 0;
		return HTU21D_E_ADDR_NACKED;
	} else if (status != CY_RSLT_SUCCESS) {
		isCommunicating = 0;
		return HTU21D_E_BUS_ERROR;
	} else {
		isCommunicating = 1;
		return HTU21D_E_OK;
	}

}

int HTU21D_I2CRead(uint8_t* buffer, size_t size) {
	cy_rslt_t status;

	while (size--) {
		cy_en_scb_i2c_command_t ack = CY_SCB_I2C_ACK;
		if (size == 0) {
			ack = CY_SCB_I2C_NAK;
		}
		status = Cy_SCB_I2C_MasterReadByte(HTU21D_SCB, ack, buffer++, 1000, &i2cContext);

		if (status != CY_RSLT_SUCCESS) {
			return HTU21D_E_BUS_ERROR;
		}
	}

	return HTU21D_E_OK;
}

int HTU21D_I2CWrite(uint8_t* buffer, size_t size) {
	cy_rslt_t status;

	while (size--) {
		status = Cy_SCB_I2C_MasterWriteByte(HTU21D_SCB, *buffer++, 1000, &i2cContext);

		if (status != CY_RSLT_SUCCESS) {
			return HTU21D_E_BUS_ERROR;
		}
	}

	return HTU21D_E_OK;
}

int HTU21D_I2CStop() {
    cy_rslt_t status;

	status = Cy_SCB_I2C_MasterSendStop(HTU21D_SCB, 1000, &i2cContext);

	isCommunicating = 0;

	if (status != CY_RSLT_SUCCESS) {
		return HTU21D_E_BUS_ERROR;
	} else {
		return HTU21D_E_OK;
	}
}
