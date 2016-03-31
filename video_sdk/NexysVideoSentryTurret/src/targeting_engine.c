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
#define X_MIDDLE (639)
#define Y_MIDDLE (359)
#define MAX_OBJECT_SIZE (100000)

static XAxiVdma *vdmaPtr;
volatile TargetingIPStatus *targetingIp = (TargetingIPStatus *)IMAGE_PROC_BASE_ADDR;

static int dbgTargetX = X_MIDDLE;
static int dbgTargetY = Y_MIDDLE;

int sMinObjSize = 1000;

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

XStatus targeting_begin_transfer(XAxiVdma *dmaPtr, int outputEnable) {
    targetingIp->input_only = !outputEnable;
    targetingIp->reset = 1;
    targetingIp->reset = 0;
    return fnStartTargetingDmaInOut(dmaPtr, 0, 1, outputEnable);
}

void generate_debug_target(int x, int y) {
    dbgTargetX = x;
    dbgTargetY = y;
}

void draw_debug_dots(void) {
    int i;
    draw_dot(targetingIp->laserLocation.x, targetingIp->laserLocation.y, COLOUR_RED);
    for (i=1; i<targetingIp->num_objs; i++) {
        targetingIp->obj_id = i;
        if (targetingIp->obj_area > sMinObjSize) {
            u32 x = targetingIp->obj_x / targetingIp->obj_area;
            u32 y = targetingIp->obj_y / targetingIp->obj_area;
            draw_dot(x, y, COLOUR_BLUE);
        }
    }
}

TargetingState get_targeting_state(void) {
    TargetingState state;
    //while (!targetingIp->dataValid);
    state.laser.x  = targetingIp->laserLocation.x;
    state.laser.y  = targetingIp->laserLocation.y;

    targetingIp->obj_id = 1;
    u32 max_size = targetingIp->obj_area;
    int target_idx = 1;
    int i;
    for (i=2; i<targetingIp->num_objs; i++) {
        targetingIp->obj_id = i;
        if (targetingIp->obj_area > max_size &&
                targetingIp->obj_area < MAX_OBJECT_SIZE) {
            max_size = targetingIp->obj_area;
            target_idx = i;
        }
    }
    targetingIp->obj_id = target_idx;
    state.target_loc.x = targetingIp->obj_x / targetingIp->obj_area;
    state.target_loc.y = targetingIp->obj_y / targetingIp->obj_area;
    state.target_size = targetingIp->obj_area;
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

    xil_printf("  rx_fsm_state    = %08x\n\r", ipStatus->rx_fsm_state);
    xil_printf("  tx_fsm_state    = %08x\n\r", ipStatus->tx_fsm_state);
    xil_printf("  rx_write_ptr    = %08x\n\r", ipStatus->rx_write_ptr);
    xil_printf("  rx_read_ptr     = %08x\n\r", ipStatus->rx_read_ptr);
    xil_printf("  tx_write_ptr    = %08x\n\r", ipStatus->tx_write_ptr);
    xil_printf("  tx_read_ptr     = %08x\n\r", ipStatus->tx_read_ptr);
    xil_printf("  rx_fifo_tracker = %08x\n\r", ipStatus->rx_fifo_tracker);
    xil_printf("  tx_fifo_tracker = %08x\n\r", ipStatus->tx_fifo_tracker);
    xil_printf("  MM2S_Ready      = %08x\n\r", ipStatus->MM2S_ready);
    xil_printf("  MM2S_Valid      = %08x\n\r", ipStatus->MM2S_valid);
    xil_printf("  S2MM_Valid      = %08x\n\r", ipStatus->S2MM_valid);
    xil_printf("  S2MM_Ready      = %08x\n\r", ipStatus->S2MM_ready);
    xil_printf("  flood1 threshold = %08x\n\r", ipStatus->flood1_threshold);
    xil_printf("  flood2 threshold = %08x\n\r", ipStatus->flood2_threshold);
    xil_printf("  sobel threshold = %08x\n\r", ipStatus->sobel_threshold);
    xil_printf("  red_threshold   = %08x\n\r", ipStatus->red_threshold);
    xil_printf("  mode            = %08x\n\r", ipStatus->mode);
    xil_printf("  reset           = %08x\n\r", ipStatus->reset);
    xil_printf("  laser           = (%d, %d)\n\r", ipStatus->laserLocation.x, ipStatus->laserLocation.y);
    xil_printf("  num_labels      = %08x\n\r", ipStatus->num_objs);
    xil_printf("  obj_id          = %08x\n\r", ipStatus->obj_id);
    xil_printf("  obj_loc         = (%d, %d)\n\r", ipStatus->obj_x / ipStatus->obj_area, ipStatus->obj_y / ipStatus->obj_area);
}


void SetOutputMode(TargettingControlMode mode) {
    targetingIp->mode = mode;
}


// Hard coded to draw on frame 1
XStatus draw_dot(int x, int y, colour_t colour) {
    int stride = VMODE_1280x720.width*3;
    u8 *frame = ((u8 *) FRAMES_BASE_ADDR) + (stride*VMODE_1280x720.height);
    int xo, yo;
    for (yo=-1; yo<=1; yo++) {
        for (xo=-1; xo<=1; xo++) {
            *(frame + (y+yo)*stride + (x+xo)*3)     = colour.green;
            *(frame + (y+yo)*stride + (x+xo)*3 + 1) = colour.blue;
            *(frame + (y+yo)*stride + (x+xo)*3 + 2) = colour.red;
        }
    }
    for (xo=-3; xo<=3; xo++) {
        *(frame + (y)*stride + (x+xo)*3)     = colour.green;;
        *(frame + (y)*stride + (x+xo)*3 + 1) = colour.blue;
        *(frame + (y)*stride + (x+xo)*3 + 2) = colour.red;
    }
    for (yo=-3; yo<=3; yo++) {
        *(frame + (y+yo)*stride + (x)*3)     = colour.green;;
        *(frame + (y+yo)*stride + (x)*3 + 1) = colour.blue;
        *(frame + (y+yo)*stride + (x)*3 + 2) = colour.red;
    }

   return XST_SUCCESS;
}

void SetFlood1ThresholdValue(int threshold) {
    targetingIp->flood1_threshold = threshold;
}
void SetFlood2ThresholdValue(int threshold) {
    targetingIp->flood2_threshold = threshold;
}
void SetSobelThresholdValue(int threshold) {
    targetingIp->sobel_threshold = threshold;
}
void SetRedThresholdValue(int threshold) {
    targetingIp->red_threshold = threshold;
}
void SetObjIdValue(int id) {
    targetingIp->obj_id = id;
}

void setMinObjSize(int size) {
    sMinObjSize = size;
}
