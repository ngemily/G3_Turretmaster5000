/*
 * dma.h
 *
 *  Created on: Jan 20, 2015
 *      Author: ROHegbeC
 */

#ifndef DMA_TARGETING_H_
#define DMA_TARGETING_H_

#include "xparameters.h"
#include "xil_printf.h"
#include "xaxidma.h"

/************************** Variable Definitions *****************************/


/************************** Function Definitions *****************************/

void fnTargS2MMInterruptHandler (void *Callback);
void fnTargMM2SInterruptHandler (void *Callback);
XStatus fnConfigTargetingDma(XAxiDma *AxiDma);

#endif /* DMA_H_ */
