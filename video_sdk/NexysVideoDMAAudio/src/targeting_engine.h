/*
 * targeting_engine.h
 *
 *  Created on: Mar 13, 2016
 *      Author: Robert
 */

#ifndef TARGETING_ENGINE_H_
#define TARGETING_ENGINE_H_

#include <xstatus.h>
#include <xaxivdma.h>
#include "targeting/targeting_hw.h"

typedef struct {
	u16 x;
	u16 y;
} location_t;

typedef struct {
	location_t laser;
	location_t target;
} TargetingState;

XStatus        initialize_targeting(XAxiVdma *targetingDmaPtr);
XStatus        targeting_begin_transfer(XAxiVdma *dmaPtr);
void           print_ip_info(void);
TargetingState get_targeting_state(void);

void SetOutputMode(TargettingControlMode mode);

#endif /* TARGETING_ENGINE_H_ */
