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
#include "targeting/vdma.h"
#include "intc/intc.h"
#include "iic/iic.h"
#include "diskio/diskio.h"
#include "diskio/ff.h"
#include "motorcontrol/motorcontrol.h"
#include "video_capture/video_capture.h"
#include "display_ctrl/display_ctrl.h"

#include "platform.h"
#include "audio_engine.h"
#include "uart_engine.h"
#include "video_engine.h"
#include "targeting_engine.h"

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
#include "xintc.h"


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
#define SD_GPIO_CHANNEL	    2

#define BUTTON_CENTER (0x1)
#define BUTTON_UP (0x2)
#define BUTTON_LEFT (0x4)
#define BUTTON_RIGHT (0x8)
#define BUTTON_DOWN (0x10)



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

void ButtonIsr(void *InstancePtr);

static void ManualMainLoop(void);
static void AutoMainLoop(void);
static void EnterManualMainLoop(void);
static void EnterAutomaticMainLoop(void);
static void EnterLaserTest(void);
static void EnterMotorTest(void);
static void EnterIpTest(void);
static void SendImageToIP(void);
static void TestArgs(void);

/************************** Variable Definitions *****************************/
/*
 * Device instance definitions
 */

static XIic      	sIic;
static XAxiDma   	sAxiAudioDma;
static XAxiVdma   	sAxiTargetingDma;
static XIntc     	sIntc;
static XUartLite 	sUartLite;
static XGpio     	sGpio;
static VideoCapture	sVideoCapt;

// static variables storing information about what sounds are available, and
// where they are stored on the SD card.

typedef struct SdFile {
    u32 *baseAddr;
    u32 length;
    bool loaded;
} SdFile;


static SdFile sSdFileBoard[FILE_ID_MAX] = {};
static u32 *sSdFileMemTip;

volatile static enum {
    DEFAULT_LOOP = 0,
                 MANUAL_MODE,
                 AUTOMATIC_MODE,
                 LASER_TEST,
                 MOTOR_TEST,
                 IP_TEST,
} sLoopSelect;


volatile static u32 sButtonState;

// Interrupt vector table
const ivt_t ivt[] = {
    //IIC
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_IIC_0_IIC2INTC_IRPT_INTR,     (XInterruptHandler)XIic_InterruptHandler, &sIic},
    //DMA Stream to MemoryMap Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_DMA_0_S2MM_INTROUT_INTR,      (XInterruptHandler)fnAudioS2MMInterruptHandler, &sAxiAudioDma},
    //DMA MemoryMap to Stream Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_DMA_0_MM2S_INTROUT_INTR,      (XInterruptHandler)fnAudioMM2SInterruptHandler, &sAxiAudioDma},
    //DMA Stream to MemoryMap Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_VDMA_1_S2MM_INTROUT_INTR,     (XInterruptHandler)XAxiVdma_WriteIntrHandler, &sAxiTargetingDma},
    //DMA MemoryMap to Stream Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_VDMA_1_MM2S_INTROUT_INTR,     (XInterruptHandler)XAxiVdma_ReadIntrHandler, &sAxiTargetingDma},
    //UART Lite Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_UARTLITE_0_INTERRUPT_INTR,    (XInterruptHandler)XUartLite_InterruptHandler, &sUartLite},
    //Video Capture Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_V_TC_1_IRQ_INTR,                  (XInterruptHandler)XVtc_IntrHandler, &(sVideoCapt.vtc)},
    //Video GPIO Interrupt handler
    {XPAR_MICROBLAZE_0_AXI_INTC_AXI_GPIO_VIDEO_IP2INTC_IRPT_INTR, (XInterruptHandler)GpioIsr, &sVideoCapt},

    {XPAR_INTC_0_GPIO_0_VEC_ID, (XInterruptHandler)ButtonIsr, &sGpio},
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
    char *ptr = (char *) FRAMES_BASE_ADDR;
    for (i=0; i<100000; i++) {
        XUartLite_SendByte(sUartLite.RegBaseAddress, *(ptr+i));
    }
}


void uart_rec_audio(void) {
    rec_audio();
}


void uart_play_audio(void) {
    play_audio( (u32 *) AUDIO_BASE_ADDR, RECORD_LENGTH);
}


