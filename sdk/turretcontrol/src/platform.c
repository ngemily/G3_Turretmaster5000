/******************************************************************************
* Author: Patrick Payne
*
* Basic routines for initializing and tearing down the various hardware
* peripherals attached to the Microblaze processor.
*
******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"

void enable_caches() {
#ifdef __PPC__
    Xil_ICacheEnableRegion(CACHEABLE_REGION_MASK);
    Xil_DCacheEnableRegion(CACHEABLE_REGION_MASK);
#elif __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
    Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
    Xil_DCacheEnable();
#endif
#endif
}


void disable_caches() {
    Xil_DCacheDisable();
    Xil_ICacheDisable();
}





void init_platform() {
    enable_caches();

}


void cleanup_platform() {
    disable_caches();
}
