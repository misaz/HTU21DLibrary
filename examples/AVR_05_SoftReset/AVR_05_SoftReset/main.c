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
	
	if (HTU21D_SoftReset() == 0) {
		UART_PrintString("Sensor reseted.\r\n");
	} else {
		UART_PrintString("Error while reseting sensor.\r\n");
	}
	
	while (1) {}
}