/*
 * targeting_engine.c
 *
 *  Created on: Mar 13, 2016
 *      Author: Robert
 */

#include <xstatus.h>
#include <xaxidma.h>

#include "targeting/vdma.h"
#include "targeting/targeting_hw.h"
#include "display_ctrl/vga_modes.h"

#include "targeting_engine.h"
#include "demo.h"

static XAxiVdma *vdmaPtr;

XStatus initialize_targeting(XAxiVdma *targetingDmaPtr) {
    int status = XST_SUCCESS;

    XAxiVdma_Reset(targetingDmaPtr, XAXIVDMA_READ);
    XAxiVdma_Reset(targetingDmaPtr, XAXIVDMA_WRITE);
    while (XAxiVdma_ResetNotDone(targetingDmaPtr, XAXIVDMA_READ));
    while (XAxiVdma_ResetNotDone(targetingDmaPtr, XAXIVDMA_WRITE));

    vdmaPtr = targetingDmaPtr;

    print_ip_info();

    if (XST_SUCCESS == status) {
        status = fnConfigTargetingDma(targetingDmaPtr, (u8 *) FRAMES_BASE_ADDR);
    }

    if (XST_SUCCESS == status) {
        status = fnSetupTargetingDmaInput(targetingDmaPtr, &VMODE_1280x720, 0);
    }

    if (XST_SUCCESS == status) {
        status = fnSetupTargetingDmaOutput(targetingDmaPtr, &VMODE_1280x720, 1);
    }

    return status;
}

XStatus targeting_begin_transfer(XAxiVdma *dmaPtr) {
    return fnStartTargetingDmaInOut(dmaPtr, 0, 1);
}

void print_ip_info(void) {
    xil_printf("VDMA Info:\n\r");
    XAxiVdma_DmaRegisterDump(vdmaPtr, XAXIVDMA_READ);
    XAxiVdma_DmaRegisterDump(vdmaPtr, XAXIVDMA_WRITE);
    volatile TargetingIPStatus *ipStatus = (TargetingIPStatus *) 0x44A70000;
    xil_printf("rx_fsm_state = %08x\n\r", ipStatus->rx_fsm_state);
    xil_printf("tx_fsm_state = %08x\n\r", ipStatus->tx_fsm_state);
    xil_printf("Read_Ptr     = %08x\n\r", ipStatus->read_ptr);
    xil_printf("Write_Ptr    = %08x\n\r", ipStatus->write_ptr);
    xil_printf("MM2S_Ready   = %08x\n\r", ipStatus->MM2S_ready);
    xil_printf("MM2S_Valid   = %08x\n\r", ipStatus->MM2S_valid);
    xil_printf("S2MM_Valid   = %08x\n\r", ipStatus->S2MM_valid);
    xil_printf("S2MM_Ready   = %08x\n\r", ipStatus->S2MM_ready);
}
