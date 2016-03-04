/******************************************************************************
*
* (c) Copyright 2010-2013 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
 * ***************************************************************************
 */

#include <stdbool.h>
#include "demo.h"

#include "audio/audio.h"
#include "dma/dma.h"
#include "intc/intc.h"
#include "iic/iic.h"

#include "platform.h"
#include "audio_engine.h"
#include "uart_engine.h"

/***************************** Include Files *********************************/

#include <xaxidma.h>
#include <xparameters.h>
#include <xil_exception.h>
#include <xdebug.h>
#include <xiic.h>
#include <xaxidma.h>
#include <xuartlite.h>
#include "xuartlite_l.h"
#include "xgpio.h"

#ifdef XPAR_INTC_0_DEVICE_ID
 #include "xintc.h"
#else
 #include "xscugic.h"
#endif

/************************** Constant Definitions *****************************/

/*
 * Device hardware build related constants.
 */

// Audio constants
// Number of seconds to record/playback
#define NR_SEC_TO_REC_PLAY		5

// ADC/DAC sampling rate in Hz
#define AUDIO_SAMPLING_RATE	  96000

// Number of samples to record/playback
#define NR_AUDIO_SAMPLES		(NR_SEC_TO_REC_PLAY*AUDIO_SAMPLING_RATE)

/* Timeout loop counter for reset
 */
#define RESET_TIMEOUT_COUNTER	10000

#define TEST_START_VALUE	0x0

#define GPIO_DEVICE_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define BUTTON_INT_MASK XPAR_AXI_GPIO_0_IP2INTC_IRPT_MASK


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#if (!defined(DEBUG))
extern void xil_printf(const char *format, ...);
#endif


/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */

static XIic      sIic;
static XAxiDma   sAxiDma;
static XIntc     sIntc;
static XUartLite sUartLite;
static XGpio sGpio;

volatile int numInterrupts;

 // Interrupt vector table
 const ivt_t ivt[] = {
 	//IIC
 	{XPAR_MICROBLAZE_0_AXI_INTC_AXI_IIC_0_IIC2INTC_IRPT_INTR, (XInterruptHandler)XIic_InterruptHandler, &sIic},
 	//DMA Stream to MemoryMap Interrupt handler
 	{XPAR_MICROBLAZE_0_AXI_INTC_AXI_DMA_0_S2MM_INTROUT_INTR,   (XInterruptHandler)fnS2MMInterruptHandler, &sAxiDma},
 	//DMA MemoryMap to Stream Interrupt handler
 	{XPAR_MICROBLAZE_0_AXI_INTC_AXI_DMA_0_MM2S_INTROUT_INTR,   (XInterruptHandler)fnMM2SInterruptHandler, &sAxiDma},
 	//UART Lite Interrupt handler
 	{XPAR_MICROBLAZE_0_AXI_INTC_AXI_UARTLITE_0_INTERRUPT_INTR, (XInterruptHandler)XUartLite_InterruptHandler, &sUartLite},

 	//{XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_0_IP2INTC_IRPT_INTR, (XInterruptHandler)XIntc_InterruptHandler, &sUartLite},
 };

 void test_fcn(void) {
	 xil_printf("test\n\r");
 }

 static bool do_run = true;
 void end_fcn(void) {
	 do_run = 0;
 }

 void dump_mem(void) {
    int i;
    char *ptr = (char *) MEM_BASE_ADDR;
    for (i=0; i<100000; i++) {
        XUartLite_SendByte(sUartLite.RegBaseAddress, *(ptr+i));
    }
 }


 inline void uart_rec_audio(void) {
	 rec_audio();
 }


 inline void uart_play_audio(void) {
	 play_audio();
 }


 void GpioIsr(void *InstancePtr) {
	XGpio *gpio = (XGpio *)InstancePtr;
	XGpio_InterruptDisable(gpio, BUTTON_INT_MASK);

	// Clear the interrupt such that it is no longer pending in the GPIO
	XGpio_InterruptClear(gpio, BUTTON_INT_MASK);

	xil_printf("GPIO interrupt!\r\n");

	numInterrupts += 1;

	// Enable the interrupt
	XGpio_InterruptEnable(gpio, BUTTON_INT_MASK);
 }


 XStatus initGpio(XGpio *gpio) {
	int status;

	xil_printf("Initialize GPIO.");

	/* Initialize the GPIO driver. If an error occurs then exit */

	status = XGpio_Initialize(gpio, GPIO_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test on the GPIO.  This is a minimal test and only
	 * verifies that there is not any bus error when reading the data
	 * register
	 */
	XGpio_SelfTest(gpio);

	XGpio_SetDataDirection(gpio, 1, ~0);

	/* Hook up interrupt service routine */
	XIntc_Connect(&sIntc, XPAR_INTC_0_GPIO_0_VEC_ID,
		      (Xil_ExceptionHandler)GpioIsr, gpio);

	/* Enable the interrupt vector at the interrupt controller */

	XIntc_Enable(&sIntc, XPAR_INTC_0_GPIO_0_VEC_ID);

	XGpio_InterruptEnable(gpio, BUTTON_INT_MASK);
	XGpio_InterruptGlobalEnable(gpio);

	numInterrupts = 0;

	return XST_SUCCESS;
 }

/*****************************************************************************/
/**
*
* Main function
*
* This function is the main entry of the interrupt test. It does the following:
*	Initialize the audio
*	Initialize the debug uart
*	Enable the interrupts
*
* @param	None
*
* @return
*		- XST_SUCCESS if example finishes successfully
*		- XST_FAILURE if example fails.
*
* @note		None.
*
******************************************************************************/
int main(void)
{
    init_platform();

	xil_printf("\r\n--- Entering main() --- \r\n");

	int status = XST_SUCCESS;
	status |= initialize_audio(&sIic, &sAxiDma);
	status |= fnInitInterruptController(&sIntc);
	status |= initialize_debug(&sUartLite);
	status |= initGpio(&sGpio);

	if (status != XST_SUCCESS) {
		xil_printf("Failed to initialize system.\r\n");
		return XST_FAILURE;
	}

	fnEnableInterrupts(&sIntc, &ivt[0], sizeof(ivt)/sizeof(ivt[0]));

	register_uart_response('t', test_fcn);
	register_uart_response('r', uart_rec_audio);
	register_uart_response('p', uart_play_audio);
	register_uart_response('k', end_fcn);
	register_uart_response('e', end_fcn);
	register_uart_response('d', dump_mem);

	xil_printf("\r\n--- Done registering UART commands --- \r\n");

	while (do_run) {
		MB_Sleep(1000);
		xil_printf("int count: %d\r\n", numInterrupts);
	}

	xil_printf("\r\n--- Exiting main() --- \r\n");

	return XST_SUCCESS;
}









