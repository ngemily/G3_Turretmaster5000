/*
 * targeting_engine.c
 *
 *  Created on: Mar 13, 2016
 *      Author: Robert
 */

#include <xstatus.h>
#include <xaxidma.h>

#include "targeting/dma.h"

#include "targeting_engine.h"

XStatus initialize_targeting(XAxiDma *targetingDmaPtr) {
    int status = XST_SUCCESS;

    if (XST_SUCCESS == status) {
        status = fnConfigTargetingDma(targetingDmaPtr);
    }

    return status;
}

//XStatus transfer
