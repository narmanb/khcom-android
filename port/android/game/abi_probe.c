/*
 * Compile-time ABI probes for the Android game compiler.
 *
 * These deliberately use records whose natural AAPCS boundary would be 1 or
 * 2 bytes. The forced Android game ABI must round both to four bytes, matching
 * the native Vita/GBA-compatible build contract.
 */
#include "types.h"

typedef struct AndroidAbiByteProbe {
    u8 value;
} AndroidAbiByteProbe;

typedef struct AndroidAbiHalfProbe {
    u16 value;
} AndroidAbiHalfProbe;

typedef union AndroidAbiUnionProbe {
    u8 b;
    u16 h;
} AndroidAbiUnionProbe;

_Static_assert(sizeof(void*) == 4, "KHCoM game core requires 32-bit pointers");
_Static_assert(sizeof(AndroidAbiByteProbe) == 4,
               "game structs must use a 4-byte size boundary");
_Static_assert(sizeof(AndroidAbiHalfProbe) == 4,
               "halfword-only structs must use a 4-byte size boundary");
_Static_assert(sizeof(AndroidAbiUnionProbe) == 4,
               "game unions must use a 4-byte size boundary");
_Static_assert(_Alignof(AndroidAbiByteProbe) == 4,
               "game structs must be 4-byte aligned");
