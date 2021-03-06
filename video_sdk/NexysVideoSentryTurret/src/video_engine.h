/*
 * video_engine.h
 *
 *  Created on: Mar 11, 2016
 *      Author: Robert
 */

#ifndef VIDEO_ENGINE_H_
#define VIDEO_ENGINE_H_

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"
#include "video_capture/video_capture.h"
#include "intc/intc.h"

/* ------------------------------------------------------------ */
/*					Miscellaneous Declarations					*/
/* ------------------------------------------------------------ */

#define DEMO_PATTERN_0 0
#define DEMO_PATTERN_1 1

#define FRAME_BUFFER_WIDTH  1280
#define FRAME_BUFFER_HEIGHT 720
#define NUM_FRAME_BUFFER    3
#define FRAME_BUFFER_STRIDE (FRAME_BUFFER_WIDTH * 3)
#define FRAME_BUFFER_SIZE   (FRAME_BUFFER_STRIDE * FRAME_BUFFER_HEIGHT)

/*
 * Configure the Video capture driver to start streaming on signal
 * detection
 */
#define DEMO_START_ON_DET 1

enum OUTPUT_RESOULTION {
    RES_1080P,
    RES_720P,
    RES_480P,
    RES_SVGA
};

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

XStatus initialize_video(VideoCapture *videoPtr, XIntc *intcPtr);
XStatus video_set_input_enabled(int enable);
XStatus video_set_input_frame(u32 idx);
XStatus video_set_output_enabled(int enable);
XStatus video_set_output_frame(u32 idx);
XStatus video_set_output_resolution(enum OUTPUT_RESOULTION res);
void    print_video_info(void);

// Demo leftovers
void VideoISR(void *callBackRef, void *pVideo);

#endif /* VIDEO_ENGINE_H_ */
