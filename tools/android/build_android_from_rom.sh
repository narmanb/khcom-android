#!/usr/bin/env bash
set -euo pipefail

ROOT="$(CDPATH= cd -- "$(dirname "$0")/../.." && pwd)"
cd "$ROOT"

ROM=""
NDK_ROOT="${ANDROID_NDK_HOME:-${ANDROID_NDK_ROOT:-}}"
OUTPUT="$ROOT/KHCoM-Android-RP5-sourcebuild.apk"
KEYSTORE="${KHCOM_SIGNING_STORE_FILE:-}"
BASELINE_APK=""
if [ -n "${KHCOM_VERSION_CODE:-}" ]; then
    VERSION_CODE="$KHCOM_VERSION_CODE"
elif [ -n "${GITHUB_RUN_NUMBER:-}" ]; then
    VERSION_CODE="$((100000 + GITHUB_RUN_NUMBER))"
else
    VERSION_CODE="3"
fi

if [ -n "${KHCOM_VERSION_NAME:-}" ]; then
    VERSION_NAME="$KHCOM_VERSION_NAME"
elif [ -n "${GITHUB_RUN_NUMBER:-}" ]; then
    VERSION_NAME="0.0.${GITHUB_RUN_NUMBER}"
else
    VERSION_NAME="0.0.3"
fi
CLEAN=0
REQUIRE_UPDATE_SIGNING=0
US_SHA1="10729bd884f8fdca7a310b6d606c52e46657aa48"
AGBCC_REVISION="da598c1d918402c42c0c0d7128ba14567f3175e9"

usage() {
    cat <<'EOF'
Usage: tools/android/build_android_from_rom.sh --rom /path/to/B8CE.gba [options]

Required:
  --rom PATH                 Legally dumped US KHCoM ROM (SHA-1 is verified).

Options:
  --ndk PATH                 Android NDK root. Otherwise uses ANDROID_NDK_HOME/
                             ANDROID_NDK_ROOT or the newest NDK under ANDROID_HOME.
  --output PATH              Output APK path.
  --keystore PATH            Preserved Android signing keystore.
  --baseline-apk PATH        Verify the finished APK uses the same certificate.
  --version-code N           Android versionCode (must increase for updates).
  --version-name NAME        Android versionName.
  --clean                    Remove US GBA/Android build output and extracted assets first.
  --require-update-signing   Fail instead of using Gradle's ephemeral debug signer.
  -h, --help                 Show this help.

Signing passwords/alias may also be supplied with:
  KHCOM_SIGNING_STORE_PASSWORD (default: android)
  KHCOM_SIGNING_KEY_ALIAS      (default: androiddebugkey)
  KHCOM_SIGNING_KEY_PASSWORD   (default: android)
EOF
}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --rom) ROM="$2"; shift 2 ;;
        --ndk) NDK_ROOT="$2"; shift 2 ;;
        --output) OUTPUT="$2"; shift 2 ;;
        --keystore) KEYSTORE="$2"; shift 2 ;;
        --baseline-apk) BASELINE_APK="$2"; shift 2 ;;
        --version-code) VERSION_CODE="$2"; shift 2 ;;
        --version-name) VERSION_NAME="$2"; shift 2 ;;
        --clean) CLEAN=1; shift ;;
        --require-update-signing) REQUIRE_UPDATE_SIGNING=1; shift ;;
        -h|--help) usage; exit 0 ;;
        *) echo "error: unknown argument: $1" >&2; usage >&2; exit 2 ;;
    esac
done

if [ -z "$ROM" ]; then
    echo "error: --rom is required" >&2
    usage >&2
    exit 2
fi
if [ ! -f "$ROM" ]; then
    echo "error: ROM not found: $ROM" >&2
    exit 2
fi
case "$VERSION_CODE" in
    ''|*[!0-9]*) echo "error: --version-code must be a positive integer" >&2; exit 2 ;;
esac
if [ "$VERSION_CODE" -lt 1 ]; then
    echo "error: --version-code must be positive" >&2
    exit 2
fi

required_commands="git python3 ninja make cc sha1sum unzip arm-none-eabi-as arm-none-eabi-ar arm-none-eabi-nm arm-none-eabi-readelf arm-none-eabi-objcopy arm-none-eabi-cpp"
missing=""
for command in $required_commands; do
    if ! command -v "$command" >/dev/null 2>&1; then
        missing="$missing $command"
    fi
