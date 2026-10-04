/*
 * Raw GBA bus-address fallback for Android/ARM32.
 *
 * Most hardware/ROM accesses are translated explicitly in source. A few raw
 * bus pointers can still escape from binary data or pointer arithmetic. On a
 * GBA they are valid addresses; in an Android process they fault. This handler
 * recognizes only the mapped GBA regions (IO, palette, VRAM, OAM, ROM, SRAM),
 * decodes the faulting Thumb load/store, performs it against emulated memory,
 * and resumes. NULL/BIOS accesses are intentionally NOT swallowed here: known
 * GBA open-bus quirks are source-patched so an unexpected NULL remains a real
 * native crash.
 */
#include "android_fault.h"
#include "android_gba_memory.h"
#include "android_diagnostics.h"

#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ucontext.h>

#if !defined(__arm__)
#error "KHCoM Android fault translation is only supported in the ARM32 build"
#endif

#define CPSR_THUMB 0x20u
#define REG_SP 13
#define REG_LR 14
#define REG_PC 15

static struct sigaction sOldSegv;

static int IsMappedGbaAddress(uint32_t addr) {
    switch (addr >> 24) {
    case 0x04:
        return (addr & 0x00FFFFFFu) < GBA_IO_SIZE;
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0E:
        return 1;
    default:
        return 0;
    }
}

static unsigned long* RegPtr(mcontext_t* m, int reg) {
    switch (reg) {
    case 0: return &m->arm_r0;
    case 1: return &m->arm_r1;
    case 2: return &m->arm_r2;
    case 3: return &m->arm_r3;
    case 4: return &m->arm_r4;
    case 5: return &m->arm_r5;
    case 6: return &m->arm_r6;
    case 7: return &m->arm_r7;
    case 8: return &m->arm_r8;
    case 9: return &m->arm_r9;
    case 10: return &m->arm_r10;
    case 11: return &m->arm_fp;
    case 12: return &m->arm_ip;
    case REG_SP: return &m->arm_sp;
    case REG_LR: return &m->arm_lr;
    case REG_PC: return &m->arm_pc;
    default: return NULL;
    }
}

static uint32_t ReadReg(mcontext_t* m, int reg) {
    unsigned long* p = RegPtr(m, reg);
    return p != NULL ? (uint32_t)*p : 0;
}

static void WriteReg(mcontext_t* m, int reg, uint32_t value) {
    unsigned long* p = RegPtr(m, reg);

    if (p == NULL) {
        return;
    }
    if (reg == REG_PC) {
        if (value & 1u) {
            m->arm_cpsr |= CPSR_THUMB;
        } else {
            m->arm_cpsr &= ~CPSR_THUMB;
        }
        *p = value & ~1u;
    } else {
        *p = value;
    }
}

static int BusLoad(uint32_t addr, int size, uint32_t* value) {
    uint8_t* p = (uint8_t*)AndroidGbaAddressToHost(addr);

    if (p == NULL) {
        return 0;
    }
    switch (size) {
    case 1:
        *value = *p;
        return 1;
    case 2:
        *value = *(uint16_t*)((uintptr_t)p & ~(uintptr_t)1u);
        return 1;
    case 4:
        *value = *(uint32_t*)((uintptr_t)p & ~(uintptr_t)3u);
        return 1;
    default:
        return 0;
    }
}

static int BusStore(uint32_t addr, int size, uint32_t value) {
    uint8_t* p = (uint8_t*)AndroidGbaAddressToHost(addr);

    if (p == NULL) {
        return 0;
    }
    switch (size) {
    case 1:
        *p = (uint8_t)value;
        return 1;
    case 2:
        *(uint16_t*)((uintptr_t)p & ~(uintptr_t)1u) = (uint16_t)value;
        return 1;
    case 4:
        *(uint32_t*)((uintptr_t)p & ~(uintptr_t)3u) = value;
        return 1;
    default:
        return 0;
    }
}

static uint32_t Extend(uint32_t value, int size, int sign) {
    if (!sign) {
        return value;
    }
    if (size == 1) {
        return (uint32_t)(int32_t)(int8_t)value;
    }
    if (size == 2) {
        return (uint32_t)(int32_t)(int16_t)value;
    }
    return value;
}

static int DoSingle(mcontext_t* m, int load, int size, int sign,
                    int rt, uint32_t addr) {
    uint32_t value;

    if (!IsMappedGbaAddress(addr)) {
        return 0;
    }
    if (load) {
        if (!BusLoad(addr, size, &value)) {
            return 0;
        }
        WriteReg(m, rt, Extend(value, size, sign));
        return 1;
    }
    return BusStore(addr, size, ReadReg(m, rt));
}

static int DoMultiple(mcontext_t* m, int load, uint32_t addr, uint16_t list) {
    int reg;

    for (reg = 0; reg < 16; reg++) {
        uint32_t value;

        if (!(list & (1u << reg))) {
            continue;
        }
        if (!IsMappedGbaAddress(addr)) {
            return 0;
        }
        if (load) {
            if (!BusLoad(addr, 4, &value)) {
                return 0;
            }
            WriteReg(m, reg, value);
        } else if (!BusStore(addr, 4, ReadReg(m, reg))) {
            return 0;
        }
        addr += 4;
    }
    return 1;
}

