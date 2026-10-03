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
