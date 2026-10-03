#ifndef KHCOM_ANDROID_HOST_H
#define KHCOM_ANDROID_HOST_H

#include <stdint.h>

/* GBA key bits, active high. */
void AndroidHostSetKeys(uint16_t keys);

/* Save file lives beside the private ROM copy. */
int AndroidHostInitSram(const char* romPath, char* error, unsigned errorSize);
void AndroidHostFlushSram(void);

/* Latest rendered 240x160 RGBA8888 frame. Owned by the host. */
const uint32_t* AndroidHostGetFrame(void);
uint32_t AndroidHostGetFrameCounter(void);

#endif
