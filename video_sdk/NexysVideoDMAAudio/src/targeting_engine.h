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

XStatus initialize_targeting(XAxiVdma *targetingDmaPtr);
XStatus targeting_begin_transfer(XAxiVdma *dmaPtr);
void    print_ip_info(void);

#endif /* TARGETING_ENGINE_H_ */
