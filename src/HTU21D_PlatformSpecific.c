#include "HTU21D_PlatformSpecific.h"
#include "HTU21D.h"

void HTU21D_PlatformSpecificInit() {
	// init clocks, GPIOs and I2C Peripheral here
}

int HTU21D_I2CBeginWrite() {
	// this function must:
	// 1) generate start condition
	// 2) send 7bit address (address is avalaible using HTU21D_I2C_ADDRESS constant) 
	//    and direction bit set to 0. Note that on some platforms you must shift 
	//    address 1 bit left.
	// 3) return one of HTU21D_E_OK, HTU21D_E_BUS_ERROR or HTU21D_E_ADDR_NACKED

	// you must NOT:
	// - send any data expect address and direction bit
	// - generate STOP condition
	// - return HTU21D_E_OK in case of failure
}

int HTU21D_I2CBeginRead() {
	// this function must:
	// 1) generate start condition
	// 2) send 7bit address (address is avalaible using HTU21D_I2C_ADDRESS constant) 
	//    and direction bit set to 1. Note that on some platforms you must shift 
	//    address 1 bit left and or with direction bit.
	// 3) return one of HTU21D_E_OK, HTU21D_E_BUS_ERROR or HTU21D_E_ADDR_NACKED

	// you must NOT:
	// - send any data expect address and direction bit
	// - generate STOP condition
	// - return HTU21D_E_OK in case of failure
}

int HTU21D_I2CRead(uint8_t* buffer, size_t size) {
	// this function is called after HTU21D_I2CBeginRead() and must read from 
	// device specified amount of data. All bytes must be ACKed and last mus tbe NACKed.
	// function must return one HTU21D_E_OK in case of success or HTU21D_E_BUS_ERROR 
	// in case of any error on bus. Note that in case of 1 byte transfer that single 
	// byte is NACKed.

	// you must NOT:
	// - generate STOP condition
	// - return HTU21D_E_OK in case of failure
}

int HTU21D_I2CWrite(uint8_t* buffer, size_t size) {
	// this function is called after HTU21D_I2CBeginWrite() and must attemp to 
	// transfer specified amount of data. If some of byte is NACKed function must 
	// return HTU21D_E_BUS_ERROR and do not attemp to read data anymore. 
	// Function may update buffer even in case of failure. If all bytes are ACKed 
	// functions should return HTU21D_E_OK.

	// you must NOT:
	// - generate STOP condition
	// - return HTU21D_E_OK in case of failure
}

int HTU21D_I2CStop() {
	// this function must generate STOP condition and allways must return HTU21D_E_OK.
}