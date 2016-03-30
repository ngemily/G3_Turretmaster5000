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

typedef struct {
    u8 red;
    u8 green;
    u8 blue;
} colour_t;

static const colour_t COLOUR_RED  = {0xFF, 0x00, 0x00};
static const colour_t COLOUR_BLUE = {0x00, 0x00, 0xFF};

XStatus        initialize_targeting(XAxiVdma *targetingDmaPtr);
XStatus        targeting_begin_transfer(XAxiVdma *dmaPtr, int outputEnable);
void           print_ip_info(void);
TargetingState get_targeting_state(void);
int            ip_busy(void);
XStatus        draw_dot(int x, int y, colour_t colour);

void SetOutputMode(TargettingControlMode mode);
void SetOutputMode(TargettingControlMode mode);
void SetThresholdValue(int threshold);
void SetFloodThresholdValue(int threshold);
void SetSobelThresholdValue(int threshold);
void SetRedThresholdValue(int threshold);
void SetObjIdValue(int id);

#endif /* TARGETING_ENGINE_H_ */
