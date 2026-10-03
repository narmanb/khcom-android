#!/usr/bin/env python3
"""Remove ROM-derived bytes from the Android shared library.

The Android native link initially contains the same ROM-derived asset/data units
used to resolve and relocate the decompiled program. This tool verifies those
bytes against the user's ROM, zeros them in the shared library, and emits a
small rommap.bin. At runtime AndroidRomLoad() verifies the user's ROM and
restores the runs before AgbMain() starts.
"""

import argparse
import bisect
import hashlib
import re
import struct
import subprocess
from pathlib import Path

ROM_BASE = 0x08000000
ANDROID_TO_GBA = {
    ".romtext": ".text",
    ".romrodata": ".rodata",
    ".romdata": ".data",
}
SYMBOL_SECTIONS = (".gamerodata", ".data")
MAGIC = b"KHRM"
FORMAT_VERSION = 1


def parse_gnu_map(path, wanted):
    out = {}
    lines = Path(path).read_text(errors="replace").splitlines()
    rx_full = re.compile(r"^ (\.[\w.]+)\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+) (\S+\.o)$")
    rx_name = re.compile(r"^ (\.[\w.]+)$")
    rx_rest = re.compile(r"^\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+) (\S+\.o)$")
    i = 0

    while i < len(lines):
        m = rx_full.match(lines[i])
        if m:
            sec, addr, size, obj = m[1], int(m[2], 16), int(m[3], 16), m[4]
        else:
            m = rx_name.match(lines[i])
            r = rx_rest.match(lines[i + 1]) if m and i + 1 < len(lines) else None
            if not r:
                i += 1
                continue
            sec, addr, size, obj = m[1], int(r[1], 16), int(r[2], 16), r[3]
            i += 1

        if sec in wanted and size:
            out[(sec, Path(obj).name)] = (addr, size)
        i += 1

    return out


def parse_lld_map(path, wanted):
    """Parse ELF lld's: VMA LMA Size Align Out In Symbol."""
    out = {}
    rx = re.compile(
        r"^\s*([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+([0-9a-fA-F]+)\s+"
        r"\d+\s+(.+?\.o):\((\.[\w.]+)(?:\+0x[0-9a-fA-F]+)?\)\s*$"
    )

    for line in Path(path).read_text(errors="replace").splitlines():
        m = rx.match(line)
        if not m:
            continue
        addr = int(m[1], 16)
        size = int(m[3], 16)
        obj = Path(m[4]).name
        sec = m[5]
        if sec in wanted and size:
            out[(sec, obj)] = (addr, size)

    return out


def elf_sections(data):
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        raise SystemExit("error: Android ROM stripper requires a 32-bit little-endian ELF")

    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", data, 0x2E)
    raw = [struct.unpack_from("<10I", data, shoff + i * shentsize) for i in range(shnum)]
    strtab = raw[shstrndx]
    names = data[strtab[4]:strtab[4] + strtab[5]]
    secs = {}

    for index, s in enumerate(raw):
        end = names.index(b"\0", s[0])
        name = names[s[0]:end].decode()
        secs[name] = {
            "index": index,
            "type": s[1],
            "flags": s[2],
            "addr": s[3],
            "offset": s[4],
            "size": s[5],
            "info": s[7],
        }

    return secs


def reloc_words(data, secs, name):
    rel = secs.get(".rel" + name)
    if rel is None:
        return set()

    out = set()
    for i in range(0, rel["size"], 8):
        r_offset, r_info = struct.unpack_from("<II", data, rel["offset"] + i)
        rtype = r_info & 0xFF
        if rtype == 40:  # R_ARM_V4BX: instruction annotation, no data patch.
            continue
        if rtype not in (2, 3, 38):  # ABS32, REL32, TARGET1
            raise SystemExit(
                f"error: unexpected ARM relocation type {rtype} at {r_offset:08X} in {name}"
            )
        out.add(r_offset)
    return out


def object_symbols(elf, readelf):
    seen = {}
    dup = set()

    output = subprocess.check_output([readelf, "-sW", elf], text=True)
    for line in output.splitlines():
        parts = line.split()
        if len(parts) != 8 or parts[3] != "OBJECT" or parts[6] in ("UND", "ABS", "COM"):
            continue
        size = int(parts[2], 0)
        if size == 0:
            continue
        name = parts[7]
        if name in seen:
            dup.add(name)
        seen[name] = (int(parts[1], 16), size, int(parts[6]))

    return {k: v for k, v in seen.items() if k not in dup}


