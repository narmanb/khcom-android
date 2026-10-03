#ifndef KHCOM_ANDROID_PORT_H
#define KHCOM_ANDROID_PORT_H

#include <stdint.h>

#define GBA_SCREEN_WIDTH 240
#define GBA_SCREEN_HEIGHT 160

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
