#define _GNU_SOURCE
#include "android_diagnostics.h"
#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int sLogFd = -1;

void AndroidDiagnosticsLog(const char* message) {
    char line[1200];
    int n;
    if (sLogFd < 0) return;
    n = snprintf(line, sizeof(line), "%s\n", message);
    if (n > 0) {
        size_t len = (size_t)n < sizeof(line) ? (size_t)n : sizeof(line)-1;
        (void)write(sLogFd, line, len);
    }
}

void AndroidDiagnosticsInit(const char* romPath) {
    char path[1200], previous[1232], line[256];
    const char* slash = strrchr(romPath, '/');
    Dl_info info;
    if (!slash || (size_t)(slash-romPath) > sizeof(path)-32) return;
    snprintf(path, sizeof(path), "%.*s/native.log", (int)(slash-romPath), romPath);
    if (sLogFd >= 0) close(sLogFd);
    snprintf(previous, sizeof(previous), "%s.previous", path);
    (void)rename(path, previous);
    sLogFd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND | O_CLOEXEC, 0600);
    AndroidDiagnosticsLog("KHCoM ARMv7 native startup");
    if (dladdr((void*)AndroidDiagnosticsInit, &info)) {
        snprintf(line, sizeof(line), "libkhcom load base=%p", info.dli_fbase);
        AndroidDiagnosticsLog(line);
    }
}

static char* AppendHex(char* p, uint32_t value) {
    static const char hex[] = "0123456789abcdef";
    int i;
    for (i=28; i>=0; i-=4) *p++ = hex[(value >> i) & 15];
    return p;
}

void AndroidDiagnosticsFault(uint32_t fault, const uint32_t regs[16], uint32_t cpsr) {
    char line[512], *p = line;
    const char* prefix = "UNHANDLED SIGSEGV fault=";
    int i;
    if (sLogFd < 0) return;
    while (*prefix) *p++ = *prefix++;
    p = AppendHex(p, fault);
    for (i=0; i<16; i++) {
        *p++ = ' ';
        if (i == 15) { *p++ = 'p'; *p++ = 'c'; }
        else { *p++ = 'r'; *p++ = '0' + i/10; *p++ = '0' + i%10; }
        *p++ = '=';
        p = AppendHex(p, regs[i]);
    }
    prefix = " cpsr=";
    while (*prefix) *p++ = *prefix++;
    p = AppendHex(p, cpsr);
    *p++ = '\n';
    (void)write(sLogFd, line, (size_t)(p-line));
}
