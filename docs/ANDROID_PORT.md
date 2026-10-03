# KHCoM Android port

This branch ports the matching GBA decompilation of *Kingdom Hearts: Chain of Memories* to native Android,
with the Retroid Pocket 5 (Android 13+, arm64-v8a) as the primary hardware target.

## Rules for the first port

- Keep gameplay behavior vanilla until the Android build is stable.
- Do not add randomizer logic during platform bring-up.
- Keep Android/platform code under `port/android/` whenever practical.
- Keep game-data/ROM assets out of the repository. The user's legally dumped US ROM remains the source of
  graphics, text, music, movies, and other copyrighted game data.
- Preserve the original GBA build path. Android-only changes to decompiled game code must be isolated behind
  platform conditionals or platform-neutral helpers.
- Treat every GBA address as a 32-bit bus address, never as an Android pointer-sized integer.

## Upstream references

The Android branch currently starts from Pheenoh/khcom commit
`227820f1528fc3c14c4ed15ae1dff6aefaab914a`.

The native Vita port (ggdrn/KH_COM_VITA) is the main portability reference. It already demonstrates that the
game's C logic can run natively while GBA hardware services are replaced by a host platform layer. We will
reuse the architecture, algorithms, and compatible MIT-licensed host code where appropriate, but Android
implementations must account for arm64 and Android lifecycle/API differences.

## Why Android needs its own address layer

The Vita is a 32-bit ARM target, so its port can often cast a pointer to `u32` while deciding whether a value
is a real host pointer or a raw GBA bus address. Android on the RP5 is arm64. Truncating a 64-bit host pointer
to 32 bits can silently turn a valid pointer into an apparent GBA address and cause nonlocal crashes.

The Android port therefore uses `uintptr_t` for host pointers and an explicit 32-bit `GbaAddress` type for
GBA bus addresses. Only values inside known GBA address ranges are translated.

## Port layers

1. **Game code** — the existing decompiled C.
2. **GBA compatibility layer** — emulated IO registers, VRAM, palette RAM, OAM, SRAM, BIOS helpers, DMA,
   interrupt/VBlank behavior, and ROM-address translation.
3. **Portable software devices** — GBA PPU renderer, m4a/PSG audio logic, ROM-data loader.
4. **Android host** — app lifecycle, files/saves, controller input, audio output, threading, timing, logging,
   and OpenGL ES presentation.
5. **Packaging** — Gradle/NDK project producing an arm64 APK.

## Bring-up milestones

- **M0 — scaffold:** Android branch, 64-bit-safe GBA address layer, initial NDK build target.
- **M1 — native game build:** compile the decompiled C and generated data with the Android NDK while keeping
  the original GBA reference build byte-identical.
- **M2 — game loop:** BIOS calls, DMA, interrupt dispatch, VBlank pacing, and input reach `AgbMain()`.
- **M3 — video:** software GBA PPU renders frames; OpenGL ES presents them on Android.
- **M4 — audio:** native m4a mixer + PSG reach Android audio output.
- **M5 — persistence/lifecycle:** ROM selection/loading, SRAM save files, suspend/resume, clean shutdown.
- **M6 — FMV:** movie decoder executable-memory path works on Android.
- **M7 — stabilization:** title screen, new game, saving/loading, field, battles, movies, and multiple worlds
  tested on real RP5 hardware.

Only after M7 should the source-level randomizer be introduced.
