# Building KHCoM Android

The Android port does not contain or distribute *Kingdom Hearts: Chain of
Memories* ROM data. You must provide your own unmodified cartridge dump.

The first target is the US release:

- code: `B8CE`
- expected SHA-1: `10729bd884f8fdca7a310b6d606c52e46657aa48`

## Dependencies

The normal decomp build dependencies are still required because the Android
linker map is derived from a verified matching GBA build:

- Python 3 + PyYAML
- Ninja
- `binutils-arm-none-eabi`
- pret/agbcc installed into the repository as described by the upstream README
- Android NDK
- Gradle 9.6+ for APK packaging

The native game process is intentionally `armeabi-v7a`. Do not switch it to
`arm64-v8a`: the original game has 32-bit pointer fields in runtime structures
and copies ARM32 FMV decoder code into executable memory.

## One-command build

From the repository root:

```sh
python3 tools/android/build_android.py /path/to/your/KingdomHeartsCoM.gba
```

The driver:

1. verifies the ROM SHA-1 before copying it into `roms/`;
2. extracts the upstream assets;
3. builds the matching reference GBA ELF/map;
4. generates the ARMv7 Android build;
5. links `libkhcom.so`;
6. strips ROM-derived data back out of the library;
7. emits and audits `rommap.bin`;
8. packages a debug APK when a compatible `gradle` command is available.

Use `--native-only` to stop before Gradle packaging.

If the NDK is not discoverable through `ANDROID_NDK_HOME`,
`ANDROID_NDK_ROOT`, `ANDROID_SDK_ROOT`, or `ANDROID_HOME`, pass it
explicitly:

```sh
python3 tools/android/build_android.py game.gba --ndk /path/to/android-ndk
```

## Outputs

After the native stage:

- `build/android/us/package/jniLibs/armeabi-v7a/libkhcom.so`
- `build/android/us/package/assets/rommap.bin`
- `build/android/us/package/rom_audit.txt`

After APK packaging:

- `port/android/app/build/outputs/apk/debug/app-debug.apk`

The APK contains the stripped native executable and relocation map, not the
game ROM. On first launch the app asks the user to choose their own ROM, copies
it to app-private storage, verifies the exact revision, restores the stripped
data in memory, and starts the native game.


## First Android test build (2026-10-04)

The US reference build was rebuilt after the Android fixes and matches SHA-1
`10729bd884f8fdca7a310b6d606c52e46657aa48`. The ARMv7 build, native link,
ROM reconstruction check, sampled ROM audit, and signed APK packaging pass.
Runtime launch and gameplay still require testing on an ARMv7-capable Android
device; they have not been verified by the build environment.

Validated tools: NDK r27c, API 23 native target, SDK 36, build-tools 36.0.0,
JDK 17, Gradle 9.8.0. The GBA tools export omitted GCC's `cc1` backend. For
this build the host GNU cpp was used with the reference build's existing
`-undef -nostdinc` flags; the final matching ROM hash verifies its output.
Install a complete ARM GCC package for the normal `arm-none-eabi-cpp` path.

The Android generator uses Thumb for game C code to match the raw-address
fault decoder, and quote-only game include paths to avoid shadowing Bionic
headers. Explicitly packed animation records override the default four-byte
record alignment. The ARM audio transform uses a PC-relative table reference
and direct native aliases instead of the GBA interworking veneers.

ROM stripping covers Clang's pointer-containing constant tables as well as
ordinary data. It distinguishes repeated shared-library symbol-table entries
from genuinely ambiguous definitions. It preserves known LLVM movie-code
relocations, performs an exact reconstruction check, and still fails on unknown
relocation types or audit hits. `build/android/us/libkhcom.romfree-debug.so`
retains debug information with ROM data removed; the packaged library has its
unneeded symbols stripped separately. Never distribute `libkhcom.unstripped.so`.

### RP5 test sequence

1. Install the test APK and open it.
2. Select the uncompressed, matching USA `.gba` file using Android's picker.
3. Record whether the copyright screens/title appear; try Start and A.
4. If it boots, test D-pad/left stick, A/B, L1/R1, Start/Select and sound.
5. Then test New Game, save/load, the first field and tutorial battle, and FMV.

After a crash, reopen the app and choose **Save report** before continuing.
Send back `KHCoM-test1-diagnostics.txt` and describe the last visible screen.
For a running app, Android Back opens **Save diagnostic report**. The report
contains native startup milestones, load base and registers for unhandled
SIGSEGV, and Android's previous-exit trace where available. It excludes the
ROM and save data. Report export preserves its snapshot across Activity/process
recreation. If the app freezes so completely that export is unavailable, force
close it and return an Android system bug report along with the symptom.
