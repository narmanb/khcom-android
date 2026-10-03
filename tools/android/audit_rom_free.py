#!/usr/bin/env python3
"""Fail if a stripped Android library still contains sampled ROM data."""

import argparse
import re
from pathlib import Path

STRIDE = 0x4000
CHUNK = 64


def main():
    p = argparse.ArgumentParser()
    p.add_argument("library")
    p.add_argument("rom")
    p.add_argument("stamp")
    args = p.parse_args()

    blob = re.sub(b"\x00{64,}", b"\x00" * 63, Path(args.library).read_bytes())
    rom = Path(args.rom).read_bytes()
    found = []
    tested = 0

    for off in range(0, len(rom) - CHUNK, STRIDE):
        chunk = rom[off:off + CHUNK]
        if len(set(chunk)) < 8:
            continue
        tested += 1
        if chunk in blob:
            found.append(off)

    if found:
        raise SystemExit(
            f"error: {len(found)} of {tested} ROM samples remain in {args.library}; "
            f"first at ROM offset {found[0]:#x}"
        )

    Path(args.stamp).write_text(f"{tested} samples, none found\n")
    print(f"audit: none of {tested} ROM samples found in the Android library")


if __name__ == "__main__":
    main()