done
if [ -n "$missing" ]; then
    echo "error: missing build commands:$missing" >&2
    echo "On Ubuntu install: build-essential flex bison ninja-build git libpng-dev pkg-config gcc-arm-none-eabi binutils-arm-none-eabi python3-pip unzip" >&2
    exit 2
fi

# arm-none-eabi-cpp is a GCC driver and also needs its internal cc1 binary.
if ! printf '#define KHCOM_CPP_PROBE 1\n' | arm-none-eabi-cpp -P - >/dev/null 2>&1; then
    echo "error: arm-none-eabi-cpp is incomplete (its GCC cc1 component is unavailable)." >&2
    echo "Install the complete gcc-arm-none-eabi package; a binutils-only export is not sufficient." >&2
    exit 2
fi

if ! python3 -c 'import yaml, PIL' >/dev/null 2>&1; then
    python3 -m pip install --user --break-system-packages pyyaml pillow 2>/dev/null \
        || python3 -m pip install --user pyyaml pillow
fi

ROM_ABS="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$ROM")"
ROM_TARGET="$ROOT/roms/B8CE.gba"
ROM_TARGET_ABS="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$ROM_TARGET")"
REMOVE_ROM=0

cleanup() {
    if [ "$REMOVE_ROM" -eq 1 ]; then
        rm -f "$ROM_TARGET"
    fi
}
trap cleanup EXIT INT TERM

actual_sha1="$(sha1sum "$ROM" | awk '{print $1}')"
if [ "$actual_sha1" != "$US_SHA1" ]; then
    echo "error: ROM SHA-1 mismatch" >&2
    echo " expected: $US_SHA1" >&2
    echo " actual:   $actual_sha1" >&2
    exit 2
fi

if [ "$CLEAN" -eq 1 ]; then
    rm -rf build/us build/android/us assets/us build.ninja build.android.ninja
fi

if [ ! -x tools/agbcc/bin/old_agbcc ]; then
    echo "==> Bootstrapping pinned pret/agbcc from public source"
    tmp_agbcc="$(mktemp -d)"
    git clone --quiet https://github.com/pret/agbcc.git "$tmp_agbcc/agbcc"
    git -C "$tmp_agbcc/agbcc" checkout --quiet "$AGBCC_REVISION"
    (
        cd "$tmp_agbcc/agbcc"
        ./build.sh
        ./install.sh "$ROOT"
    )
    rm -rf "$tmp_agbcc"
fi

if [ -x tools/legacy/bin/arm-elf-as ] \
   && [ -x tools/legacy/bin/arm-elf-ld ] \
   && [ -f tools/legacy/lib/libgcc.a ] \
   && [ -f tools/legacy/lib/libc.a ] \
   && tools/legacy/bin/arm-elf-as --version | head -1 | grep -Fq 'GNU assembler 2.10' \
   && tools/legacy/bin/arm-elf-ld --version | head -1 | grep -Fq 'GNU ld 2.10'; then
    echo "==> Reusing verified cached legacy toolchain"
else
    echo "==> Building pinned legacy assembler/linker/runtime from public source"
    python3 tools/setup_legacy_toolchain.py
fi

echo "==> Building/reusing gbagfx from public pret source"
sh tools/fetch_gbagfx.sh

mkdir -p roms
if [ "$ROM_ABS" != "$ROM_TARGET_ABS" ]; then
    cp "$ROM" "$ROM_TARGET"
    chmod 600 "$ROM_TARGET"
    REMOVE_ROM=1
fi

echo "==> Extracting US assets from verified user ROM"
python3 tools/extract_assets.py us

echo "==> Building byte-identical US GBA reference"
python3 configure.py --version us
ninja
echo "$US_SHA1  build/us/com_us.gba" | sha1sum -c -

if [ -z "$NDK_ROOT" ]; then
    SDK_ROOT="${ANDROID_HOME:-${ANDROID_SDK_ROOT:-}}"
    if [ -n "$SDK_ROOT" ] && [ -d "$SDK_ROOT/ndk" ]; then
        NDK_ROOT="$(find "$SDK_ROOT/ndk" -mindepth 1 -maxdepth 1 -type d | sort -V | tail -1)"
    fi
