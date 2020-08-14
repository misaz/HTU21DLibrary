#include "HTU21D.h"

#include <avr/io.h>

void UART_Init() {
	UBRR1H = 0;
	UBRR1L = 103;
	UCSR1B |= (1<<TXEN1);
	UCSR1C |= (1<<UCSZ10) | (1<<UCSZ11);
}

void UART_PrintString(char* message) {
	for (int i = 0; i < strlen(message); i++) {
		while (!(UCSR1A & (1<<UDRE1)));
		UDR1 = message[i];
	}
}

void UART_PrintFloat(float number) {
	unsigned char buff[32];
	dtostrf(number, 6, 2, buff);
	UART_PrintString(buff);
}

int main(void) {
	HTU21D_Init();
	
	UART_Init();
	UART_PrintString("INIT OK\r\n");
	
	if (HTU21D_ReadTemperatureNonBlockingBegin() != 0) {
		UART_PrintString("Error while beginning non-blocking operation.\r\n");
	}

	// Sensor is busy so you can do any other communication
	// on the same bus while measurement is in progress.
	
	// Write code communicating with other device on the same
	// bus here.

	float temperature;
	while (1) {
		int status = HTU21D_ReadTemperatureNonBlockingPoll(&temperature);

		if (status == HTU21D_E_OK) {
			// temperature is loaded so we can exit loop
			break;
		} else if (status == HTU21D_E_ADDR_NACKED) {

			// Sensor is still busy so you can do any other communication
			// on the same bus while measurement is in progress.
			
			// Write code communicating with other device on the same
			// bus here.

			continue;
		} else {
			UART_PrintString("Error while polling measured data.\r\n");
		}
	}
	
	UART_PrintString("Temperature: ");
	UART_PrintFloat(temperature);
	UART_PrintString("\r\n");
			
	while (1) {}
}