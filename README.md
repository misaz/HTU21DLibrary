# HTU21D Library

This project contains library for HTU21D sensor. Library is multiplatform and independent on any other library.

## Features

- Support for reading temperature and humidity from sensor (see example #1).
- Support for both holding and no-holding modes of operations (see description of example #3).
- Support for selecting resolution (see example #2).
- Support for manually triggering measurement and using I2C bus for different communication while measurement is in progress (see example #3).
- Support for enabling on-chip heater (see example #4).
- Support for software reseting sensor (see example #5).
- Support for End of batery sensing feature (see example #6).
- Support for CRC velidation of received data

## Deploy
When used It consists of 4 files. Files `HTU21D.c` and `HTU21D.h` contains library itself. There are declared and defined library functions needded to manipulate connected microcontroller. In your app include only `HTU21D.h` file. `HTU21D.c` and `HTU21D.h` files depends on platform specific implementation of I2C access and time delay. Platform specific implementation is provided in `HTU21D_PlatformSpecific.c` and `HTU21D_PlatformSpecific.h` files. Default files contains empty implementation which does nothing. If you want to use it on supported platform (AVR or STM32f4 with StdPeriph library) you need to delete `HTU21D_PlatformSpecific.c` and `HTU21D_PlatformSpecific.h` files and rename files for your platform to correct names. For example for AVR you need to rename `HTU21D_PlatformSpecificAVR.c` to `HTU21D_PlatformSpecific.c` and `HTU21D_PlatformSpecificAVR.h` to `HTU21D_PlatformSpecific.h` (remove `_AVR` suffix from name). You need to also delete platform specific files for other platforms.

## Examples

Examples are implemented for all platform in example folder. Examples on this page are platform neutral.

### 1 - Read Temperature and Humidity

Following example is example of very simple (and the most common) usage of library. Library is initialized using `HTU21D_Init()` function and measurements are triggered using `HTU21D_ReadTemperature()` and `HTU21D_ReadHumidity` functions. Functions expect pointer to float where the measured value will be stored. Most functions in library returns integers containing status information. 0 is OK and non-zero value identifies error. Error codes are defined in `HTU21D.h` file in constants with names starting with `HTU21D_E_`.

```
#include "HTU21D.h"

int main(void) {
	float temperature;
	float humidity;
	
	HTU21D_Init();

	if (HTU21D_ReadTemperature(&temperature) == 0) { // success
		// print temperature here
	} else { // error
		// print error here
	}
	
	if (HTU21D_ReadHumidity(&humidity) == 0) { // success
		// print temperature here
	} else { // error
		// print error here
	}
	
	while (1) {}
}
```

### 2 - Specifiy resolution

Following example shows usage of `HTU21D_GetResolution` and `HTU21D_SetResolution` functions. Better resolution decrease performance. Sensor support resolutions defined in constants `HTU21D_REG_RESOLUTION_T11_H11`, `HTU21D_REG_RESOLUTION_T12_H08`, `HTU21D_REG_RESOLUTION_T13_H10`, `HTU21D_REG_RESOLUTION_T14_H12`. Constant name says how much bits are used for temperature and humidity. For example mode `HTU21D_REG_RESOLUTION_T13_H10` says that temperature will be measured as 13 bit number and humidity as 10 bit number.

```
#include "HTU21D.h"

int main(void) {
	HTU21D_Init();
	
	uint8_t resolution;
	if (HTU21D_GetResolution(&resolution) == 0) {
		switch (resolution) {
			case HTU21D_REG_RESOLUTION_T11_H11:
				// print "11bit temperature and 11bit humidity"
				break;
			case HTU21D_REG_RESOLUTION_T12_H08:
				// print "12bit temperature and 8bit humidity"
				break;
			case HTU21D_REG_RESOLUTION_T13_H10:
				// print "13bit temperature and 10bit humidity"
				break;
			case HTU21D_REG_RESOLUTION_T14_H12:
				// print "14bit temperature and 12bit humidity"
				break;
		}
	} else {
		// print error
	}

	if (HTU21D_SetResolution(HTU21D_REG_RESOLUTION_T11_H11) != 0) {
		// print error
	}
	
	float temp1;
	if (HTU21D_ReadTemperature(&temp1) == 0) {
		// print temperature
	} else {
		// print error
	}
	
	while (1) {}
}
```

### 3 - Measurement in bus non-blocking mode

Sensor supports measuring in 2 modes. One is blocking which works in way that when controllers triggers measurement (using `HTU21D_ReadTemperature` or `HTU21D_ReadTemperatureBlocking` functions, both do the same) it holds I2C bus low so master must wait until bus is released, then it can read measured value from sensor. Other mode is that the master send command to tirgger measure and device imediately release bus. Then master must attemp to read measured data from sensor. If it do that early when value is not measured yet sensor will deny that request and master must try it again later. Benefit of that mode is that in time when sensor is measuring value bus could be used to communicate with other node on the bus. This mode is used by `HTU21D_ReadTemperatureNonBlocking` function but this blocks your program between attemps to read dat from sensor. If you want to use it fully you need to call begin (`HTU21D_ReadTemperatureNonBlockingBegin`) function to execute non-blocking request and then call poll (`HTU21D_ReadTemperatureNonBlockingPoll`) function until it succeds. Between begin and polls you can communicate with other nodes on the bus. Pointer to float where value will be sotred is passed into poll (not begin) method and it is filled only in case of poll success (when it return 0). It is reccommended to distinguis between NACK and BUS error. NACK error is normal in this mode and means that device is not ready to provide data while BUS error si any other error. In case of BUS error BUS is reseted and transaction cannot be considered as valid anymore. Usage of described non-blocking mode is shown on the following example.

Also note that HTU21D_Init fully initialize I2C bus and peripheral of bus master. If you have multiple sensors on one bus you probably need initialize bus only once. It is recommend to verify that library for other node on the bus initialize that bus properly. If it does you do not need to call HTU21D_Init.

```
#include "HTU21D.h"

int main(void) {
	HTU21D_Init();

	if (HTU21D_ReadTemperatureNonBlockingBegin() != 0) {
		// print error
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
			// error (bus error, CRC error, ...)
		}
	}

	// print temperature
	while (1) {}
}
```

### 4 - On-chip heater

Sensor contains heater which could be enabled and used for test that sensor is working. Heater increase temperature about approximaly 0.5 - 1.5 °C. Following example measure temperature before enabling heater, after enabling and shows how much heater increased temperature.

```
#include "HTU21D.h"

int main(void) {
	HTU21D_Init();

	if (HTU21D_DisableOnChipHeadere() != 0) {
		// print error
	}
	if (HTU21D_SetResolution(HTU21D_REG_RESOLUTION_T12_H08) != 0) {
		// print error
	}

	// some delay
	for (int i = 0; i < 10000000; i++) {}

	float tempBefore;
	if (HTU21D_ReadTemperature(&tempBefore) != 0) {
		// print error
	}
	if (HTU21D_EnableOnChipHeater() != 0) {
		// print error
	}

	// some delay
	for (int i = 0; i < 10000000; i++) {}

	
	float tempAfter;
	if (HTU21D_ReadTemperature(&tempAfter) != 0) {
		// print error
	}
	if (HTU21D_DisableOnChipHeadere() != 0) {
		// print error
	}

	float heaterTemperatureIncrease = tempAfter - tempBefore;
	// print heaterTemperatureIncrease
	
	while (1) {}
}
```

### 5 - Soft reset

Sensor could be reseted using software. Library provides this feature using `HTU21D_SoftReset` function. It is reccomended to use this function in time when error on bus occure. Because on some bus error sensore may becaome unresponsive it is recommended to call this function in loop while it returns error. When it succeeds sensor is avalaible and ready again.

```
#include "HTU21D.h"

int main(void) {
	HTU21D_Init();
	
	if (HTU21D_SoftReset() != 0) {
		// print error
	}
	
	while (1) {}
}
```

### 6 - End of battery

Sensor supports providing informations if voltage going low. Feature is named "End of battery". This flag is updated after every measurement and is active when voltage drops under aproximately 2.25 V. It could be read using `HTU21D_GetEndOfBattery` function.

```
#include "HTU21D.h"

int main(void) {
	HTU21D_Init();
	
	// you must measure temperature or humidity before calling EndOfbattery 
	// otherwise you read old and outdated information. End of battery is 
	// updated after every measurement. If you need to improve speed It is 
	// recommended to decrease resolution because it decreases time of 
	// measurement and end of battery become available earlier.
	float dummy;
	if (HTU21D_ReadTemperature(&dummy) != 0) {
		// print error
	}
	
	uint8_t isEndOfBattery;
	if (HTU21D_GetEndOfBattery(&isEndOfBattery) == 0) {
		// print end of battery
	} else {
		// print error
	}
	
	while (1) {}
}
```

## Disclaimer

Library is provided without any warranty. Use it on your own risk.

## Questions?

If you have any question you can ask simply by creating issue.