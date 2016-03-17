/*
 * dma.c
 *
 *  Created on: Jan 20, 2015
 *      Author: ROHegbeC
 */

#include <xaxivdma.h>

#include "vdma.h"
#include "../display_ctrl/vga_modes.h"

#define IP_NUM_FRAMES 2

/************************** Variable Definitions *****************************/

volatile int targeting_dma_error = 0;

static XAxiVdma_DmaSetup targetingInputSetup;
static XAxiVdma_DmaSetup targetingOutputSetup;
static u8 *baseFrameBufferAddress;

/******************************************************************************
 *
 *****************************************************************************/
static void vdmaInputComplete(void *CallBackRef, u32 InterruptTypes)
{
    xil_printf("vdmaInputComplete\n\r");
}
static void vdmaInputError(void *CallBackRef, u32 InterruptTypes)
{
    xil_printf("vdmaInputError\n\r");
}
static void vdmaOutputComplete(void *CallBackRef, u32 InterruptTypes)
{
    xil_printf("vdmaOutputComplete\n\r");
}
static void vdmaOutputError(void *CallBackRef, u32 InterruptTypes)
{
    xil_printf("vdmaOutputError!\n\r");
}

/******************************************************************************
 * Function to configure the DMA in Interrupt mode, this implies that the scatter
 * gather function is disabled. Prior to calling this function, the user must
 * make sure that the Interrupts and the Interrupt Handlers have been configured
 *
 * @return    XST_SUCCESS - if configuration was successful
 *             XST_FAILURE - when the specification are not met
 *****************************************************************************/
XStatus fnConfigTargetingDma(XAxiVdma *AxiVdma, u8 *baseFramePtr)
{
    int Status;
    XAxiVdma_Config *vdmaConfig;

    baseFrameBufferAddress = baseFramePtr;

    /*
     * Initialize VDMA driver
     */
    vdmaConfig = XAxiVdma_LookupConfig(XPAR_AXIVDMA_1_DEVICE_ID);
    if (!vdmaConfig)
    {
        xil_printf("No video DMA found for ID %d\r\n", XPAR_AXIVDMA_1_DEVICE_ID);
        return XST_FAILURE;
    }
    Status = XAxiVdma_CfgInitialize(AxiVdma, vdmaConfig, vdmaConfig->BaseAddress);
    if (Status != XST_SUCCESS)
    {
        xil_printf("VDMA Configuration Initialization failed %d\r\n", Status);
        return XST_FAILURE;
    }

    targetingInputSetup.FrameDelay         = 0;
    targetingInputSetup.EnableCircularBuf  = 1;
    targetingInputSetup.EnableSync         = 0;
    targetingInputSetup.PointNum           = 0;
    targetingInputSetup.EnableFrameCounter = 1;

    targetingOutputSetup.FrameDelay         = 0;
    targetingOutputSetup.EnableCircularBuf  = 1;
    targetingOutputSetup.EnableSync         = 0;
    targetingOutputSetup.PointNum           = 0;
    targetingOutputSetup.EnableFrameCounter = 1;

    XAxiVdma_SetCallBack(AxiVdma, XAXIVDMA_HANDLER_GENERAL,
            vdmaInputComplete,  (void *)AxiVdma, XAXIVDMA_READ);

    XAxiVdma_SetCallBack(AxiVdma, XAXIVDMA_HANDLER_ERROR,
            vdmaInputError,     (void *)AxiVdma, XAXIVDMA_READ);

    XAxiVdma_SetCallBack(AxiVdma, XAXIVDMA_HANDLER_GENERAL,
            vdmaOutputComplete, (void *)AxiVdma, XAXIVDMA_WRITE);

    XAxiVdma_SetCallBack(AxiVdma, XAXIVDMA_HANDLER_ERROR,
            vdmaOutputError,    (void *)AxiVdma, XAXIVDMA_WRITE);

    return XST_SUCCESS;
}

