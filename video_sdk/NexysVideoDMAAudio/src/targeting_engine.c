/*
 * targeting_engine.c
 *
 *  Created on: Mar 13, 2016
 *      Author: Roberto Bortolussi
 */

#include <xstatus.h>
#include <xaxidma.h>

#include "targeting/vdma.h"
#include "targeting/targeting_hw.h"
#include "display_ctrl/vga_modes.h"

#include "targeting_engine.h"
#include "demo.h"

#define IMAGE_PROC_BASE_ADDR (XPAR_IMAGE_PROCESSING_IP_0_S_AXI_LITE_BASEADDR)

static XAxiVdma *vdmaPtr;
volatile TargetingIPStatus *targetingIp = (TargetingIPStatus *)IMAGE_PROC_BASE_ADDR;

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
    targetingIp->reset = 1;
    targetingIp->reset = 0;
    return fnStartTargetingDmaInOut(dmaPtr, 0, 1);
}

TargetingState get_targeting_state(void) {
	TargetingState state;
	//while (!targetingIp->dataValid);
	state.laser.x = targetingIp->laserLocation.x;
	state.laser.y = targetingIp->laserLocation.y;
	state.target.x = 0; // TODO
	state.target.y = 0; // TODO
	return state;
}

int ip_busy(void) {
    return !isVdmaComplete();
}

void print_ip_info(void) {
    xil_printf("VDMA Info:\n\r");
    XAxiVdma_DmaRegisterDump(vdmaPtr, XAXIVDMA_READ);
    XAxiVdma_DmaRegisterDump(vdmaPtr, XAXIVDMA_WRITE);
    volatile TargetingIPStatus *ipStatus = (TargetingIPStatus *) IMAGE_PROC_BASE_ADDR;

    xil_printf("rx_fsm_state = %08x\n\r", ipStatus->rx_fsm_state);
    xil_printf("tx_fsm_state = %08x\n\r", ipStatus->tx_fsm_state);
    xil_printf("rx_write_ptr = %08x\n\r", ipStatus->rx_write_ptr);
    xil_printf("rx_read_ptr  = %08x\n\r", ipStatus->rx_read_ptr);
    xil_printf("tx_write_ptr = %08x\n\r", ipStatus->tx_write_ptr);
    xil_printf("tx_read_ptr  = %08x\n\r", ipStatus->tx_read_ptr);
    xil_printf("rx_fifo_tracker = %08x\n\r", ipStatus->rx_fifo_tracker);
    xil_printf("tx_fifo_tracker  = %08x\n\r", ipStatus->tx_fifo_tracker);
    xil_printf("MM2S_Ready   = %08x\n\r", ipStatus->MM2S_ready);
    xil_printf("MM2S_Valid   = %08x\n\r", ipStatus->MM2S_valid);
    xil_printf("S2MM_Valid   = %08x\n\r", ipStatus->S2MM_valid);
    xil_printf("S2MM_Ready   = %08x\n\r", ipStatus->S2MM_ready);
    xil_printf("reserved     = %08x\n\r", ipStatus->reserved);
    xil_printf("threshold    = %08x\n\r", ipStatus->threshold);
    xil_printf("red_threshold = %08x\n\r", ipStatus->red_threshold);
    xil_printf("mode         = %08x\n\r", ipStatus->mode);
    xil_printf("reset        = %08x\n\r", ipStatus->reset);
    xil_printf("laser        = (%d, %d)\n\r", ipStatus->laserLocation.x, ipStatus->laserLocation.y);
}


void SetOutputMode(TargettingControlMode mode) {
	targetingIp->mode = mode;
}


void SetThresholdValue(int threshold) {
	targetingIp->threshold = threshold;
}
void SetRedThresholdValue(int threshold) {
	targetingIp->red_threshold = threshold;
}
