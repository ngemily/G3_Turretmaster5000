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

typedef enum {
    COLOUR_RED,
} colour_t;

XStatus        initialize_targeting(XAxiVdma *targetingDmaPtr);
XStatus        targeting_begin_transfer(XAxiVdma *dmaPtr);
void           print_ip_info(void);
TargetingState get_targeting_state(void);
int            ip_busy(void);
XStatus        draw_dot(int x, int y, colour_t colour);

void SetOutputMode(TargettingControlMode mode);
void 		   SetOutputMode(TargettingControlMode mode);
void           SetThresholdValue(int threshold);

#endif /* TARGETING_ENGINE_H_ */
