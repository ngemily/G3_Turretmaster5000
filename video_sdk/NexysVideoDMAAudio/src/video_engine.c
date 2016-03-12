/*
 * video_engine.c
 *
 *  Created on: Mar 11, 2016
 *      Author: Robert
 */

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "video_engine.h"
#include "video_capture/video_capture.h"
#include "display_ctrl/display_ctrl.h"
#include "intc/intc.h"
#include <stdio.h>
#include "xuartlite_l.h"
//#include "xuartps.h"
#include "math.h"
#include <ctype.h>
#include <stdlib.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xparameters.h"

/*
 * XPAR redefines
 */
#define DYNCLK_BASEADDR 	XPAR_AXI_DYNCLK_0_BASEADDR
#define VGA_VDMA_ID 		XPAR_AXIVDMA_0_DEVICE_ID
#define DISP_VTC_ID 		XPAR_VTC_0_DEVICE_ID
#define VID_VTC_ID 			XPAR_VTC_1_DEVICE_ID
#define VID_GPIO_ID 		XPAR_AXI_GPIO_VIDEO_DEVICE_ID
#define VID_VTC_IRPT_ID 	XPAR_INTC_0_VTC_1_VEC_ID
#define VID_GPIO_IRPT_ID 	XPAR_INTC_0_GPIO_0_VEC_ID
#define SCU_TIMER_ID 		XPAR_AXI_TIMER_0_DEVICE_ID
#define UART_BASEADDR 		XPAR_UARTLITE_0_BASEADDR

#define DDR_BASE_ADDR		XPAR_MIG7SERIES_0_BASEADDR
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x10000000)

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/*
 * Display and Video Driver structs
 */
static DisplayCtrl	dispCtrl;
static XAxiVdma 	vdma;
static VideoCapture *videoCaptPtr;

char fRefresh; //flag used to trigger a refresh of the Menu on video detect

/*
 * Framebuffers for video data
 */
//u8 frameBuf[DISPLAY_NUM_FRAMES][DEMO_MAX_FRAME];
u8 *frameBuf = (u8*) MEM_BASE_ADDR;
u8 *pFrames[DISPLAY_NUM_FRAMES]; //array of pointers to the frame buffers

void DemoPrintMenu();
void DemoChangeRes();
void DemoCRMenu();
void DemoPrintTest(u8 *frame, u32 width, u32 height, u32 stride, int pattern);

/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */

XStatus initialize_video(VideoCapture *videoPtr, XIntc *intcPtr)
{
    int Status;
    XAxiVdma_Config *vdmaConfig;
    int i;

    videoCaptPtr = videoPtr;

    /*
     * Initialize an array of pointers to the 3 frame buffers
     */
    for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
    {
        pFrames[i] = frameBuf + (i*DEMO_MAX_FRAME);
    }

    /*
     * Initialize VDMA driver
     */
    vdmaConfig = XAxiVdma_LookupConfig(VGA_VDMA_ID);
    if (!vdmaConfig)
    {
        xil_printf("No video DMA found for ID %d\r\n", VGA_VDMA_ID);
        return XST_FAILURE;
    }
    Status = XAxiVdma_CfgInitialize(&vdma, vdmaConfig, vdmaConfig->BaseAddress);
    if (Status != XST_SUCCESS)
    {
        xil_printf("VDMA Configuration Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }

    /*
     * Initialize the Display controller and start it
     */
    Status = DisplayInitialize(&dispCtrl, &vdma, DISP_VTC_ID, DYNCLK_BASEADDR, pFrames, DEMO_STRIDE);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Display Ctrl initialization failed during demo initialization%d\r\n", Status);
        return XST_FAILURE;
    }
    Status = DisplayStart(&dispCtrl);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Couldn't start display during demo initialization%d\r\n", Status);
        return XST_FAILURE;
    }

    /*
     * Initialize the Video Capture device
     */
    Status = VideoInitialize(videoPtr, intcPtr, &vdma, VID_GPIO_ID, VID_VTC_ID,
            VID_VTC_IRPT_ID, pFrames, DEMO_STRIDE, DEMO_START_ON_DET);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Video Ctrl initialization failed during demo initialization%d\r\n", Status);
        return XST_FAILURE;
    }

    /*
     * Set the Video Detect callback to trigger the menu to reset, displaying the new detected resolution
     */
    VideoSetCallback(videoPtr, VideoISR, &fRefresh);

    DemoPrintTest(dispCtrl.framePtr[dispCtrl.curFrame], dispCtrl.vMode.width,
            dispCtrl.vMode.height, dispCtrl.stride, DEMO_PATTERN_1);

    return XST_SUCCESS;
}