/* Advance Thumb IT state after manually completing an instruction. */
static void AdvanceIt(mcontext_t* m) {
    uint32_t cpsr = (uint32_t)m->arm_cpsr;
    uint32_t it = ((cpsr >> 25) & 3u) | ((cpsr >> 8) & 0xFCu);

    if (it == 0) {
        return;
    }
    if ((it & 7u) == 0) {
        it = 0;
    } else {
        it = (it & 0xE0u) | ((it << 1) & 0x1Fu);
    }
    cpsr &= ~((3u << 25) | (0x3Fu << 10));
    cpsr |= ((it & 3u) << 25) | ((it >> 2) << 10);
    m->arm_cpsr = cpsr;
}

/* Return Thumb instruction length when handled, otherwise 0. */
static int EmulateThumb(mcontext_t* m) {
    uint32_t pcValue = (uint32_t)m->arm_pc;
    const uint16_t* pc = (const uint16_t*)(uintptr_t)pcValue;
    uint16_t hw1 = pc[0];
    uint16_t hw2;

    /* 16-bit LDR/STR register-offset family. */
    if ((hw1 & 0xF000u) == 0x5000u) {
        static const struct {
            uint8_t load;
            uint8_t size;
            uint8_t sign;
        } ops[8] = {
            {0, 4, 0}, {0, 2, 0}, {0, 1, 0}, {1, 1, 1},
            {1, 4, 0}, {1, 2, 0}, {1, 1, 0}, {1, 2, 1},
        };
        int op = (hw1 >> 9) & 7;
        uint32_t addr = ReadReg(m, (hw1 >> 3) & 7) +
                        ReadReg(m, (hw1 >> 6) & 7);
        return DoSingle(m, ops[op].load, ops[op].size, ops[op].sign,
                        hw1 & 7, addr) ? 2 : 0;
    }

    /* 16-bit word/byte immediate LDR/STR. */
    if ((hw1 & 0xE000u) == 0x6000u) {
        int byte = (hw1 >> 12) & 1;
        int imm = ((hw1 >> 6) & 0x1F) * (byte ? 1 : 4);
        uint32_t addr = ReadReg(m, (hw1 >> 3) & 7) + (uint32_t)imm;
        return DoSingle(m, (hw1 >> 11) & 1, byte ? 1 : 4, 0,
                        hw1 & 7, addr) ? 2 : 0;
    }

    /* 16-bit halfword immediate LDRH/STRH. */
    if ((hw1 & 0xF000u) == 0x8000u) {
        uint32_t addr = ReadReg(m, (hw1 >> 3) & 7) +
                        (uint32_t)(((hw1 >> 6) & 0x1F) * 2);
        return DoSingle(m, (hw1 >> 11) & 1, 2, 0,
                        hw1 & 7, addr) ? 2 : 0;
    }

    /* 16-bit LDMIA/STMIA. */
    if ((hw1 & 0xF000u) == 0xC000u) {
        int rn = (hw1 >> 8) & 7;
        int load = (hw1 >> 11) & 1;
        uint16_t list = hw1 & 0xFFu;
        uint32_t addr = ReadReg(m, rn);

        if (!DoMultiple(m, load, addr, list)) {
            return 0;
        }
        if (!load || !(list & (1u << rn))) {
            WriteReg(m, rn, addr + 4u * (uint32_t)__builtin_popcount((unsigned)list));
        }
        return 2;
    }

    /* Not the prefix of a 32-bit Thumb-2 instruction we support. */
    if ((hw1 & 0xE000u) != 0xE000u || (hw1 & 0x1800u) == 0) {
        return 0;
    }

    hw2 = pc[1];

    /* Thumb-2 single LDR/STR variants. */
    if ((hw1 & 0xFE00u) == 0xF800u) {
        int sign = (hw1 >> 8) & 1;
        int size = 1 << ((hw1 >> 5) & 3);
        int load = (hw1 >> 4) & 1;
        int rn = hw1 & 0xF;
        int rt = hw2 >> 12;
        uint32_t base;
        uint32_t addr;

        if (rn == REG_PC || size == 8 || (sign && !load)) {
            return 0;
        }
        base = ReadReg(m, rn);

        if (hw1 & 0x80u) {
            addr = base + (hw2 & 0xFFFu);
        } else if (hw2 & 0x800u) {
            int pre = (hw2 >> 10) & 1;
            int add = (hw2 >> 9) & 1;
            int writeback = (hw2 >> 8) & 1;
            uint32_t off = hw2 & 0xFFu;
            uint32_t target = add ? base + off : base - off;

            addr = pre ? target : base;
            if (writeback) {
                WriteReg(m, rn, target);
            }
        } else if ((hw2 & 0xFC0u) == 0) {
            addr = base +
                   (ReadReg(m, hw2 & 0xF) << ((hw2 >> 4) & 3));
        } else {
            return 0;
        }

        return DoSingle(m, load, size, sign, rt, addr) ? 4 : 0;
    }

    /* Thumb-2 LDRD/STRD. */
    if ((hw1 & 0xFE40u) == 0xE840u && (hw1 & 0x0120u)) {
        int pre = (hw1 >> 8) & 1;
        int add = (hw1 >> 7) & 1;
        int writeback = (hw1 >> 5) & 1;
        int load = (hw1 >> 4) & 1;
        int rn = hw1 & 0xF;
        int rt = hw2 >> 12;
        int rt2 = (hw2 >> 8) & 0xF;
        uint32_t off = (hw2 & 0xFFu) * 4u;
        uint32_t base;
        uint32_t target;
        uint32_t addr;
        uint32_t lo;
        uint32_t hi;

        if (rn == REG_PC) {
            return 0;
        }
        base = ReadReg(m, rn);
        target = add ? base + off : base - off;
        addr = pre ? target : base;

        if (load) {
            if (!IsMappedGbaAddress(addr) ||
                !IsMappedGbaAddress(addr + 4u) ||
                !BusLoad(addr, 4, &lo) ||
                !BusLoad(addr + 4u, 4, &hi)) {
                return 0;
            }
            WriteReg(m, rt, lo);
            WriteReg(m, rt2, hi);
        } else if (!IsMappedGbaAddress(addr) ||
                   !IsMappedGbaAddress(addr + 4u) ||
                   !BusStore(addr, 4, ReadReg(m, rt)) ||
                   !BusStore(addr + 4u, 4, ReadReg(m, rt2))) {
            return 0;
        }

        if (writeback) {
            WriteReg(m, rn, target);
        }
        return 4;
    }

    /* Thumb-2 LDM/STM IA or DB. */
    if ((hw1 & 0xFF90u) == 0xE880u || (hw1 & 0xFF90u) == 0xE900u) {
        int db = (hw1 & 0xFF90u) == 0xE900u;
        int writeback = (hw1 >> 5) & 1;
        int load = (hw1 >> 4) & 1;
        int rn = hw1 & 0xF;
        uint32_t count = (uint32_t)__builtin_popcount((unsigned)hw2);
        uint32_t base = ReadReg(m, rn);
        uint32_t addr = db ? base - 4u * count : base;

        if (!DoMultiple(m, load, addr, hw2)) {
            return 0;
        }
        if (writeback && !(load && (hw2 & (1u << rn)))) {
            WriteReg(m, rn, db ? base - 4u * count : base + 4u * count);
        }
        return 4;
    }

    return 0;
}

