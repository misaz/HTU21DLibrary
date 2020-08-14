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
	
	
	if (HTU21D_DisableOnChipHeadere() != 0) {
		UART_PrintString("Error while disabling on-chip heater.\r\n");
		goto infloop;
	}
	if (HTU21D_SetResolution(HTU21D_REG_RESOLUTION_T12_H08) != 0) {
		UART_PrintString("Error while setting resolution.\r\n");
		goto infloop;
	}

	// some delay
	for (long i = 0; i < 1000000; i++) {}

	float tempBefore;
	if (HTU21D_ReadTemperature(&tempBefore) != 0) {
		UART_PrintString("Error while reading temperature.\r\n");
		goto infloop;
	}
	if (HTU21D_EnableOnChipHeater() != 0) {
		UART_PrintString("Error while enabling on-chip heater.\r\n");
		goto infloop;
	}

	// some delay
	for (long i = 0; i < 1000000; i++) {}
	
	float tempAfter;
	if (HTU21D_ReadTemperature(&tempAfter) != 0) {
		UART_PrintString("Error while reading temperature.\r\n");
		goto infloop;
	}
	if (HTU21D_DisableOnChipHeadere() != 0) {
		UART_PrintString("Error while disabling on-chip heater.\r\n");
		goto infloop;
	}

	float heaterTemperatureIncrease = tempAfter - tempBefore;
	
	UART_PrintString("Temperature before: ");
	UART_PrintFloat(tempBefore);
	UART_PrintString("\r\nTemperature after: ");
	UART_PrintFloat(tempAfter);
	UART_PrintString("\r\nTemperature difference: ");
	UART_PrintFloat(heaterTemperatureIncrease);
	UART_PrintString("\r\n");
	
infloop:
	while (1) {}
}