void loadFileIntoMemory(FileIndex fileIndex, char *name) {
    FATFS FatFs;
    FIL FHandle;
    UINT numBytesRead;
    u32 *buff = sSdFileMemTip;
    FRESULT result;

    if (f_mount(&FatFs, "", 0) != FR_OK) {
        xil_printf("Failed to mount filesystem.\n\r");
        return;
    }

    if ((result = f_open(&FHandle, name, FA_READ)) != FR_OK) {
        xil_printf("Failed to f_open %s: %d .\n\r", name, result);
        return;
    }

    if ((result = f_read(&FHandle, (void *) buff, MAX_SONG_LENGTH, &numBytesRead)) != FR_OK) {
        xil_printf("Failed to f_read %s: %d.\n\r", name, result);
        return;
    }

    sSdFileBoard[fileIndex].baseAddr = (u32 *) sSdFileMemTip;
    sSdFileBoard[fileIndex].length = numBytesRead;
    sSdFileBoard[fileIndex].loaded = true;

    // Update the tip of the sound memory region.
    sSdFileMemTip += (numBytesRead >> 2);
}


void loadSounds(void) {
    loadFileIntoMemory(FILE_ID_MACHINE_GUN, GUN_PATH);
    loadFileIntoMemory(FILE_ID_PORTAL_GUN, PORTAL_GUN_PATH);
    //loadSongIntoMemory(FILE_ID_TARGET_ACQUIRED, TARGET_PATH);
    //loadSongIntoMemory(SOUND_ID_STILL_ALIVE, SONG_PATH);
}


void loadImages(void) {
    loadFileIntoMemory(FILE_ID_LEMONGRAB, LEMONGRAB_FILE_PATH);
    loadFileIntoMemory(FILE_ID_HEMAN, HEMAN_FILE_PATH);
}


void playPortalSong(void) {
    if (sSdFileBoard[FILE_ID_STILL_ALIVE].loaded) {
        play_audio(sSdFileBoard[FILE_ID_STILL_ALIVE].baseAddr,
                sSdFileBoard[FILE_ID_STILL_ALIVE].length);
    } else {
        xil_printf("Portal song not loaded yet!\r\n");
    }
}


void playGunSound(void) {
    if (sSdFileBoard[FILE_ID_MACHINE_GUN].loaded) {
        play_audio(sSdFileBoard[FILE_ID_MACHINE_GUN].baseAddr,
                sSdFileBoard[FILE_ID_MACHINE_GUN].length);
    } else {
        xil_printf("Gun sound not loaded yet!\r\n");
    }
}


void playPortalGunSound(void) {
    if (sSdFileBoard[FILE_ID_PORTAL_GUN].loaded) {
        play_audio(sSdFileBoard[FILE_ID_PORTAL_GUN].baseAddr,
                sSdFileBoard[FILE_ID_PORTAL_GUN].length);
    } else {
        xil_printf("Portal gun sound not loaded yet!\r\n");
    }
}


void playTargetAcquired(void) {
    if (sSdFileBoard[FILE_ID_TARGET_ACQUIRED].loaded) {
        play_audio(sSdFileBoard[FILE_ID_TARGET_ACQUIRED].baseAddr,
                sSdFileBoard[FILE_ID_TARGET_ACQUIRED].length);
    } else {
        xil_printf("Target Acquired not loaded yet!\r\n");
    }
}



static void passthroughHdmi(void) {
    if (XST_SUCCESS != video_set_output_resolution(RES_720P)) {
        xil_printf("Error Setting resolution!");
    }
    video_set_input_frame(2);
    video_set_output_frame(2);
}


static void df0(void) {
    video_set_output_frame(0);
}


static void df1(void) {
    video_set_output_frame(1);
}


static void df2(void) {
    video_set_output_frame(2);
}


static void vf0(void) {
    video_set_input_frame(0);
}


static void vf1(void) {
    video_set_input_frame(1);
}


static void vf2(void) {
    video_set_input_frame(2);
}


static void r720p(void) {
    video_set_output_resolution(RES_720P);
}


static void runImageProcessing(void) {
    video_set_input_enabled(0);
    targeting_begin_transfer(&sAxiTargetingDma);
    while(ip_busy()) MB_Sleep(200);
    video_set_input_enabled(1);
}


