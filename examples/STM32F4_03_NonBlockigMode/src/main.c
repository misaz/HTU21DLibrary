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

	if (HTU21D_ReadTemperatureNonBlockingBegin() != 0) {
		UART_PrintString("Error while beginning non-blocking operation.\r\n");
		goto infloop;
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
			goto infloop;
		}
	}

	UART_PrintString("Temperature: ");
	UART_PrintFloat(temperature);
	UART_PrintString("\r\n");

infloop:
	while (1) {
	}
}
