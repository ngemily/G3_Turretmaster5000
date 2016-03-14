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
#include "microblaze_sleep.h"
#include "demo.h"

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

XStatus video_set_input_frame(u32 idx) {
    if (idx >= DISPLAY_NUM_FRAMES) {
        return XST_FAILURE;
    }

    return VideoChangeFrame(videoCaptPtr, idx);
}

XStatus video_set_output_frame(u32 idx) {
    if (idx >= DISPLAY_NUM_FRAMES) {
        return XST_FAILURE;
    }

    return DisplayChangeFrame(&dispCtrl, idx);
}

XStatus video_set_output_resolution(enum OUTPUT_RESOULTION res) {
    const VideoMode *mode = NULL;
    switch (res) {
    case RES_1080P:
        mode = &VMODE_1920x1080;
        break;
    case RES_720P:
        mode = &VMODE_1280x720;
        break;
    case RES_480P:
        mode = &VMODE_640x480;
        break;
    case RES_SVGA:
        mode = &VMODE_800x600;
        break;
    default:
        break;
    }

    if (NULL == mode) {
        return XST_FAILURE;
    }

    XStatus status = DisplayStop(&dispCtrl);
    status |= DisplaySetMode(&dispCtrl, &VMODE_640x480);
    status |= DisplayStart(&dispCtrl);
    return status;
}

XStatus video_set_input_enabled(int enable) {
    if (!enable && VIDEO_STREAMING == videoCaptPtr->state) {
        VideoStop(videoCaptPtr);
    } else if (enable && VIDEO_PAUSED == videoCaptPtr->state) {
        VideoStart(videoCaptPtr);
    } else if (VIDEO_DISCONNECTED == videoCaptPtr->state) {
        print("Video Diconnected, can't enable!!\n\r");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

void print_video_info(void)
{
    xil_printf("Display Resolution: %28s*\n\r", dispCtrl.vMode.label);
    xil_printf("Display Pixel Clock Freq. (MHz): %15.3f*\n\r", dispCtrl.pxlFreq);
    xil_printf("Display Frame Index: %27d*\n\r", dispCtrl.curFrame);
    if (videoCaptPtr->state == VIDEO_DISCONNECTED)
        xil_printf("Video Capture Resolution: %22s*\n\r", "!HDMI UNPLUGGED!");
    else
        xil_printf("Video Capture Resolution: %17dx%-4d*\n\r",
                videoCaptPtr->timing.HActiveVideo, videoCaptPtr->timing.VActiveVideo);
    xil_printf("Video Frame Index: %29d*\n\r", videoCaptPtr->curFrame);
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