def symbol_address(elf, name, nm):
    output = subprocess.check_output([nm, elf], text=True)
    for line in output.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[2] == name:
            return int(parts[0], 16)
    raise SystemExit(f"error: {name} not found in {elf}")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--readelf", required=True)
    p.add_argument("--nm", required=True)
    p.add_argument("--gba-readelf", default="arm-none-eabi-readelf")
    p.add_argument("library")
    p.add_argument("android_map")
    p.add_argument("gba_map")
    p.add_argument("rom")
    p.add_argument("out_library")
    p.add_argument("rommap")
    p.add_argument("anchor")
    p.add_argument("gba_elf")
    args = p.parse_args()

    data = bytearray(Path(args.library).read_bytes())
    original = bytes(data)
    rom = Path(args.rom).read_bytes()
    secs = elf_sections(data)

    android = parse_lld_map(args.android_map, set(ANDROID_TO_GBA))
    gba = parse_gnu_map(args.gba_map, set(ANDROID_TO_GBA.values()))
    if not android:
        raise SystemExit("error: no Android ROM input sections found in linker map")
    if not gba:
        raise SystemExit("error: no GBA ROM input sections found in linker map")

    runs = []
    blanked = 0
    mismatched = 0

    def pieces_of(sec, start, end):
        relocs = sec.setdefault("relocs", sorted(reloc_words(data, secs, sec["name"])))
        cuts = [w for w in relocs[bisect.bisect_left(relocs, start - 3):] if w < end]
        pieces = []
        pos = start
        for word in cuts:
            if word > pos:
                pieces.append((pos, word))
            pos = max(pos, word + 4)
        if pos < end:
            pieces.append((pos, end))
        return pieces

    for name, sec in secs.items():
        sec["name"] = name

    for (asec, obj), (aaddr, asize) in sorted(android.items(), key=lambda kv: kv[1][0]):
        gkey = (ANDROID_TO_GBA[asec], obj)
        if gkey not in gba:
            raise SystemExit(f"error: {obj} {asec} has no GBA counterpart")

        gaddr, gsize = gba[gkey]
        pad = asize - gsize
        if not 0 <= pad < 16:
            raise SystemExit(
                f"error: {obj} {asec}: Android size {asize:#x} vs GBA {gsize:#x}"
            )

        sec = secs[asec]
        start = aaddr + pad
        end = start + gsize
        file_base = sec["offset"] - sec["addr"]
        rom_base = gaddr - ROM_BASE - start

        for a, b in pieces_of(sec, start, end):
            fo = a + file_base
            ro = a + rom_base
            if data[fo:b + file_base] == rom[ro:b + rom_base]:
                data[fo:b + file_base] = bytes(b - a)
                runs.append((a, ro, b - a))
                blanked += b - a
                continue

            run = None
            for x in range(a, b):
                if data[x + file_base] == rom[x + rom_base]:
                    data[x + file_base] = 0
                    blanked += 1
                    if run is None:
                        run = x
                else:
                    mismatched += 1
                    if run is not None:
                        runs.append((run, run + rom_base, x - run))
                        run = None
            if run is not None:
                runs.append((run, run + rom_base, b - run))

    sec_by_index = {v["index"]: k for k, v in secs.items()}
    gba_syms = object_symbols(args.gba_elf, args.gba_readelf)
    android_syms = object_symbols(args.library, args.readelf)
    sym_count = 0
    sym_bytes = 0
    sym_skipped = 0

    for name, (aaddr, asize, shndx) in sorted(android_syms.items(), key=lambda kv: kv[1][0]):
        sname = sec_by_index.get(shndx)
        if sname not in SYMBOL_SECTIONS or name not in gba_syms:
            continue

        gaddr, gsize, _ = gba_syms[name]
        if gsize != asize or not ROM_BASE <= gaddr < ROM_BASE + len(rom):
            continue

        sec = secs[sname]
        file_base = sec["offset"] - sec["addr"]
        rom_base = gaddr - ROM_BASE - aaddr
        pieces = pieces_of(sec, aaddr, aaddr + asize)

        if not all(
            data[a + file_base:b + file_base] == rom[a + rom_base:b + rom_base]
            for a, b in pieces
        ):
            sym_skipped += 1
            continue

        for a, b in pieces:
            data[a + file_base:b + file_base] = bytes(b - a)
            runs.append((a, a + rom_base, b - a))
            sym_bytes += b - a
        sym_count += 1

    blanked += sym_bytes
    runs.sort()

    merged = []
    for run in runs:
        if (
            merged
            and merged[-1][0] + merged[-1][2] == run[0]
            and merged[-1][1] + merged[-1][2] == run[1]
        ):
            merged[-1] = (
                merged[-1][0],
                merged[-1][1],
                merged[-1][2] + run[2],
            )
        else:
            merged.append(run)

    check = bytearray(data)
    loadable = [
        s for s in secs.values()
        if s["type"] == 1 and s["addr"] != 0 and s["size"] != 0
    ]

    for addr, off, length in merged:
        sec = next(
            (s for s in loadable if s["addr"] <= addr < s["addr"] + s["size"]),
            None,
        )
        if sec is None:
            raise SystemExit(f"error: ROM run {addr:#x} is outside a loadable section")
        fo = sec["offset"] + (addr - sec["addr"])
        check[fo:fo + length] = rom[off:off + length]

    if bytes(check) != original:
        raise SystemExit(
            "error: rebuilding the Android library from stripped bytes + ROM did not reproduce it"
        )

    Path(args.out_library).parent.mkdir(parents=True, exist_ok=True)
    Path(args.rommap).parent.mkdir(parents=True, exist_ok=True)
    Path(args.out_library).write_bytes(data)

    header = MAGIC + struct.pack(
        "<III",
        FORMAT_VERSION,
        symbol_address(args.library, args.anchor, args.nm),
        len(merged),
    )
    header += hashlib.sha1(rom).digest() + struct.pack("<I", len(rom))
    body = b"".join(struct.pack("<III", *run) for run in merged)
    Path(args.rommap).write_bytes(header + body)

    print(
        f"stripped {blanked / 1e6:.1f} MB of ROM data in {len(merged)} runs "
        f"({mismatched} differing bytes kept); "
        f"C tables: {sym_count} symbols, {sym_bytes / 1e3:.0f} KB "
        f"({sym_skipped} differing symbols kept)"
    )


if __name__ == "__main__":
    main()
