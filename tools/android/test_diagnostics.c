#include "android_diagnostics.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    char dir[] = "/tmp/khcom-log-XXXXXX";
    char path[128], text[4096];
    uint32_t regs[16] = {0};
    FILE* f;
    size_t n;
    assert(mkdtemp(dir));
    snprintf(path, sizeof(path), "%s/rom.gba", dir);
    AndroidDiagnosticsInit(path);
    AndroidDiagnosticsLog("entered AgbMain");
    regs[15] = 0x12345678;
    AndroidDiagnosticsFault(0x08001000, regs, 0x20);
    snprintf(path, sizeof(path), "%s/native.log", dir);
    f = fopen(path, "rb");
    assert(f);
    n = fread(text, 1, sizeof(text)-1, f);
    text[n] = 0;
    fclose(f);
    assert(strstr(text, "entered AgbMain\n"));
    assert(strstr(text, "fault=08001000"));
    assert(strstr(text, "pc=12345678"));
    snprintf(path, sizeof(path), "%s/rom.gba", dir);
    AndroidDiagnosticsInit(path);
    snprintf(path, sizeof(path), "%s/native.log.previous", dir);
    f = fopen(path, "rb");
    assert(f);
    n = fread(text, 1, sizeof(text)-1, f);
    text[n] = 0;
    fclose(f);
    assert(strstr(text, "pc=12345678"));
    unlink(path);
    snprintf(path, sizeof(path), "%s/native.log", dir);
    unlink(path);
    rmdir(dir);
    return 0;
}