static void SetPassthroughMode(void) {
    SetOutputMode(0);
}
static void SetGrayscaleMode(void) {
    SetOutputMode(1);
}
static void SetSobelMode(void) {
    SetOutputMode(2);
}
static void SetThresholdMode(void) {
    SetOutputMode(3);
}
static void SetLabelMode(void) {
    SetOutputMode(4);
}
static void SetColourMode(void) {
    SetOutputMode(5);
}

static void SetLowThreshold(void) {
    SetThresholdValue(1);
}
static void SetMediumThreshold(void) {
    SetThresholdValue(50);
}
static void SetHighThreshold(void) {
    SetThresholdValue(100);
}

static void SetRedLowThreshold(void) {
    SetRedThresholdValue(1);
}
static void SetRedMediumThreshold(void) {
    SetRedThresholdValue(50);
}
static void SetRedHighThreshold(void) {
    SetRedThresholdValue(100);
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
    status |= initialize_audio(&sIic, &sAxiAudioDma);
    status |= fnInitInterruptController(&sIntc);
    status |= initialize_uart(&sUartLite);
    status |= initialize_video(&sVideoCapt, &sIntc);
    status |= initialize_targeting(&sAxiTargetingDma);
    status |= SetupSdGpio(&sGpio);

    if (status != XST_SUCCESS) {
        xil_printf("Failed to initialize system.\r\n");
        return XST_FAILURE;
    }

    // Initialize static variables.
    for (int i = 0; i < FILE_ID_MAX; i++) {
        sSdFileBoard[i].loaded = false;
    }
    sSdFileMemTip = (u32 *) AUDIO_BASE_ADDR;

    sButtonState = 0;

    sLoopSelect = DEFAULT_LOOP;
    fnEnableInterrupts(&sIntc, &ivt[0], sizeof(ivt)/sizeof(ivt[0]));

    register_uart_response("test", test_fcn);
    register_uart_response("record", uart_rec_audio);
    register_uart_response("play", uart_play_audio);
    register_uart_response("kill", end_fcn);
    register_uart_response("exit", end_fcn);
    register_uart_response("dump", dump_mem);

    // Commands to run self-tests
    register_uart_response("lowlevel", LowLevelTest);
    register_uart_response("highlevel", HighLevelTest);
    register_uart_response("lasertest", EnterLaserTest);
    register_uart_response("motortest", EnterMotorTest);
    register_uart_response("stop", stopTest);

    register_uart_response("load_sounds", loadSounds);
    register_uart_response("load_images", loadImages);

    register_uart_response("still_alive", playPortalSong);
    register_uart_response("gun", playGunSound);
    register_uart_response("portal_gun", playPortalGunSound);
    register_uart_response("target", playTargetAcquired);

    register_uart_response("manual", EnterManualMainLoop);
    register_uart_response("auto", EnterAutomaticMainLoop);

    register_uart_response("passthrough", passthroughHdmi);
    register_uart_response("runip",       EnterIpTest);
    register_uart_response("videoinfo",   print_video_info);
    register_uart_response("df1",         df1);
    register_uart_response("df2",         df2);
    register_uart_response("df0",         df0);
    register_uart_response("vf1",         vf1);
    register_uart_response("vf2",         vf2);
    register_uart_response("vf0",         vf0);
    register_uart_response("ipinfo",      print_ip_info);
    register_uart_response("720p",        r720p);

    register_uart_response("lemon",       SendImageToIP);
    register_uart_response("pass",        SetPassthroughMode);
    register_uart_response("gray",        SetGrayscaleMode);
    register_uart_response("sobel",        SetSobelMode);
    register_uart_response("thresh",        SetThresholdMode);
    register_uart_response("label",        SetLabelMode);
    register_uart_response("colour",        SetColourMode);

    register_uart_response("redlow",        SetRedLowThreshold);
    register_uart_response("redmed",        SetRedMediumThreshold);
    register_uart_response("redhigh",        SetRedHighThreshold);

    register_uart_response("low",        SetLowThreshold);
    register_uart_response("med",        SetMediumThreshold);
    register_uart_response("high",        SetHighThreshold);

    register_uart_response("test_args",   TestArgs);

    xil_printf("\r\n--- Done registering UART commands --- \r\n");

    xil_printf(PROMPT_STRING);
    while (do_run) {
        switch (sLoopSelect) {
            case MANUAL_MODE: ManualMainLoop(); break;
            case AUTOMATIC_MODE: AutoMainLoop(); break;
            case LASER_TEST: LaserTest(); break;
            case MOTOR_TEST: MotorPatternTest(); break;
            case IP_TEST: runImageProcessing(); sLoopSelect = DEFAULT_LOOP; break;
            default: MB_Sleep(100); break;
        }
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
    XGpio_SetDataDirection(sGpio, 1, ~0);

    XGpio_InterruptEnable(sGpio, XGPIO_IR_CH1_MASK);
    XGpio_InterruptGlobalEnable(sGpio);

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

    if (disk_read(0, TestBuffer, 0, 3) != RES_OK) {
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

    /*
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
       */

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

    TurnOffLaser();
    DisablePanServo();
    DisableTiltServo();
}


void ButtonIsr(void *InstancePtr) {
    XGpio *GpioPtr = (XGpio *) InstancePtr;

    XGpio_InterruptClear(GpioPtr, XGPIO_IR_CH1_MASK);
    //xil_printf("in button ISR.\r\n");

    sButtonState = XGpio_DiscreteRead(GpioPtr, 1) & 0x1F;

    if (continueTest && sLoopSelect == MANUAL_MODE) {
        if (sButtonState & BUTTON_CENTER) {
            playGunSound();
        }
    }
}


static void stopTest() {
    continueTest = false;
    sLoopSelect = DEFAULT_LOOP;
}


static void AutoMainLoop(void) {
    continueTest = true;
    xil_printf("Entering auto mode.\r\n");
    while(continueTest) {
        //      TargetingState state = get_targeting_state();
        runImageProcessing();
        MB_Sleep(3000);
    }
}

void ManualMainLoop(void) {
    // Set up the turret control IP.
    InitMotorBoard();
    TurnOnLaser();
    EnablePanServo();
    EnableTiltServo();
    SetTiltAngle(0);
    SetPanAngle(0);

    continueTest = true;
    xil_printf("Entering manual mode.\r\n");
    while(continueTest) {
        // We allow users to press more than one button at a time.
        if (sButtonState & BUTTON_LEFT) {
            PanLeft(1);
        }

        if (sButtonState & BUTTON_RIGHT) {
            PanRight(1);
        }

        if (sButtonState & BUTTON_UP) {
            TiltUp(1);
        }

        if (sButtonState & BUTTON_DOWN) {
            TiltDown(1);
        }

        MB_Sleep(7);
    }

    SetTiltAngle(0);
    SetPanAngle(0);
    MB_Sleep(100);
    DisableTiltServo();
    DisablePanServo();
    TurnOffLaser();
}


void EnterManualMainLoop(void) {
    sLoopSelect = MANUAL_MODE;
}


static void EnterAutomaticMainLoop(void) {
    sLoopSelect = AUTOMATIC_MODE;
}


static void EnterLaserTest(void) {
    sLoopSelect = LASER_TEST;
}


static void EnterMotorTest(void) {
    sLoopSelect = MOTOR_TEST;
}

static void EnterIpTest(void) {
    sLoopSelect = IP_TEST;
}


static void SendImageToIP(void) {
    u32 *buff = (u32 *) FRAMES_BASE_ADDR;
    SdFile *file = &sSdFileBoard[FILE_ID_LEMONGRAB];

    if (file->loaded) {
        video_set_input_enabled(0);
        memcpy(buff, file->baseAddr, file->length);

        targeting_begin_transfer(&sAxiTargetingDma);
        MB_Sleep(3000);
        video_set_input_enabled(1);
    }
}

static void TestArgs(void) {
    char buf[50];
    int result;
    consume_uart_arg("derp", buf, 50);

    xil_printf("You typed out: %s\r\n", buf);

    consume_uart_arg("num", buf, 50);
    if (get_int_from_string(buf, &result)) {
        xil_printf("Got number: %d\r\n", result);
    } else {
        xil_printf("Not a valid integer: %s\r\n", buf);
    }
}
