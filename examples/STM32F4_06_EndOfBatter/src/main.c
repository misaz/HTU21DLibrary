#include "stm32f4xx.h"

#include "HTU21D.h"

#include <string.h>
#include <stdio.h>

void UART_Init() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_UART8);

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_1;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &gpio);

	USART_InitTypeDef uart;
	uart.USART_BaudRate = 115200;
	uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart.USART_Parity = USART_Parity_No;
	uart.USART_StopBits = USART_StopBits_1;
	uart.USART_WordLength = USART_WordLength_8b;
	USART_Init(UART8, &uart);

	USART_Cmd(UART8, ENABLE);
}

void UART_PrintString(char* message) {
	for (int i = 0; i < strlen(message); i++) {
		while (!USART_GetFlagStatus(UART8, USART_FLAG_TXE)) {
		}
		USART_SendData(UART8, message[i]);
	}
}

void UART_PrintFloat(float number) {
	char buff[32];
	sprintf(buff, "%f", number);
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
