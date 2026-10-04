# Native Android build

This Android port no longer depends on the private
`ghcr.io/pheenoh/khcom-build:main` container.

The canonical clean build is:

```sh
bash tools/android/build_android_from_rom.sh \
  --rom /path/to/B8CE.gba \
  --ndk /path/to/android-ndk \
  --keystore /path/to/preserved.keystore \
  --version-code 4 \
  --version-name 0.0.4 \
  --require-update-signing \
  --clean
```

## Private inputs

Two files must stay outside git:

1. A legally dumped US *Kingdom Hearts: Chain of Memories* ROM.
   The build accepts only SHA-1
   `10729bd884f8fdca7a310b6d606c52e46657aa48`.
2. The Android signing keystore used by the installed development line.
   Keeping this key stable is required for in-place updates.

The default development signing values are compatible with the preserved
Android debug key:

- alias: `androiddebugkey`
- store password: `android`
- key password: `android`

The values can be overridden with:

- `KHCOM_SIGNING_STORE_PASSWORD`
- `KHCOM_SIGNING_KEY_ALIAS`
- `KHCOM_SIGNING_KEY_PASSWORD`

Do not commit a ROM or keystore. `.gitignore` rejects common keystore
extensions and all GBA ROMs.

## What the build does

The script performs the complete source path:

1. Verifies the user ROM hash.
2. Bootstraps the pinned public `pret/agbcc` revision when needed.
3. Builds the pinned binutils 2.10 assembler/linker and legacy runtime.
4. Builds `gbagfx` from public pret source.
5. Extracts the US assets from the verified user ROM.
6. Builds the normal GBA target and requires its SHA-1 to match the original
   ROM exactly.
7. Uses the verified GBA ELF/map as the authoritative layout for the native
   ARMv7 Android recompilation.
8. Builds `libkhcom.so` for `armeabi-v7a`.
9. Strips ROM-derived bytes and creates `rommap.bin` for runtime restoration.
10. Removes the temporary ROM before Gradle packaging.
11. Packages the APK and verifies that neither a `.gba` file nor B8CE.gba is
    present.
12. When a baseline APK is supplied with `--baseline-apk`, verifies that the
    signing certificate matches.

The final APK is ROM-free; the user still selects their own ROM at runtime.

## Host requirements

On Ubuntu:

```sh
sudo apt-get install \
  build-essential flex bison ninja-build git unzip \
  libpng-dev pkg-config gcc-arm-none-eabi binutils-arm-none-eabi \
  python3-pip
```

Python packages:

```sh
python3 -m pip install pyyaml pillow
```

Gradle 9.6 and an Android SDK/NDK containing API 23+ and compile SDK 36 are
required for APK packaging.

The script explicitly probes `arm-none-eabi-cpp`. A partial binutils export
without GCC's internal `cc1` is rejected early instead of silently producing
a non-matching GBA reference.

## Versioning

Every APK intended to update an installed build must use a larger
`versionCode`.

`port/android/app/build.gradle` accepts:

- `KHCOM_VERSION_CODE`
- `KHCOM_VERSION_NAME`
- `KHCOM_SIGNING_STORE_FILE`

The build script exposes the same settings as command-line options.

## GitHub Actions

`.github/workflows/android-build.yml` now bootstraps and verifies the public
toolchain directly. It does not log into GHCR and does not use the old
`pheenoh/khcom-build` image.

A full APK build in Actions is intentionally conditional because ROMs and
signing keys must not be stored in the repository. To enable the private full
build, configure:

- secret `KHCOM_US_ROM_URL`: a private URL that returns the verified B8CE ROM.
- secret `KHCOM_ANDROID_KEYSTORE_B64`: base64 of the preserved keystore.
- optional secrets for signing passwords/alias listed above.
- optional repository variables `KHCOM_VERSION_CODE` and
  `KHCOM_VERSION_NAME`.

Without those private inputs, Actions still verifies that the complete public
legacy toolchain can be recreated and reports the APK stage as skipped rather
than failing on an inaccessible container.