static void ChainOrCrash(int sig, siginfo_t* info, void* context) {
    if (sOldSegv.sa_flags & SA_SIGINFO) {
        if (sOldSegv.sa_sigaction != NULL &&
            sOldSegv.sa_sigaction !=
                (void (*)(int, siginfo_t*, void*))SIG_DFL &&
            sOldSegv.sa_sigaction !=
                (void (*)(int, siginfo_t*, void*))SIG_IGN) {
            sOldSegv.sa_sigaction(sig, info, context);
            return;
        }
    } else if (sOldSegv.sa_handler != NULL &&
               sOldSegv.sa_handler != SIG_DFL &&
               sOldSegv.sa_handler != SIG_IGN) {
        sOldSegv.sa_handler(sig);
        return;
    }

    /*
     * Do not mask a genuine native fault. abort() is async-signal-safe on
     * Android/POSIX and still produces a normal app crash report.
     */
    abort();
}

static void SegvHandler(int sig, siginfo_t* info, void* context) {
    ucontext_t* uc = (ucontext_t*)context;
    mcontext_t* m = &uc->uc_mcontext;
    uint32_t fault = (uint32_t)(uintptr_t)info->si_addr;
    uint32_t pc;
    int len;
    uint32_t regs[16];
    int i;

    if (sig != SIGSEGV ||
        !(m->arm_cpsr & CPSR_THUMB) ||
        !IsMappedGbaAddress(fault)) {
        for (i=0; i<16; i++) regs[i] = ReadReg(m, i);
        AndroidDiagnosticsFault(fault, regs, (uint32_t)m->arm_cpsr);
        ChainOrCrash(sig, info, context);
        return;
    }

    pc = (uint32_t)m->arm_pc;
    len = EmulateThumb(m);
    if (len == 0) {
        for (i=0; i<16; i++) regs[i] = ReadReg(m, i);
        AndroidDiagnosticsFault(fault, regs, (uint32_t)m->arm_cpsr);
        ChainOrCrash(sig, info, context);
        return;
    }

    AdvanceIt(m);
    if ((uint32_t)m->arm_pc == pc) {
        m->arm_pc = pc + (uint32_t)len;
    }
}

int AndroidFaultInit(void) {
    struct sigaction action;

    memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = SegvHandler;
    action.sa_flags = SA_SIGINFO;

    return sigaction(SIGSEGV, &action, &sOldSegv) == 0;
}
