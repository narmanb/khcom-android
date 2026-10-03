#ifndef KHCOM_ANDROID_HOST_H
#define KHCOM_ANDROID_HOST_H

#include <stdint.h>

/* GBA key bits, active high. */
void AndroidHostSetKeys(uint16_t keys);
void AndroidHostSetPaused(int paused);

/* Save file lives beside the private ROM copy. */
int AndroidHostInitSram(const char* romPath, char* error, unsigned errorSize);
void AndroidHostFlushSram(void);

/* Pull fixed-rate 48 kHz stereo signed-16 frames for Android AudioTrack. */
int AndroidHostReadAudio(int16_t* out, int frames);

/* Latest rendered 240x160 RGBA8888 frame. Owned by the host. */
const uint32_t* AndroidHostGetFrame(void);
uint32_t AndroidHostGetFrameCounter(void);

#endif