void run_video_demo()
{
    int nextFrame = 0;
    char userInput = 0;
    u32 locked;
    XGpio *GpioPtr = &(videoCaptPtr->gpio);

    /* Flush UART FIFO */
    while (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
    {
        XUartLite_ReadReg(UART_BASEADDR, XUL_RX_FIFO_OFFSET);
    }
    while (userInput != 'q')
    {
        fRefresh = 0;
        DemoPrintMenu();

        /* Wait for data on UART */
        while (XUartLite_IsReceiveEmpty(UART_BASEADDR) && !fRefresh)
        {}

        /* Store the first character in the UART receive FIFO and echo it */
        if (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
        {
            userInput = XUartLite_ReadReg(UART_BASEADDR, XUL_RX_FIFO_OFFSET);
            xil_printf("%c", userInput);
        }
        else  //Refresh triggered by video detect interrupt
        {
            userInput = 'r';
        }

        xil_printf(" ~~ %c ~~ ", userInput);

        switch (userInput)
        {
            case 'i':
                VtcIsr(videoCaptPtr, 0);
                break;
            case '1':
                DemoChangeRes();
                break;
            case '2':
                nextFrame = dispCtrl.curFrame + 1;
                if (nextFrame >= DISPLAY_NUM_FRAMES)
                {
                    nextFrame = 0;
                }
                DisplayChangeFrame(&dispCtrl, nextFrame);
                break;
            case '3':
                DemoPrintTest(pFrames[dispCtrl.curFrame], dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_STRIDE, DEMO_PATTERN_0);
                break;
            case '4':
                DemoPrintTest(pFrames[dispCtrl.curFrame], dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_STRIDE, DEMO_PATTERN_1);
                break;
            case '5':
                if (videoCaptPtr->state == VIDEO_STREAMING)
                    VideoStop(videoCaptPtr);
                else
                    VideoStart(videoCaptPtr);
                break;
            case '6':
                nextFrame = videoCaptPtr->curFrame + 1;
                if (nextFrame >= DISPLAY_NUM_FRAMES)
                {
                    nextFrame = 0;
                }
                VideoChangeFrame(videoCaptPtr, nextFrame);
                break;
            case 'q':
                break;
            case 'r':
                locked = XGpio_DiscreteRead(GpioPtr, 2);
                xil_printf("%d", locked);
                break;
            default :
                xil_printf("\n\rInvalid Selection");
                MB_Sleep(50);
        }
    }

    return;
}

void DemoPrintMenu()
{
    //xil_printf("\x1B[H"); //Set cursor to top left of terminal
    //xil_printf("\x1B[2J"); //Clear terminal
    xil_printf("**************************************************\n\r");
    xil_printf("*                ZYBO Video Demo                 *\n\r");
    xil_printf("**************************************************\n\r");
    xil_printf("*Display Resolution: %28s*\n\r", dispCtrl.vMode.label);
    printf("*Display Pixel Clock Freq. (MHz): %15.3f*\n\r", dispCtrl.pxlFreq);
    xil_printf("*Display Frame Index: %27d*\n\r", dispCtrl.curFrame);
    if (videoCaptPtr->state == VIDEO_DISCONNECTED) xil_printf("*Video Capture Resolution: %22s*\n\r", "!HDMI UNPLUGGED!");
    else xil_printf("*Video Capture Resolution: %17dx%-4d*\n\r", videoCaptPtr->timing.HActiveVideo, videoCaptPtr->timing.VActiveVideo);
    xil_printf("*Video Frame Index: %29d*\n\r", videoCaptPtr->curFrame);
    xil_printf("**************************************************\n\r");
    xil_printf("\n\r");
    xil_printf("1 - Change Display Resolution\n\r");
    xil_printf("2 - Change Display Framebuffer Index\n\r");
    xil_printf("3 - Print Blended Test Pattern to Display Framebuffer\n\r");
    xil_printf("4 - Print Color Bar Test Pattern to Display Framebuffer\n\r");
    xil_printf("5 - Start/Stop Video stream into Video Framebuffer\n\r");
    xil_printf("6 - Change Video Framebuffer Index\n\r");
    xil_printf("7 - Grab Video Frame and invert colors\n\r");
    xil_printf("8 - Grab Video Frame and scale to Display resolution\n\r");
    xil_printf("q - Quit\n\r");
    xil_printf("\n\r");
    xil_printf("\n\r");
    xil_printf("Enter a selection:");
}

void DemoChangeRes()
{
    int fResSet = 0;
    int status;
    char userInput = 0;

    /* Flush UART FIFO */
    while (!XUartLite_IsReceiveEmpty(UART_BASEADDR))
    {
        XUartLite_ReadReg(UART_BASEADDR, XUL_RX_FIFO_OFFSET);
    }

    while (!fResSet)
    {
        DemoCRMenu();

        /* Wait for data on UART */
        while (XUartLite_IsReceiveEmpty(UART_BASEADDR) && !fRefresh)
        {}

        /* Store the first character in the UART recieve FIFO and echo it */

        userInput = XUartLite_ReadReg(UART_BASEADDR, XUL_RX_FIFO_OFFSET);
        xil_printf("%c", userInput);
        status = XST_SUCCESS;
        switch (userInput)
        {
            case '1':
                status = DisplayStop(&dispCtrl);
                DisplaySetMode(&dispCtrl, &VMODE_640x480);
                DisplayStart(&dispCtrl);
                fResSet = 1;
                break;
            case '2':
                status = DisplayStop(&dispCtrl);
                DisplaySetMode(&dispCtrl, &VMODE_800x600);
                DisplayStart(&dispCtrl);
                fResSet = 1;
                break;
            case '3':
                status = DisplayStop(&dispCtrl);
                DisplaySetMode(&dispCtrl, &VMODE_1280x720);
                DisplayStart(&dispCtrl);
                fResSet = 1;
                break;
            case '4':
                status = DisplayStop(&dispCtrl);
                DisplaySetMode(&dispCtrl, &VMODE_1280x1024);
                DisplayStart(&dispCtrl);
                fResSet = 1;
                break;
            case '5':
                status = DisplayStop(&dispCtrl);
                DisplaySetMode(&dispCtrl, &VMODE_1920x1080);
                DisplayStart(&dispCtrl);
                fResSet = 1;
                break;
            case 'q':
                fResSet = 1;
                break;
            default :
                xil_printf("\n\rInvalid Selection");
                MB_Sleep(50);
        }
        if (status == XST_DMA_ERROR)
        {
            xil_printf("\n\rWARNING: AXI VDMA Error detected and cleared\n\r");
        }
    }
}

void DemoCRMenu()
{
    xil_printf("\x1B[H"); //Set cursor to top left of terminal
    xil_printf("\x1B[2J"); //Clear terminal
    xil_printf("**************************************************\n\r");
    xil_printf("*                ZYBO Video Demo                 *\n\r");
    xil_printf("**************************************************\n\r");
    xil_printf("*Current Resolution: %28s*\n\r", dispCtrl.vMode.label);
    printf("*Pixel Clock Freq. (MHz): %23.3f*\n\r", dispCtrl.pxlFreq);
    xil_printf("**************************************************\n\r");
    xil_printf("\n\r");
    xil_printf("1 - %s\n\r", VMODE_640x480.label);
    xil_printf("2 - %s\n\r", VMODE_800x600.label);
    xil_printf("3 - %s\n\r", VMODE_1280x720.label);
    xil_printf("4 - %s\n\r", VMODE_1280x1024.label);
    xil_printf("5 - %s\n\r", VMODE_1920x1080.label);
    xil_printf("q - Quit (don't change resolution)\n\r");
    xil_printf("\n\r");
    xil_printf("Select a new resolution:");
}

void DemoPrintTest(u8 *frame, u32 width, u32 height, u32 stride, int pattern)
{
    u32 xcoi, ycoi;
    u32 iPixelAddr;
    u8 wRed, wBlue, wGreen;
    u32 wCurrentInt;
    double fRed, fBlue, fGreen, fColor;
    u32 xLeft, xMid, xRight, xInt;
    u32 yMid, yInt;
    double xInc, yInc;


    switch (pattern)
    {
        case DEMO_PATTERN_0:

            xInt = width / 4; //Four intervals, each with width/4 pixels
            xLeft = xInt * 3;
            xMid = xInt * 2 * 3;
            xRight = xInt * 3 * 3;
            xInc = 256.0 / ((double) xInt); //256 color intensities are cycled through per interval (overflow must be caught when color=256.0)

            yInt = height / 2; //Two intervals, each with width/2 lines
            yMid = yInt;
            yInc = 256.0 / ((double) yInt); //256 color intensities are cycled through per interval (overflow must be caught when color=256.0)

            fBlue = 0.0;
            fRed = 256.0;
            for(xcoi = 0; xcoi < (width*3); xcoi+=3)
            {
                /*
                 * Convert color intensities to integers < 256, and trim values >=256
                 */
                wRed = (fRed >= 256.0) ? 255 : ((u8) fRed);
                wBlue = (fBlue >= 256.0) ? 255 : ((u8) fBlue);
                iPixelAddr = xcoi;
                fGreen = 0.0;
                for(ycoi = 0; ycoi < height; ycoi++)
                {

                    wGreen = (fGreen >= 256.0) ? 255 : ((u8) fGreen);
                    frame[iPixelAddr] = wRed;
                    frame[iPixelAddr + 1] = wBlue;
                    frame[iPixelAddr + 2] = wGreen;
                    if (ycoi < yMid)
                    {
                        fGreen += yInc;
                    }
                    else
                    {
                        fGreen -= yInc;
                    }

                    /*
                     * This pattern is printed one vertical line at a time, so the address must be incremented
                     * by the stride instead of just 1.
                     */
                    iPixelAddr += stride;
                }

                if (xcoi < xLeft)
                {
                    fBlue = 0.0;
                    fRed -= xInc;
                }
                else if (xcoi < xMid)
                {
                    fBlue += xInc;
                    fRed += xInc;
                }
                else if (xcoi < xRight)
                {
                    fBlue -= xInc;
                    fRed -= xInc;
                }
                else
                {
                    fBlue += xInc;
                    fRed = 0;
                }
            }
            /*
             * Flush the framebuffer memory range to ensure changes are written to the
             * actual memory, and therefore accessible by the VDMA.
             */
            Xil_DCacheFlushRange((unsigned int) frame, DEMO_MAX_FRAME);
            break;
        case DEMO_PATTERN_1:

            xInt = width / 7; //Seven intervals, each with width/7 pixels
            xInc = 256.0 / ((double) xInt); //256 color intensities per interval. Notice that overflow is handled for this pattern.

            fColor = 0.0;
            wCurrentInt = 1;
            for(xcoi = 0; xcoi < (width*3); xcoi+=3)
            {

                /*
                 * Just draw white in the last partial interval (when width is not divisible by 7)
                 */
                if (wCurrentInt > 7)
                {
                    wRed = 255;
                    wBlue = 255;
                    wGreen = 255;
                }
                else
                {
                    if (wCurrentInt & 0b001)
                        wRed = (u8) fColor;
                    else
                        wRed = 0;

                    if (wCurrentInt & 0b010)
                        wBlue = (u8) fColor;
                    else
                        wBlue = 0;

                    if (wCurrentInt & 0b100)
                        wGreen = (u8) fColor;
                    else
                        wGreen = 0;
                }

                iPixelAddr = xcoi;

                for(ycoi = 0; ycoi < height; ycoi++)
                {
                    frame[iPixelAddr] = wRed;
                    frame[iPixelAddr + 1] = wBlue;
                    frame[iPixelAddr + 2] = wGreen;
                    /*
                     * This pattern is printed one vertical line at a time, so the address must be incremented
                     * by the stride instead of just 1.
                     */
                    iPixelAddr += stride;
                }

                fColor += xInc;
                if (fColor >= 256.0)
                {
                    fColor = 0.0;
                    wCurrentInt++;
                }
            }
            /*
             * Flush the framebuffer memory range to ensure changes are written to the
             * actual memory, and therefore accessible by the VDMA.
             */
            Xil_DCacheFlushRange((unsigned int) frame, DEMO_MAX_FRAME);
            break;
        default :
            xil_printf("Error: invalid pattern passed to DemoPrintTest");
    }
}

void VideoISR(void *callBackRef, void *pVideo)
{
    char *data = (char *) callBackRef;
    //print("VideoISR\n\r");
    *data = 1; //set fRefresh to 1
}
