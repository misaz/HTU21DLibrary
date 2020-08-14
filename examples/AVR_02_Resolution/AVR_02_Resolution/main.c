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
	
	uint8_t resolution;
	if (HTU21D_GetResolution(&resolution) == 0) {
		switch (resolution) {
			case HTU21D_REG_RESOLUTION_T11_H11:
				UART_PrintString("Current resolution: 11bit temperature and 11bit humidity\r\n");
				break;
			case HTU21D_REG_RESOLUTION_T12_H08:
				UART_PrintString("Current resolution: 12bit temperature and 8bit humidity\r\n");
				break;
			case HTU21D_REG_RESOLUTION_T13_H10:
				UART_PrintString("Current resolution: 13bit temperature and 10bit humidity\r\n");
				break;
			case HTU21D_REG_RESOLUTION_T14_H12:
				UART_PrintString("Current resolution: 14bit temperature and 12bit humidity\r\n");
				break;
		}
	} else {
		UART_PrintString("Error while getting current resolution.\r\n");
	}

	if (HTU21D_SetResolution(HTU21D_REG_RESOLUTION_T11_H11) != 0) {
		UART_PrintString("Error while setting resolution.\r\n");
	}
	
	float temperature;
	if (HTU21D_ReadTemperature(&temperature) == 0) {
		UART_PrintString("Temperature: ");
		UART_PrintFloat(temperature);
		UART_PrintString("\r\n");
	} else {
		UART_PrintString("Error while reading temperature.\r\n");
	}
	
	while (1) {}
}