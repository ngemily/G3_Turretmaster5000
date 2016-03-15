/*
 * dma.h
 *
 *  Created on: Jan 20, 2015
 *      Author: ROHegbeC
 */

#ifndef DMA_TARGETING_H_
#define DMA_TARGETING_H_

#include <xparameters.h>
#include <xil_printf.h>
#include <xaxivdma.h>

#include "../display_ctrl/vga_modes.h"


/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

void fnTargS2MMInterruptHandler (void *Callback);
void fnTargMM2SInterruptHandler (void *Callback);
XStatus fnConfigTargetingDma(XAxiVdma *AxiVdma, u8 *baseFramePtr);
XStatus fnSetupTargetingDmaInput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx);
XStatus fnSetupTargetingDmaOutput(XAxiVdma *AxiVdma, const VideoMode *videoMode, int frameIdx);
XStatus fnStartTargetingDmaInOut(XAxiVdma *AxiVdma, int frameIdxIn, int frameIdxOut);

#endif /* DMA_H_ */
