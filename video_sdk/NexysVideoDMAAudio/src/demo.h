/******************************************************************************
 * @file demo.h
 *
 * This is the main Nexys Video Demo application.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who          Date     Changes
 * ----- ------------ -------- -----------------------------------------------
 * 1.00  Mihaita Nagy 05/01/15 First release
 *
 * </pre>
 *
 *****************************************************************************/

#ifndef MAIN_H_
#define MAIN_H_

/***************************** Include Files *********************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xil_io.h"
#include "xstatus.h"
#include "xparameters.h"
#include "microblaze_sleep.h"

/************************** Constant Definitions *****************************/
#define RETURN_ON_FAILURE(x) if ((x) != XST_SUCCESS) return XST_FAILURE;

#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID


#define DDR_BASE_ADDR		XPAR_MIG7SERIES_0_BASEADDR
#define FRAMES_BASE_ADDR	(DDR_BASE_ADDR + 0x08000000)
#define AUDIO_BASE_ADDR		(DDR_BASE_ADDR + 0x10000000)
#define AUDIO_HIGH_ADDR     (XPAR_MIG7SERIES_0_HIGHADDR)

#define RX_INTR_ID		XPAR_INTC_0_AXIDMA_0_S2MM_INTROUT_VEC_ID
#define TX_INTR_ID		XPAR_INTC_0_AXIDMA_0_MM2S_INTROUT_VEC_ID

#define TX_BUFFER_BASE		(FRAMES_BASE_ADDR + 0x00100000)
#define RX_BUFFER_BASE		(FRAMES_BASE_ADDR + 0x00300000)
#define RX_BUFFER_HIGH		(FRAMES_BASE_ADDR + 0x004FFFFF)

#define INTC_DEVICE_ID          XPAR_INTC_0_DEVICE_ID

#define INTC		XIntc
#define INTC_HANDLER	XIntc_InterruptHandler


#define RECORD_LENGTH (5*96000)

// The DMA only allows transfers of up to 8M.
// The DMA works in units of 32bit words.
#define MAX_SONG_LENGTH (0x800000)

// for now, while we debug the random reset issue, we'll set the max length lower
#define MAX_PLAYBACK_LENGTH (15 * 96000)
#define SONG_PATH "song.raw"
#define GUN_PATH "gun.raw"
#define PORTAL_GUN_PATH "portal.raw"
#define TARGET_PATH "target.raw"

#define FRAME_SIZE_BYTES (3 * 1280 * 720)
#define IMAGE_FILE_PATH "image.bin"


typedef enum {
	SOUND_ID_STILL_ALIVE = 0,
	SOUND_ID_MACHINE_GUN,
	SOUND_ID_PORTAL_GUN,
	SOUND_ID_BUTTON_POSITIVE,
	SOUND_ID_BUTTON_NEGATIVE,
	SOUND_ID_TARGET_ACQUIRED,
	SOUND_ID_MAX,
} SoundIndex;

#endif /* MAIN_H_ */
