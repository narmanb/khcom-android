# KHCoM Android port

This branch ports the matching GBA decompilation of *Kingdom Hearts: Chain of Memories* to native Android,
with the Retroid Pocket 5 (Android 13) as the primary hardware target.

## Target ABI: 32-bit ARM on purpose

The Android game core targets **armeabi-v7a**, not arm64-v8a.

That is a compatibility decision, not a performance limitation. The original game and the successful Vita
port both use 32-bit pointers throughout runtime structures. Compiling the decompiled game directly as
AArch64 would enlarge pointer fields and change structure offsets. The game also copies an ARM32 movie
decoder into executable RAM at runtime; a 64-bit process cannot execute that AArch32 code directly.

The RP5's Android installation supports 32-bit applications, so a 32-bit native process preserves the
original ABI and lets us reuse substantially more of the proven Vita portability work. The Snapdragon 865
has vastly more CPU performance than this game needs.

We still use uintptr_t at platform boundaries and explicitly distinguish GBA bus addresses from host
pointers. That keeps address translation auditable and avoids depending on accidental pointer ranges.

## Rules for the first port

- Keep gameplay behavior vanilla until the Android build is stable.
- Do not add randomizer logic during platform bring-up.
- Keep Android/platform code under `port/android/` whenever practical.
- Keep game-data/ROM assets out of the repository. The user's legally dumped US ROM remains the source of
  graphics, text, music, movies, and other copyrighted game data.
- Preserve the original GBA build path. Android-only changes to decompiled game code must be isolated behind
  `PLATFORM_ANDROID` or platform-neutral helpers.
- Preserve the GBA's 32-bit data layouts.

## Upstream references

The Android branch starts from Pheenoh/khcom commit
`227820f1528fc3c14c4ed15ae1dff6aefaab914a`.

The native Vita port (ggdrn/KH_COM_VITA) is the main portability reference. It demonstrates that the game's
C logic can run natively while GBA hardware services are replaced by a host platform layer. Android reuses
that architecture where appropriate while replacing Vita APIs with Android/NDK equivalents.

## Port layers

1. **Game code** — the existing decompiled C, built as 32-bit ARM.
2. **GBA compatibility layer** — emulated IO registers, VRAM, palette RAM, OAM, SRAM, BIOS helpers, DMA,
   interrupt/VBlank behavior, and ROM-address translation.
3. **Portable software devices** — GBA PPU renderer, m4a/PSG audio logic, ROM-data loader.
4. **Android host** — app lifecycle, files/saves, controller input, audio output, threading, timing, logging,
   executable-memory handling, and OpenGL ES presentation.
5. **Packaging** — Gradle/NDK project producing an armeabi-v7a APK.

## Bring-up milestones

- **M0 — scaffold:** Android branch, explicit GBA address layer, 32-bit ABI decision, initial NDK target.
- **M1 — native game build:** compile the decompiled C and generated data with the Android NDK while keeping
  the original GBA reference build byte-identical.
- **M2 — game loop:** BIOS calls, DMA, interrupt dispatch, VBlank pacing, and input reach `AgbMain()`.
- **M3 — video:** software GBA PPU renders frames; OpenGL ES presents them on Android.
- **M4 — audio:** native m4a mixer + PSG reach Android audio output.
- **M5 — persistence/lifecycle:** ROM selection/loading, SRAM save files, suspend/resume, clean shutdown.
- **M6 — FMV:** the game's ARM32 movie decoder executes from Android-managed executable memory.
- **M7 — stabilization:** title screen, new game, saving/loading, field, battles, movies, and multiple worlds
  tested on real RP5 hardware.

Only after M7 should the source-level randomizer be introduced.
