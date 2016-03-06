/*
 * This program just probes the SD card and checks that it is functioning, using the SPI interface.
 */

// First include the BSP headers and standard libraries.
#include "xil_printf.h"
#include "xparameters.h"
#include "xspi.h"
#include "xintc.h"
#include "xil_exception.h"
#include "xgpio.h"
#include "microblaze_sleep.h"

// Now include project-specific headers.
#include "main.h"
#include "platform.h"
#include "diskio.h"
#include "ff.h"
#include "motorcontrol.h"


// Function Declarations
static int SetupInterruptSystem();
int SetupSdGpio();
bool SdCardInserted();

static void LowLevelTest();
static void HighLevelTest();

static void LaserTest();
static void MotorPatternTest();

/*
 * The instances to support the device drivers are global such that the
 * are initialized to zero each time the program runs.  They could be local
 * but should at least be static so they are zeroed.
 */
XIntc InterruptController;
XGpio SdGpio;

static TCHAR Buff[100];

int SetupSdGpio() {
	int Status;

	// Initialize the GPIO driver. If an error occurs then exit.
	Status = XGpio_Initialize(&SdGpio, SD_GPIO_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		xil_printf("Failed to initialize SD GPIO driver: %d\n\r", Status);
		return XST_FAILURE;
	}

	// Set the direction for all signals to be inputs.
	XGpio_SetDataDirection(&SdGpio, SD_GPIO_CHANNEL, SD_GPIO_CARD_PRESENT_MASK);

	XGpio_SelfTest(&SdGpio);

	// TODO: add Interrupt configuration code.

	return XST_SUCCESS;
}

int main() {
	// Initialize all peripherals and internal state.
    init_platform();

    if (SetupInterruptSystem() != XST_SUCCESS) {
    	xil_printf("Failed to initialize Interrupts.\n\r");
    	return -1;
    }

    if (SetupSdGpio() != XST_SUCCESS) {
    	xil_printf("Failed to initialize the SD card's GPIO.\n\r");
    	return -1;
    }

    print("Platform Loaded.\n\r");

    // Check for presence of SD card.
    if (!SdCardInserted()) {
    	xil_printf("SD card not inserted! Derp!\n\r");
    	xil_printf("Please insert SD card...\n\r");
        while (!SdCardInserted()) {
        	MB_Sleep(100);
        }
    }

    xil_printf("SD card inserted.\n\r");

	MB_Sleep(500);

    //LowLevelTest();
    //HighLevelTest();
	//LaserTest();
	MotorPatternTest();

    while(1);

    cleanup_platform();
    return 0;
}


inline bool SdCardInserted() {
    int RegRead = XGpio_DiscreteRead(&SdGpio, SD_GPIO_CHANNEL);
    return (RegRead & SD_GPIO_CARD_PRESENT_MASK) == 0;
}


/****************************************************************************
* This function sets up the interrupt system such that interrupts can occur.
*
* @return	XST_SUCCESS if successful else XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static int SetupInterruptSystem()
{

	int Status;

	/*
	 * Initialize the interrupt controller driver so that
	 * it's ready to use, specify the device ID that is generated in
	 * xparameters.h
	 */
	Status = XIntc_Initialize(&InterruptController, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Start the interrupt controller such that interrupts are enabled for
	 * all devices that cause interrupts, specific real mode so that
	 * the SPI can cause interrupts thru the interrupt controller.
	 */
	Status = XIntc_Start(&InterruptController, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the exception table
	 */
	Xil_ExceptionInit();

	/*
	 * Enable non-critical exceptions
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
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

	xil_printf("Test Successful!\n\r");
}


static void LaserTest() {
	InitMotorBoard();

	while (true) {
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

	while (true) {
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
