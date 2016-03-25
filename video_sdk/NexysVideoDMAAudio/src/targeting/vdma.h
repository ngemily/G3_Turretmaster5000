/*
 * vdma.h
 *
 *  Created on: Mar 14, 2016
 *      Author: Roberto Bortolussi
 */

#ifndef DMA_TARGETING_H_
#define DMA_TARGETING_H_

#include <xparameters.h>
#include <xil_printf.h>
#include <xaxivdma.h>

#include "../display_ctrl/vga_modes.h"


/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

XStatus fnConfigTargetingDma(XAxiVdma *AxiVdma, u8 *baseFramePtr);
XStatus fnSetupTargetingDmaInput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx);
XStatus fnSetupTargetingDmaOutput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx);
XStatus fnStartTargetingDmaInOut(XAxiVdma *AxiVdma, int frameIdxIn, int frameIdxOut);
int     isVdmaComplete(void);

#endif /* DMA_TARGETING_H_ */
