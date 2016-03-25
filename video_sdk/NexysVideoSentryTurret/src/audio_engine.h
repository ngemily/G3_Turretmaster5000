/*
 * audio_engine.h
 *
 *  Created on: Feb 16, 2016
 *      Author: Roberto Bortolussi
 */

#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <xstatus.h>
#include <stdbool.h>

typedef struct AudioClip {
    u32 *baseAddr;
    u32 length;
    bool loaded;
} AudioClip;

XStatus initialize_audio(XIic *IicPtr, XAxiDma *AxiDmaPtr);
XStatus rec_audio(void);
XStatus play_audio(u32 *baseAddr, int numBytes);

#endif /* AUDIO_ENGINE_H_ */
