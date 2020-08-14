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
	
	// you must measure temperature or humidity before calling EndOfbattery
	// otherwise you read old and outdated information. End of battery is
	// updated after every measurement. If you need to improve speed It is
	// recommended to decrease resolution because it decreases time of
	// measurement and end of battery become available earlier.
	float dummy;
	if (HTU21D_ReadTemperature(&dummy) != 0) {
		UART_PrintString("Error while reading temperature.\r\n");
		goto infloop;
	}
	
	uint8_t isEndOfBattery;
	if (HTU21D_GetEndOfBattery(&isEndOfBattery) == 0) {
		UART_PrintString("End of battery: ");
		if (isEndOfBattery) {
			UART_PrintString("YES\r\n");
		} else {
			UART_PrintString("NO\r\n");
		}
	} else {
		UART_PrintString("Error while reading end of battery status.\r\n");
	}
	
infloop:
	while (1) {}
}