XStatus fnSetupTargetingDmaOutput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx)
{
    int Status;
    int i;

    u32 stride = videoMode->width * 3;

    /*
     * Configure the VDMA to access a frame with the same dimensions as the
     * current mode
     */
    targetingOutputSetup.VertSizeInput       = videoMode->height;
    targetingOutputSetup.HoriSizeInput       = videoMode->width * 3;
    targetingOutputSetup.FixedFrameStoreAddr = frameIdx;
    xil_printf("Out Hor: %d; Ver: %d\n\r", targetingOutputSetup.HoriSizeInput, targetingOutputSetup.VertSizeInput);

    /*
     *Also reset the stride and address values, in case the user manually changed them
     */
    targetingOutputSetup.Stride = stride;
    for (i = 0; i < IP_NUM_FRAMES; i++)
    {
        targetingOutputSetup.FrameStoreStartAddr[i] = (u32) baseFrameBufferAddress + (videoMode->height * stride * i);
        xil_printf("ImagIp BUFF %d: %08x\n\r", i, targetingOutputSetup.FrameStoreStartAddr[i]);
    }

    xil_printf("Starting VDMA for Video capture\n\r");
    Status = XAxiVdma_DmaConfig(AxiVdma, XAXIVDMA_WRITE, &(targetingOutputSetup));
    if (Status != XST_SUCCESS)
    {
        xil_printf("Write channel config failed %d\r\n", Status);
        return XST_FAILURE;
    }
    Status = XAxiVdma_DmaSetBufferAddr(AxiVdma, XAXIVDMA_WRITE, targetingOutputSetup.FrameStoreStartAddr);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Write channel set buffer address failed %d\r\n", Status);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

XStatus fnSetupTargetingDmaInput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx)
{
    int Status;
    u32 stride = videoMode->width * 3;

    /*
     * Configure the VDMA to access a frame with the same dimensions as the
     * current mode
     */
    targetingInputSetup.VertSizeInput       = videoMode->height;
    targetingInputSetup.HoriSizeInput       = videoMode->width * 3;
    targetingInputSetup.FixedFrameStoreAddr = frameIdx;
    xil_printf("In  Hor: %d; Ver: %d\n\r", targetingInputSetup.HoriSizeInput, targetingInputSetup.VertSizeInput);

    /*
     *Also reset the stride and address values, in case the user manually changed them
     */
    targetingInputSetup.Stride = stride;
    int i;
    for (i = 0; i < IP_NUM_FRAMES; i++)
    {
        targetingInputSetup.FrameStoreStartAddr[i] = (u32) (u32) baseFrameBufferAddress + (videoMode->height * stride * i);
    }

    Status = XAxiVdma_DmaConfig(AxiVdma, XAXIVDMA_READ, &(targetingInputSetup));
    if (Status != XST_SUCCESS)
    {
        xil_printf("Read channel config failed %d\r\n", Status);
        return XST_FAILURE;
    }
    Status = XAxiVdma_DmaSetBufferAddr(AxiVdma, XAXIVDMA_READ, targetingInputSetup.FrameStoreStartAddr);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Read channel set buffer address failed %d\r\n", Status);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

XStatus fnStartTargetingDmaInOut(XAxiVdma *AxiVdma, int frameIdxIn, int frameIdxOut)
{
    int Status;

    static XAxiVdma_FrameCounter frameCounterCfg;
    frameCounterCfg.ReadDelayTimerCount  = 0;
    frameCounterCfg.WriteDelayTimerCount = 0;
    frameCounterCfg.ReadFrameCount       = 1;
    frameCounterCfg.WriteFrameCount      = 1;
    XAxiVdma_SetFrameCounter(AxiVdma, &frameCounterCfg);

    XAxiVdma_IntrEnable(AxiVdma,
        		XAXIVDMA_IXR_ERROR_MASK|XAXIVDMA_IXR_FRMCNT_MASK, XAXIVDMA_READ);
    XAxiVdma_IntrEnable(AxiVdma,
        		XAXIVDMA_IXR_ERROR_MASK|XAXIVDMA_IXR_FRMCNT_MASK, XAXIVDMA_WRITE);

    // Start the write channel (IP -> Mem)
    Status = XAxiVdma_DmaStart(AxiVdma, XAXIVDMA_WRITE);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Start Write transfer failed %d\r\n", Status);
        return XST_FAILURE;
    }
    Status = XAxiVdma_StartParking(AxiVdma, frameIdxOut, XAXIVDMA_WRITE);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Unable to park the Write channel %d\r\n", Status);
        return XST_FAILURE;
    }

    // Start the read channel (Mem -> IP)
    Status = XAxiVdma_DmaStart(AxiVdma, XAXIVDMA_READ);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Start read transfer failed %d\r\n", Status);
        return XST_FAILURE;
    }
    Status = XAxiVdma_StartParking(AxiVdma, frameIdxIn, XAXIVDMA_READ);
    if (Status != XST_SUCCESS)
    {
        xil_printf("Unable to park the channel %d\r\n", Status);
        return XST_FAILURE;
    }

    xil_printf("VDMA run success??\n\r");

    return XST_SUCCESS;
}
