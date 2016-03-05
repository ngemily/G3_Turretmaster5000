/*
 * main.h
 *
 *  Created on: Feb 6, 2016
 *      Author: papayne
 */

#ifndef MAIN_H_
#define MAIN_H_
#include <stdbool.h>
#include "xintc.h"
#include "xgpio.h"



#define SD_GPIO_CARD_PRESENT_MASK (0x00000001)
#define SD_GPIO_DEVICE_ID   XPAR_GPIO_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID


extern XIntc InterruptController;
extern XGpio SdGpio;

#endif /* MAIN_H_ */
