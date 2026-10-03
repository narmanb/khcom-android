#ifndef KHCOM_ANDROID_PORT_H
#define KHCOM_ANDROID_PORT_H

#include <stdint.h>

#define GBA_SCREEN_WIDTH 240
#define GBA_SCREEN_HEIGHT 160
#define PORT_MAX_SCREEN_WIDTH 288

extern uint8_t gGbaIo[0x400];
extern uint8_t gGbaPltt[0x400];
extern uint8_t gGbaVram[0x18000];
extern uint8_t gGbaOam[0x400];
extern uint8_t gGbaSram[0x10000];

void PortAudioPush(const int8_t* right, const int8_t* left, int samples, int rate);
void PortSramWritten(void);
void PsgNewNotes(unsigned mask);
void* PortCodeAlloc(uint32_t size);
void PortCodeFree(void* p);
void PortCodeBeginWrite(void);
void PortCodeEndWrite(void);

void PortCaptureLine(int y);
void PortCaptureSubmit(void);
void PortVBlankWait(void);
uint16_t PortReadKeys(void);
void PortSoftReset(void) __attribute__((noreturn));
void PortLog(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
void PortFatal(const char* fmt, ...) __attribute__((format(printf, 1, 2), noreturn));

extern volatile uint32_t gPortVBlankIrqs;
extern volatile uint32_t gPortModeUpdates;

#endif
