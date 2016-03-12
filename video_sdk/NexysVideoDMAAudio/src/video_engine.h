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

#define DEMO_MAX_FRAME  (1920*1080*3)
#define DEMO_STRIDE     (1920 * 3)

/*
 * Configure the Video capture driver to start streaming on signal
 * detection
 */
#define DEMO_START_ON_DET 1

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

XStatus initialize_video(VideoCapture *videoPtr, XIntc *intcPtr);
void run_video_demo(void);
void VideoISR(void *callBackRef, void *pVideo);

#endif /* VIDEO_ENGINE_H_ */
