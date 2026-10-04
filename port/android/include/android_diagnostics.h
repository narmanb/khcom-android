#ifndef KHCOM_ANDROID_DIAGNOSTICS_H
#define KHCOM_ANDROID_DIAGNOSTICS_H
#include <stdint.h>
void AndroidDiagnosticsInit(const char* romPath);
void AndroidDiagnosticsLog(const char* message);
/* Fixed-buffer formatting and write(2) only; safe in the fault handler. */
void AndroidDiagnosticsFault(uint32_t fault, const uint32_t regs[16], uint32_t cpsr);
#endif
