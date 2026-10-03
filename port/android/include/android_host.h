#ifndef KHCOM_ANDROID_HOST_H
#define KHCOM_ANDROID_HOST_H

#include <stdint.h>

/* GBA key bits, active high. */
void AndroidHostSetKeys(uint16_t keys);

/* Latest rendered 240x160 RGBA8888 frame. Owned by the host. */
const uint32_t* AndroidHostGetFrame(void);
uint32_t AndroidHostGetFrameCounter(void);

#endif
