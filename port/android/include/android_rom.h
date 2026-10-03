#ifndef KHCOM_ANDROID_ROM_H
#define KHCOM_ANDROID_ROM_H

#include <stddef.h>
#include <stdint.h>

/*
 * Initializes translation of raw 0x08xxxxxx/0x09xxxxxx GBA ROM pointers to
 * their relocated native symbols.
 */
void AndroidRomResolverInit(void);

/*
 * Restore ROM-derived bytes into the stripped native image.
 *
 * romPath points to the user's legally dumped ROM. mapData is the generated
 * rommap.bin packaged with the app. Returns 1 on success, 0 on failure and
 * writes a human-readable error when error/errorSize are provided.
 */
int AndroidRomLoad(const char* romPath, const void* mapData, size_t mapSize,
                   char* error, size_t errorSize);

#endif
