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
    u32 tx_fsm_state;;
    u32 read_ptr;
    u32 write_ptr;
    u32 MM2S_ready;
    u32 MM2S_valid;
    u32 S2MM_valid;
    u32 S2MM_ready;
    u32 dataValid;
    TargetLocation laserLocation;
    TargetObjectData objects[MAX_NUM_TARGETING_OBJECTS];
} TargetingIPStatus;

#endif /* TARGETING_HW_H_ */
