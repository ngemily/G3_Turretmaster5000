/*
 * dma.h
 *
 *  Created on: Jan 20, 2015
 *      Author: ROHegbeC
 */

#ifndef DMA_AUDIO_H_
#define DMA_AUDIO_H_

#include "xparameters.h"
#include "xil_printf.h"
#include "xaxidma.h"

/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

void fnAudioS2MMInterruptHandler (void *Callback);
void fnAudioMM2SInterruptHandler (void *Callback);
XStatus fnConfigAudioDma(XAxiDma *AxiDma);

#endif /* DMA_H_ */
