/*
 * audio_engine.c
 *
 *  Created on: Feb 16, 2016
 *      Author: Roberto Bortolussi
 */

#include <mb_interface.h>
#include <xparameters.h>
#include <xiic.h>
#include <xintc.h>
#include <xaxidma.h>
#include <xstatus.h>

#include "audio_engine.h"
#include "audio/audio.h"
#include "iic/iic.h"
#include "intc/intc.h"

#define RECORD_LENGTH (5*96000)

int is_verbose = 0;
volatile int audio_codec_busy;

extern volatile int audio_dma_error;
extern void (*dma_operation_colmplete_cb)(void);

XIic    *sIicPtr;
XAxiDma *sAxiDmaPtr;

void finish_audio_task(void) {
    Xil_Out32(I2S_STREAM_CONTROL_REG, 0x00000000);
    Xil_Out32(I2S_TRANSFER_CONTROL_REG, 0x00000000);
    microblaze_flush_dcache();
    microblaze_invalidate_dcache();
    if (audio_dma_error) {
        audio_dma_error = 0;
        print("\n\rAudio DMA Error\n\r");
    }
    print("Audio Task Complete\n\r");
    audio_codec_busy = 0;
}

XStatus initialize_audio(XIic *IicPtr, XAxiDma *AxiDmaPtr) {
    print("Initializing Audio...\n\r");

    sIicPtr    = IicPtr;
    sAxiDmaPtr = AxiDmaPtr;
    int status = XST_SUCCESS;

    if (XST_SUCCESS == status) {
        status = fnInitIic(IicPtr);
    }

    if (XST_SUCCESS == status) {
        status = fnConfigAudioDma(AxiDmaPtr);
    }

    if (XST_SUCCESS == status) {
        status = fnInitAudio();
    }

    fnSetLineInput();
    fnSetHpOutput();
    is_verbose = 0;

    dma_operation_colmplete_cb = finish_audio_task;

    return status;
}

XStatus rec_audio(void) {
    if (audio_codec_busy) {
        print("Audio Codec Busy; Can't Record!\n\r");
        return XST_SUCCESS;
    }

    audio_codec_busy = 1;
    print("Start Recording Audio\n\r");
    fnAudioRecord(*sAxiDmaPtr, (u32) RECORD_LENGTH);
    return XST_SUCCESS;
}


XStatus play_audio(void) {
    if (audio_codec_busy) {
        print("Audio Codec Busy; Can't Record!\n\r");
        return XST_SUCCESS;
    }

    print("Start Playing Audio\n\r");

    audio_codec_busy = 1;
    fnAudioPlay(*sAxiDmaPtr, (u32) RECORD_LENGTH);
    return XST_SUCCESS;
}
