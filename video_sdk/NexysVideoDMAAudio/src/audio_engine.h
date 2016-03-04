/*
 * audio_engine.h
 *
 *  Created on: Feb 16, 2016
 *      Author: Roberto Bortolussi
 */

#ifndef AUDIO_ENGINE_H_
#define AUDIO_ENGINE_H_

#include <xstatus.h>

XStatus initialize_audio(XIic *IicPtr, XAxiDma *AxiDmaPtr);
XStatus rec_audio(void);
XStatus play_audio(void);

#endif /* AUDIO_ENGINE_H_ */