fi
if [ -z "$NDK_ROOT" ] || [ ! -x "$NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin/clang" ]; then
    echo "error: Android NDK not found; pass --ndk or set ANDROID_NDK_HOME" >&2
    exit 2
fi

echo "==> Building native armeabi-v7a Android library and ROM restoration map"
python3 tools/android/configure_android.py --version us --ndk "$NDK_ROOT" --api 23
ninja -f build.android.ninja android-native

test -f build/android/us/package/jniLibs/armeabi-v7a/libkhcom.so
test -f build/android/us/package/assets/rommap.bin

# The APK must never contain the user's ROM. Remove the temporary copy before packaging.
if [ "$REMOVE_ROM" -eq 1 ]; then
    rm -f "$ROM_TARGET"
    REMOVE_ROM=0
fi

if ! command -v gradle >/dev/null 2>&1; then
    echo "error: Gradle is not on PATH (Gradle 9.6 is the tested version)" >&2
    exit 2
fi

export KHCOM_VERSION_CODE="$VERSION_CODE"
export KHCOM_VERSION_NAME="$VERSION_NAME"
if [ -n "$KEYSTORE" ]; then
    if [ ! -f "$KEYSTORE" ]; then
        echo "error: keystore not found: $KEYSTORE" >&2
        exit 2
    fi
    export KHCOM_SIGNING_STORE_FILE="$(python3 -c 'import os,sys; print(os.path.abspath(sys.argv[1]))' "$KEYSTORE")"
    export KHCOM_SIGNING_STORE_PASSWORD="${KHCOM_SIGNING_STORE_PASSWORD:-android}"
    export KHCOM_SIGNING_KEY_ALIAS="${KHCOM_SIGNING_KEY_ALIAS:-androiddebugkey}"
    export KHCOM_SIGNING_KEY_PASSWORD="${KHCOM_SIGNING_KEY_PASSWORD:-android}"
elif [ "$REQUIRE_UPDATE_SIGNING" -eq 1 ]; then
    echo "error: update-compatible signing was required but no --keystore was supplied" >&2
    exit 2
else
    echo "warning: no preserved keystore supplied; Gradle will use an ephemeral debug signer." >&2
    echo "warning: that APK may not update an already-installed development build." >&2
fi

echo "==> Packaging Android APK"
(
    cd port/android
    gradle --stacktrace :app:assembleDebug
)

APK="port/android/app/build/outputs/apk/debug/app-debug.apk"
test -f "$APK"
unzip -l "$APK" | grep -q 'lib/armeabi-v7a/libkhcom.so'
unzip -l "$APK" | grep -q 'assets/rommap.bin'
if unzip -l "$APK" | grep -Eiq '\.gba$|B8CE\.gba'; then
    echo "error: ROM file unexpectedly present in APK" >&2
    exit 1
fi

APKSIGNER=""
if [ -n "${ANDROID_HOME:-}" ]; then
    APKSIGNER="$(find "$ANDROID_HOME/build-tools" -type f -name apksigner 2>/dev/null | sort -V | tail -1 || true)"
fi
if [ -n "$APKSIGNER" ] && [ -x "$APKSIGNER" ]; then
    "$APKSIGNER" verify --verbose "$APK" >/dev/null
    if [ -n "$BASELINE_APK" ]; then
        if [ ! -f "$BASELINE_APK" ]; then
            echo "error: baseline APK not found: $BASELINE_APK" >&2
            exit 2
        fi
        new_cert="$("$APKSIGNER" verify --print-certs "$APK" | sed -n 's/^Signer #1 certificate SHA-256 digest: //p' | head -1)"
        old_cert="$("$APKSIGNER" verify --print-certs "$BASELINE_APK" | sed -n 's/^Signer #1 certificate SHA-256 digest: //p' | head -1)"
        if [ -z "$new_cert" ] || [ "$new_cert" != "$old_cert" ]; then
            echo "error: signer differs from baseline APK" >&2
            echo " baseline: $old_cert" >&2
            echo " new:      $new_cert" >&2
            exit 1
        fi
    fi
fi

mkdir -p "$(dirname "$OUTPUT")"
cp "$APK" "$OUTPUT"
echo "==> APK ready: $OUTPUT"
sha256sum "$OUTPUT"
