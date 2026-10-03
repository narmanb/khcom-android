#!/usr/bin/env python3
"""Build the KHCoM Android port from a legally dumped GBA ROM."""

import argparse
import hashlib
import os
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

VERSIONS = {
    "us": ("B8CE", "10729bd884f8fdca7a310b6d606c52e46657aa48"),
    "jp": ("B8CJ", "59ec0a0a4ccd1e6acb3bbd7bfb21d63988958cfa"),
    "eu": ("B8CP", "8db73586cdb11b3795907edebf43228dbcd3e6b2"),
}


def run(*cmd, cwd=ROOT, env=None):
    print("+", " ".join(str(x) for x in cmd), flush=True)
    subprocess.run([str(x) for x in cmd], cwd=cwd, env=env, check=True)


def sha1(path):
    h = hashlib.sha1()
    with Path(path).open("rb") as handle:
        while True:
            block = handle.read(1024 * 1024)
            if not block:
                break
            h.update(block)
    return h.hexdigest()


def find_ndk(explicit):
    if explicit:
        path = Path(explicit).expanduser()
        if path.is_dir():
            return path
        raise SystemExit(f"error: Android NDK not found at {path}")

    for name in ("ANDROID_NDK_HOME", "ANDROID_NDK_ROOT"):
        value = os.environ.get(name)
        if value and Path(value).is_dir():
            return Path(value)

    sdk = os.environ.get("ANDROID_SDK_ROOT") or os.environ.get("ANDROID_HOME")
    if sdk:
        ndk_root = Path(sdk) / "ndk"
        if ndk_root.is_dir():
            choices = sorted(
                (path for path in ndk_root.iterdir() if path.is_dir()),
                reverse=True,
            )
            if choices:
                return choices[0]

    raise SystemExit(
        "error: Android NDK not found; set ANDROID_NDK_HOME or pass --ndk"
    )


def require(command, explanation):
    path = shutil.which(command)
    if path is None:
        raise SystemExit(f"error: {command} is required ({explanation})")
    return path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", help="path to your legally dumped KHCoM GBA ROM")
    parser.add_argument("--version", choices=VERSIONS, default="us")
    parser.add_argument("--ndk", help="Android NDK directory")
    parser.add_argument("--api", type=int, default=23)
    parser.add_argument(
        "--native-only",
        action="store_true",
        help="stop after libkhcom.so + rommap.bin",
    )
    parser.add_argument(
        "--gradle",
        help="Gradle executable for APK packaging (defaults to gradle on PATH)",
    )
    args = parser.parse_args()

    os.chdir(ROOT)

    code, expected = VERSIONS[args.version]
    source = Path(args.rom).expanduser().resolve()
    if not source.is_file():
        raise SystemExit(f"error: ROM not found: {source}")

    actual = sha1(source)
    if actual.lower() != expected:
        raise SystemExit(
            f"error: wrong {args.version.upper()} ROM revision\n"
            f"expected SHA-1: {expected}\n"
            f"actual SHA-1:   {actual}"
        )

    require("python3", "project build scripts")
    require("ninja", "reference and native builds")
    require("arm-none-eabi-nm", "GBA symbol extraction")
    require("arm-none-eabi-readelf", "GBA symbol extraction")

    ndk = find_ndk(args.ndk)

    rom_dir = ROOT / "roms"
    rom_dir.mkdir(parents=True, exist_ok=True)
    target_rom = rom_dir / f"{code}.gba"
    if source != target_rom.resolve():
        print(f"copying verified ROM to {target_rom}")
        shutil.copyfile(source, target_rom)

    # The reference GBA build is not just a sanity check: its ELF/map are the
    # authoritative address layout used to construct the native ROM aliases.
    run("python3", "tools/extract_assets.py", args.version)
    run("python3", "configure.py", "--version", args.version)
    run("ninja")

    run(
        "python3",
        "tools/android/configure_android.py",
        "--version",
        args.version,
        "--ndk",
        ndk,
        "--api",
        str(args.api),
    )
    run("ninja", "-f", "build.android.ninja", "android-native")

    package = ROOT / "build" / "android" / args.version / "package"
    library = package / "jniLibs" / "armeabi-v7a" / "libkhcom.so"
    rommap = package / "assets" / "rommap.bin"
    audit = package / "rom_audit.txt"

    for output in (library, rommap, audit):
        if not output.is_file():
            raise SystemExit(f"error: expected Android output missing: {output}")

    print(f"native library: {library}")
    print(f"ROM map:        {rommap}")
    print(f"ROM audit:      {audit}")

    if args.native_only:
        return 0

    gradle = args.gradle or shutil.which("gradle")
    if gradle is None:
        print(
            "native build completed; Gradle is not installed, so APK packaging "
            "was skipped. Run Gradle 9.6+ in port/android with :app:assembleDebug."
        )
        return 0

    run(gradle, ":app:assembleDebug", cwd=ROOT / "port" / "android")
    apk = ROOT / "port" / "android" / "app" / "build" / "outputs" / "apk" / "debug" / "app-debug.apk"
    if not apk.is_file():
        raise SystemExit(f"error: Gradle succeeded but APK was not found at {apk}")

    print(f"APK:            {apk}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
