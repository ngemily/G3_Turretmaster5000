/*
 * targeting_engine.c
 *
 *  Created on: Mar 13, 2016
 *      Author: Robert
 */

#include <xstatus.h>
#include <xaxidma.h>

#include "targeting/vdma.h"
#include "display_ctrl/vga_modes.h"

#include "targeting_engine.h"
#include "demo.h"

static XAxiVdma *vdmaPtr;

XStatus initialize_targeting(XAxiVdma *targetingDmaPtr) {
    int status = XST_SUCCESS;

    vdmaPtr = targetingDmaPtr;

    if (XST_SUCCESS == status) {
        status = fnConfigTargetingDma(targetingDmaPtr, (u8 *) FRAMES_BASE_ADDR);
    }

    if (XST_SUCCESS == status) {
        status = fnSetupTargetingDmaInput(targetingDmaPtr, &VMODE_1280x720, 1);
    }

    if (XST_SUCCESS == status) {
        status = fnSetupTargetingDmaOutput(targetingDmaPtr, &VMODE_1280x720, 2);
    }

    return status;
}

XStatus targeting_begin_transfer(XAxiVdma *dmaPtr) {
    return fnStartTargetingDmaInOut(dmaPtr, 1, 2);
}

void print_ip_info(void) {
    xil_printf("VDMA Info:\n\r");
    xil_printf("XAxiVdma_GetStatus(IN)  = %08x\n\r", XAxiVdma_GetStatus(vdmaPtr, XAXIVDMA_READ));
    xil_printf("XAxiVdma_GetStatus(OUT) = %08x\n\r", XAxiVdma_GetStatus(vdmaPtr, XAXIVDMA_WRITE));
}
