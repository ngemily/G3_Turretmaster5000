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
#include "diskio/diskio.h"
#include "diskio/ff.h"
#include "motorcontrol/motorcontrol.h"

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

#define SD_GPIO_CARD_PRESENT_MASK (0x00000001)
#define SD_GPIO_DEVICE_ID   XPAR_GPIO_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define SD_GPIO_CHANNEL	    1


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#if (!defined(DEBUG))
extern void xil_printf(const char *format, ...);
#endif

static int SetupSdGpio();
static bool SdCardInserted();

static void LowLevelTest();
static void HighLevelTest();

static void LaserTest();
static void MotorPatternTest();
static void stopTest();


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
 };

 static TCHAR Buff[100];
 static volatile bool continueTest;

 void test_fcn(void) {
	 xil_printf("test %08x\n\r", 0x123ABC);
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


 void uart_rec_audio(void) {
	 rec_audio();
 }


 void uart_play_audio(void) {
	 play_audio();
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
	status |= SetupSdGpio(&sGpio);

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

	// Commands to run self-tests
	register_uart_response('l', LowLevelTest);
	register_uart_response('h', HighLevelTest);
	register_uart_response('z', LaserTest);
	register_uart_response('m', MotorPatternTest);
	register_uart_response('s', stopTest);


	xil_printf("\r\n--- Done registering UART commands --- \r\n");
	xil_printf("%08x\r\n", 0xDEADBEEF);

	while (do_run) {
		MB_Sleep(1000);
	}

	xil_printf("\r\n--- Exiting main() --- \r\n");

	return XST_SUCCESS;
}


/***************** Static Function Definitions *********************/

static int SetupSdGpio(XGpio *sGpio) {
	int Status;

	// Initialize the GPIO driver. If an error occurs then exit.
	Status = XGpio_Initialize(sGpio, SD_GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to initialize SD GPIO driver: %d\n\r", Status);
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs.
	XGpio_SetDataDirection(sGpio, SD_GPIO_CHANNEL, SD_GPIO_CARD_PRESENT_MASK);

	XGpio_SelfTest(sGpio);

	// TODO: add Interrupt configuration code.

	return XST_SUCCESS;
}


static inline bool SdCardInserted() {
    int RegRead = XGpio_DiscreteRead(&sGpio, SD_GPIO_CHANNEL);
    return (RegRead & SD_GPIO_CARD_PRESENT_MASK) == 0;
}


static void LowLevelTest() {
	u8 TestBuffer[2048];

    // Try to initialize the SD card.
    if (disk_initialize(0)) {
    	xil_printf("Failed to initialize SD card.\n\r");
    	return;
    }

    xil_printf("SD card initialized.\n\r");

  	if (disk_read(0, TestBuffer, 0, 4) != RES_OK) {
    	xil_printf("Failed to read first sector.\n\r");
    	return;
  	}

  	for (int i = 0; i < 128; i++) {
  		for (int j = 0; j < 4; j++) {
  			xil_printf("%02x ", TestBuffer[4 * i + j]);
  		}
  		xil_printf("\n\r");
  	}
}


static void HighLevelTest() {
	FATFS FatFs;
	FIL FHandle;
	unsigned int BytesWritten;

	if (f_mount(&FatFs, "", 0) != FR_OK) {
    	xil_printf("Failed to mount filesystem.\n\r");
    	return;
	}

	if (f_open(&FHandle, "foo.txt", FA_READ) != FR_OK) {
    	xil_printf("Failed to open foo.txt.\n\r");
		return;
	}

	while (!f_eof(&FHandle)) {
		if (f_gets(Buff, 100, &FHandle) == NULL) {
			xil_printf("Failed to read a line from foo.txt.\r\n");
			return;
		}

		xil_printf("%s", Buff);
	}

	f_close(&FHandle);

	if (f_open(&FHandle, "bar.txt", FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
    	xil_printf("Failed to open bar.txt.\n\r");
		return;
	}

	if (f_write(&FHandle, "Hello!\r\n", 8, &BytesWritten) != FR_OK) {
    	xil_printf("Failed to write to bar.txt.\n\r");
		return;
	}

	if (BytesWritten != 8) {
		xil_printf("Wrote incorrect number of bytes to bar.txt!\n\r");
		return;
	}

	f_close(&FHandle);

	f_mount(NULL, "", 0);

	xil_printf("Test Successful!\n\r");
}


static void LaserTest() {
	InitMotorBoard();
	continueTest = true;

	while (continueTest) {
		TurnOnLaser();
		MB_Sleep(1000);
		TurnOffLaser();
		MB_Sleep(1000);
	}
}


static void MotorPatternTest() {
	InitMotorBoard();
	TurnOnLaser();
	EnablePanServo();
	EnableTiltServo();

	continueTest = true;

	while (continueTest) {
		for (int i = -60; i < 0; i++) {
			SetPanAngle(i);
			SetTiltAngle(i + 60);
			MB_Sleep(15);
		}
		MB_Sleep(20);

		for (int i = 0; i < 60; i++) {
			SetPanAngle(i);
			SetTiltAngle(60 - i);
			MB_Sleep(15);
		}
		MB_Sleep(20);

		for (int i = 60; i > 0; i--) {
			SetPanAngle(i);
			SetTiltAngle(i - 60);
			MB_Sleep(15);
		}
		MB_Sleep(20);

		for (int i = 0; i > -60; i--) {
			SetPanAngle(i);
			SetTiltAngle(-60 - i);
			MB_Sleep(15);
		}
		MB_Sleep(20);

	}
}


static void stopTest() {
	continueTest = false;
}




