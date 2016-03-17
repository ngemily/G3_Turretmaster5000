/*
 * targeting_hw.h
 *
 *  Created on: Mar 17, 2016
 *      Author: Roberto
 */

#ifndef TARGETING_HW_H_
#define TARGETING_HW_H_

typedef struct {
    u32 rx_fsm_state;
    u32 tx_fsm_state;;
    u32 read_ptr;
    u32 write_ptr;
    u32 MM2S_ready;
    u32 MM2S_valid;
    u32 S2MM_valid;
    u32 S2MM_ready;
} TargetingIPStatus;

#endif /* TARGETING_HW_H_ */
