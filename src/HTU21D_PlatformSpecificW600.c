#include "HTU21D_PlatformSpecific.h"
#include "HTU21D.h"

#include "wm_gpio_afsel.h"
#include "wm_i2c.h"

void HTU21D_PlatformSpecificInit() {
    wm_i2c_sda_config(HTU21D_SDA_PIN);
    wm_i2c_scl_config(HTU21D_SCL_PIN);
    tls_i2c_init(HTU21D_I2C_SPEED);
}

int HTU21D_I2CBeginWrite() {
    tls_i2c_write_byte(HTU21D_I2C_ADDRESS << 1, 1);

    int status = tls_i2c_wait_ack();
    if (status != WM_SUCCESS) {
        return HTU21D_E_ADDR_NACKED;
    }

    return HTU21D_E_OK;
}

int HTU21D_I2CBeginRead() {
    tls_i2c_write_byte((HTU21D_I2C_ADDRESS << 1) | 0x01, 1);

    int status = tls_i2c_wait_ack();
    if (status != WM_SUCCESS) {
        return HTU21D_E_ADDR_NACKED;
    }

    return HTU21D_E_OK;
}

int HTU21D_I2CRead(uint8_t* buffer, size_t size) {
    while (size-- > 1) {
        *buffer++ = tls_i2c_read_byte(1, 0);
    }
    *buffer++ = tls_i2c_read_byte(0, 0);

    return HTU21D_E_OK;
}

int HTU21D_I2CWrite(uint8_t* buffer, size_t size) {
    while (size--) {
        tls_i2c_write_byte(*buffer++, 0);

        int status = tls_i2c_wait_ack();
        if (status != WM_SUCCESS) {
            return HTU21D_E_BUS_ERROR;
        }
    }

	return HTU21D_E_OK;
}

int HTU21D_I2CStop() {
	tls_i2c_stop();
	return HTU21D_E_OK;
}
