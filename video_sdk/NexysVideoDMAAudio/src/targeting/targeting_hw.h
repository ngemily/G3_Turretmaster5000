/*
 * targeting_hw.h
 *
 *  Created on: Mar 17, 2016
 *      Author: Roberto Bortolussi
 */

#ifndef TARGETING_HW_H_
#define TARGETING_HW_H_

#define MAX_NUM_TARGETING_OBJECTS 4

typedef struct {
	u16 x;
	u16 y;
} TargetLocation;

typedef struct {
	u32 valid;
	TargetLocation location;
	u32 moments[7];
} TargetObjectData;

typedef struct {
    u32 rx_fsm_state;
    u32 tx_fsm_state;
    u32 rx_write_ptr;
    u32 rx_read_ptr;
    u32 tx_write_ptr;
    u32 tx_read_ptr;
    u32 rx_fifo_tracker;
    u32 tx_fifo_tracker;
    u32 MM2S_ready;
    u32 MM2S_valid;
    u32 S2MM_valid;
    u32 S2MM_ready;
    u8 mode;
    u8 threshold;
    u16 reserved;
    u32 reset;
    TargetLocation laserLocation;
    TargetObjectData objects[MAX_NUM_TARGETING_OBJECTS];
} TargetingIPStatus;


typedef enum {
	SOBEL_MODE = 0,
	COLOUR_TRANSFORM,
	PASSTHROUGH
} TargettingControlMode;


#endif /* TARGETING_HW_H_ */
