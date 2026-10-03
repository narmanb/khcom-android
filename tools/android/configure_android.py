#!/usr/bin/env python3
"""Generate the native Android/ARMv7 build for KHCoM.

The Android build deliberately depends on a verified normal GBA build for the
same region. The GBA ELF/map provide authoritative ROM addresses and object
layout; Android recompiles the decompiled C natively, relocates ROM data into
writable sections, then tools/android/strip_rom_data.py removes those bytes
again so the packaged app contains no game ROM data.
"""

import argparse
import bisect
import os
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
os.chdir(ROOT)
sys.path.append(str(ROOT / "tools"))

import ninja_syntax  # noqa: E402
from asset_objects import materialize_assets  # noqa: E402
from assetgen import plan as asset_plan  # noqa: E402
from regional_data import asset_symbols, load_sidecars  # noqa: E402
from textgen import load_pools as load_text_pools  # noqa: E402

VERSIONS = {
    "us": "B8CE",
    "jp": "B8CJ",
    "eu": "B8CP",
}

REPLACED_UNITS = {
    "header.s",
    "crt0.s",
    "libagbsyscall.s",
    "m4a_1.s",
    "transform_veneers.s",
}

NATIVE_ARM_UNITS = {"transform.s"}

ROM_SECTIONS = {
    ".text": ".romtext",
    ".rodata": ".romrodata",
    ".data": ".romdata",
}

HW_REGIONS = [
    (0x04000000, 0x400, "gGbaIo"),
    (0x05000000, 0x400, "gGbaPltt"),
    (0x06000000, 0x18000, "gGbaVram"),
    (0x07000000, 0x400, "gGbaOam"),
    (0x0E000000, 0x10000, "gGbaSram"),
]

GAME_CFLAGS = [
    "-std=gnu89",
    "-O2",
    "-g",
    "-fPIC",
    "-fno-strict-aliasing",
    "-fwrapv",
    "-fno-common",
    "-Wno-everything",
]

PORT_CFLAGS = [
    "-std=gnu11",
    "-O2",
    "-g",
    "-fPIC",
    "-fno-strict-aliasing",
    "-fwrapv",
    "-Wall",
    "-Wextra",
    "-Wno-unused-parameter",
]


def ndk_bin(ndk):
    root = Path(ndk) / "toolchains" / "llvm" / "prebuilt"
    candidates = sorted(root.glob("*/bin"))
    for path in candidates:
        if (path / "clang").is_file() and (path / "llvm-objcopy").is_file():
            return path
    raise SystemExit(f"error: Android NDK LLVM toolchain not found under {root}")


def parse_symbols(version):
    out = []
    for line in Path(f"config/{version}/symbols.txt").read_text().splitlines():
        line = line.split("#")[0].strip()
        if not line:
            continue
        name, addr = (x.strip() for x in line.split("="))
        out.append((name, int(addr, 16)))
    return out


def blob_ranges(version):
    ranges = []
    paths = sorted(Path(f"asm/{version}").glob("*.s")) + sorted(Path("asm").glob("*.s"))
    rx = re.compile(
        r'\.incbin\s+"assets/' + re.escape(version) +
        r'/([0-9A-Fa-f]{8})-([0-9A-Fa-f]{8})\.bin"'
    )

    for path in paths:
        text = path.read_text(errors="replace")
        match = rx.search(text)
        label = re.search(r"^(\w+):", text, re.M)
        if match and label:
            ranges.append((int(match[1], 16), int(match[2], 16), label[1]))
    return ranges


def gba_elf_symbols(version, absolute, prefix):
    elf = Path(f"build/{version}/com_{version}.elf")
    if not elf.exists():
        return []

    out = subprocess.check_output([f"{prefix}nm", "-n", str(elf)], text=True)
    syms = []
    for line in out.splitlines():
        parts = line.split()
        if (
            len(parts) != 3
            or not parts[1].isupper()
            or parts[1] == "A"
            or parts[2] in absolute
            or parts[2].startswith(".")
        ):
            continue
        addr = int(parts[0], 16)
        if 0x08000000 <= addr < 0x0A000000:
            syms.append((addr, parts[2]))
    syms.sort()
    return syms


def gba_sections(version):
    """Return {(object path, section): (start, size)} from the GNU GBA map."""
    rx = re.compile(r"^ (\.\w+)\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+) (\S+\.o)$")
    out = {}

    for line in Path(f"build/{version}/com_{version}.map").read_text().splitlines():
        match = rx.match(line)
        if match and int(match[3], 16):
            out.setdefault(match[4], {})[match[1]] = (
                int(match[2], 16),
                int(match[3], 16),
            )
    return out


def write_romsyms(path, version, symbols, prefix):
    absolute = {name for name, _ in symbols}
    blobs = blob_ranges(version)
    sections = gba_sections(version)
    data_secs = [
        value
        for obj, secmap in sections.items()
        if "/gen/" in obj or "/asm/" in obj
        for value in secmap.values()
    ]
    unit_starts = {start for start, _ in data_secs}
    unit_ends = {start + size for start, size in data_secs} - unit_starts
    elf_syms = None
    elf_addrs = []
    lines = []
    unresolved = []

    for name, addr in symbols:
        target = None

        if addr < 0x02000000:
            target = f"0x{addr:X}"
        elif 0x08000000 <= addr < 0x0A000000:
            for start, end, label in blobs:
                if start <= addr < end or (addr == end and addr in unit_ends):
                    target = f"{label} + 0x{addr - start:X}"
                    break

            if target is None:
                if elf_syms is None:
                    elf_syms = gba_elf_symbols(version, absolute, prefix)
                    elf_addrs = [address for address, _ in elf_syms]
                if elf_syms:
                    if addr in unit_ends:
                        index = bisect.bisect_left(elf_addrs, addr) - 1
                    else:
                        index = bisect.bisect_right(elf_addrs, addr) - 1
                    if index >= 0:
                        base, symbol = elf_syms[index]
                        target = f"{symbol} + 0x{addr - base:X}"
        else:
            for base, size, array in HW_REGIONS:
                if base <= addr < base + size:
                    target = f"{array} + 0x{addr - base:X}"
                    break

        if target is None:
            unresolved.append((name, addr))
        else:
            lines.append(f"{name} = {target};")

    if unresolved:
        for name, addr in unresolved[:20]:
            print(f"error: cannot resolve {name} = 0x{addr:08X}", file=sys.stderr)
        if len(unresolved) > 20:
            print(f"error: plus {len(unresolved) - 20} more unresolved symbols", file=sys.stderr)
        raise SystemExit(
            "error: build the verified GBA target first so Android aliases can "
            "be resolved against its ELF/map"
        )

    path.write_text("\n".join(lines) + "\n")


def align_blob_unit(src, dst_dir, anchors):
    if not anchors:
        return None

    prelude = ""
    for sec, start in sorted(anchors.items()):
        prelude += f"\t.section {sec}\n\t.balign 16\n\t.space {start & 15}\n"

    out = prelude + "\t.text\n" + Path(src).read_text()
    dst = Path(dst_dir) / Path(src).name
    dst.parent.mkdir(parents=True, exist_ok=True)
    if not dst.exists() or dst.read_text() != out:
        dst.write_text(out)
    return str(dst)


def unit_anchors(units, version, gba_build, out_dir):
    sections = gba_sections(version)
    last = {}
    out = {}

    for src, obj, _flags in units:
        gba_obj = str(obj).replace(str(out_dir), gba_build, 1)
        is_data = Path(src).suffix != ".c"

        for sec, (start, size) in sorted(
            sections.get(gba_obj, {}).items(), key=lambda item: item[1][0]
        ):
            prev = last.get(sec)
            if (
                is_data
                and sec in (".text", ".rodata", ".data")
                and not (prev and prev[1] and prev[0] == start)
            ):
                out.setdefault(str(obj), {})[sec] = start
            last[sec] = (start + size, is_data)

    return out


def write_romxlate(path, version, gba_objs, aliases, prefix):
    elf = Path(f"build/{version}/com_{version}.elf")
    output = subprocess.check_output([f"{prefix}readelf", "-sW", str(elf)], text=True)
    kinds = {}

    for line in output.splitlines():
        parts = line.split()
        if len(parts) == 8 and parts[4] == "GLOBAL":
            kinds[parts[7]] = (int(parts[1], 16), parts[3])

    linked = set(aliases)
    existing = [obj for obj in gba_objs if Path(obj).exists()]
    for i in range(0, len(existing), 200):
        nm = subprocess.check_output(
            [f"{prefix}nm", "-g", "--defined-only", *existing[i:i + 200]],
            text=True,
        )
        for line in nm.splitlines():
            parts = line.split()
            if len(parts) == 3:
                linked.add(parts[2])

    entries = {}
    for name, (addr, kind) in kinds.items():
        if (
            name not in linked
            or kind == "FUNC"
            or not (0x08000000 <= addr < 0x0A000000)
        ):
            continue
        if addr not in entries or name < entries[addr]:
            entries[addr] = name

    rows = sorted(entries.items())
    lines = [
        "/* Generated by tools/android/configure_android.py. */",
        "#include <stdint.h>",
        "",
        "typedef struct AndroidRomXlate {",
        "    uint32_t gba;",
        "    const uint8_t* host;",
        "} AndroidRomXlate;",
        "",
    ]
    lines += [f"extern char {name}[];" for _, name in rows]
    lines += [
        "",
        "const AndroidRomXlate gRomXlate[] = {",
    ]
    lines += [f"    {{ 0x{addr:08X}u, (const uint8_t*){name} }}," for addr, name in rows]
    lines += [
        "};",
        f"const uint32_t gRomXlateCount = {len(rows)}u;",
        "",
    ]

    text = "\n".join(lines)
    if not path.exists() or path.read_text() != text:
        path.write_text(text)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--version", choices=VERSIONS, default="us")
    parser.add_argument("--ndk", default=os.environ.get("ANDROID_NDK_HOME") or os.environ.get("ANDROID_NDK_ROOT"))
    parser.add_argument("--api", type=int, default=23)
    parser.add_argument("--binutils-prefix", default="arm-none-eabi-")
    args = parser.parse_args()

    if not args.ndk:
        raise SystemExit("error: set ANDROID_NDK_HOME or pass --ndk /path/to/android-ndk")

    version = args.version
    code = VERSIONS[version]
    prefix = args.binutils_prefix
    gba_build = f"build/{version}"
    gba_elf = Path(gba_build) / f"com_{version}.elf"
    gba_map = Path(gba_build) / f"com_{version}.map"
    rom_path = Path("roms") / f"{code}.gba"

    if not gba_elf.exists() or not gba_map.exists():
        raise SystemExit(
            f"error: {gba_elf} and {gba_map} are required. "
            "Build the normal verified GBA target first."
        )

    toolbin = ndk_bin(args.ndk)
    clang = toolbin / "clang"
    objcopy = toolbin / "llvm-objcopy"
    nm = toolbin / "llvm-nm"
    readelf = toolbin / "llvm-readelf"
    target = f"armv7a-linux-androideabi{args.api}"

    out_dir = Path(f"build/android/{version}")
    out_dir.mkdir(parents=True, exist_ok=True)
    # Ninja does not create parent directories for arbitrary outputs.
    for subdir in ("gen", "src", "asm", "aligned", "port_game", "port_host"):
        (out_dir / subdir).mkdir(parents=True, exist_ok=True)

    symbols = parse_symbols(version)
    regional_plan = load_sidecars("config")["regions"][version]
    symbols.extend(asset_symbols(regional_plan, symbols))

    romsyms = out_dir / "romsyms.ld"
    write_romsyms(romsyms, version, symbols, prefix)

    groups = asset_plan(version)
    units_file = Path(f"config/{version}/units.txt")
    listed = {
        line.split()[0]
        for line in units_file.read_text().splitlines()
        if line.strip() and not line.startswith("#")
    }
    groups = {
        name: group
        for name, group in groups.items()
        if not group["objects"] or any(unit in listed for unit in group["objects"])
    }
    generated = {
        unit_name: (group_name, unit)
        for group_name, group in groups.items()
        for unit_name, unit in group["objects"].items()
    }

    text_pools = load_text_pools()
    text_objects = {
        pool.object(version)["name"]: pool
        for pool in text_pools
        if pool.object(version) is not None
    }

    sources = {path.name: path for path in sorted(Path("src").rglob("*.c"))}
    units = []

    for line in units_file.read_text().splitlines():
        line = line.strip()
        if not line or line.startswith("#") or line.startswith("@"):
            continue

        parts = line.split()
        name = parts[0]
        unit_flags = parts[1:]

        if name in REPLACED_UNITS:
            continue

        if name in generated or name in text_objects:
            src = Path(f"{gba_build}/gen") / name
            obj = out_dir / "gen" / (src.stem + ".o")
        elif name.endswith(".c"):
            src = sources.get(name, Path("src") / name)
            obj = out_dir / "src" / (src.stem + ".o")
        else:
            src = Path(f"asm/{version}") / name
            if not src.exists():
                src = Path("asm") / name
            obj = out_dir / "asm" / (src.stem + ".o")

        units.append((src, str(obj), unit_flags))

    units = materialize_assets(regional_plan, units, version, gba_build)
    gba_objs = [
        str(obj).replace(str(out_dir), gba_build, 1)
        for _src, obj, _flags in units
    ]

    romxlate = out_dir / "romxlate.c"
    write_romxlate(
        romxlate,
        version,
        gba_objs,
        [name for name, _ in symbols],
        prefix,
    )

    game_port_sources = sorted(Path("port/android/game").glob("*.c"))
    host_sources = sorted(Path("port/android/host").glob("*.c"))
    include_dirs = ["include"] + sorted(
        str(path)
        for path in Path("include").iterdir()
        if path.is_dir() and path.name != "gba"
    )

    version_text = Path("port/android/VERSION").read_text().strip()
    defines = [
        f"-DVERSION_{version.upper()}",
        "-DPLATFORM_ANDROID=1",
        f"-DPORT_VERSION=\\\"{version_text}\\\"",
    ]
    game_cflags = (
        GAME_CFLAGS
        + defines
        + [f"-I{path}" for path in include_dirs]
        + [f"-I{gba_build}/gen", "-Iport/android/include"]
    )
    port_cflags = (
        PORT_CFLAGS
        + defines
        + ["-Iinclude", "-Iport/android/include"]
    )

    manifests = sorted(os.path.relpath(group["manifest"].path) for group in groups.values())
    gen_headers = sorted(
        [os.path.relpath(group["header"]) for group in groups.values()]
        + [
            os.path.relpath(path)
            for pool in text_pools
            for path in [pool.header(version)] + pool.fragment_paths(version)
        ]
    )

    anchors = unit_anchors(units, version, gba_build, out_dir)

    ninja_path = Path("build.android.ninja")
    with ninja_path.open("w") as f:
        n = ninja_syntax.Writer(f)
        n.variable("ninja_required_version", "1.3")
        n.variable("builddir", str(out_dir))
        n.variable("cc", str(clang))
        n.variable("objcopy", str(objcopy))
        n.variable("nm", str(nm))
        n.variable("readelf", str(readelf))
        n.variable("target", target)
        n.variable("game_cflags", " ".join(game_cflags))
        n.variable("port_cflags", " ".join(port_cflags))
        n.newline()

        n.rule(
            "cc_game",
            "$cc --target=$target $game_cflags $extra -MMD -MF $out.d -c $in -o $out",
            depfile="$out.d",
            deps="gcc",
            description="CC $out",
        )
        n.rule(
            "cc_game_rom",
            "$cc --target=$target $game_cflags $extra -MMD -MF $out.d -c $in -o $out && "
            "$objcopy --rename-section .rodata=.gamerodata,alloc,load,contents,data $out",
            depfile="$out.d",
            deps="gcc",
            description="CCROM $out",
        )
        n.rule(
            "cc_port",
            "$cc --target=$target $port_cflags -MMD -MF $out.d -c $in -o $out",
            depfile="$out.d",
            deps="gcc",
            description="CCPORT $out",
        )
        n.rule(
            "as",
            "$cc --target=$target -fPIC -I. -Iinclude -c $in -o $out",
            description="AS $out",
        )
        rename = " ".join(
            f"--rename-section {old}={new},alloc,load,contents,data"
            for old, new in ROM_SECTIONS.items()
        )
        n.rule(
            "as_rom",
            f"$cc --target=$target -fPIC -I. -Iinclude -c $in -o $out && "
            f"$objcopy {rename} $out",
            description="ASROM $out",
        )
        n.rule(
            "assetgen",
            "python3 tools/assetgen.py $version $manifest",
            description="ASSETGEN $manifest",
            restat=True,
        )
        n.rule(
            "textgen",
            "python3 tools/textgen.py $version $manifest",
            description="TEXTGEN $manifest",
            restat=True,
        )
        n.rule(
            "link",
            "$cc --target=$target -shared -fPIC "
            "-Wl,-soname,libkhcom.so -Wl,-Bsymbolic -Wl,--emit-relocs "
            "-Wl,--no-undefined -Wl,-Map=$out.map "
            "-o $out @$out.rsp -llog -landroid -lm",
            rspfile="$out.rsp",
            rspfile_content="$in",
            description="LINK $out",
        )
        n.rule(
            "strip_rom",
            "python3 tools/android/strip_rom_data.py "
            "--readelf $readelf --nm $nm --gba-readelf $gbareadelf "
            "$in $in.map $gbamap $rom $out $rommap gGbaIo $gbaelf",
            description="STRIPROM $out",
        )
        n.rule(
            "audit_rom",
            "python3 tools/android/audit_rom_free.py $in $rom $out",
            description="AUDIT $in",
        )
        n.newline()

        for group in groups.values():
            outputs = (
                [os.path.relpath(unit["source"]) for unit in group["objects"].values()]
                + [os.path.relpath(group["header"])]
            )
            n.build(
                outputs,
                "assetgen",
                implicit=manifests
                + [
                    "tools/assetgen.py",
                    "tools/m4a_assets.py",
                    "tools/sprite_sheet.py",
                    "tools/gbagfx/gbagfx",
                ]
                + [os.path.relpath(path) for path in group["sources"]],
                implicit_outputs=[
                    os.path.relpath(path) for path in group["binaries"]
                ],
                variables={
                    "version": version,
                    "manifest": os.path.relpath(group["manifest"].path),
                },
            )

        for pool in text_pools:
            n.build(
                [os.path.relpath(path) for path in pool.outputs(version)],
                "textgen",
                implicit=[
                    os.path.relpath(pool.path),
                    f"config/charmaps/{version}.yaml",
                    "tools/textgen.py",
                ]
                + (
                    [os.path.relpath(pool.source(version))]
                    if pool.present(version)
                    else []
                ),
                variables={
                    "version": version,
                    "manifest": os.path.relpath(pool.path),
                },
            )

        objs = []
        for src, obj, unit_flags in units:
            src = Path(src)
            if src.suffix == ".c":
                n.build(
                    obj,
                    "cc_game_rom",
                    str(src),
                    order_only=gen_headers,
                    variables={"extra": " ".join(unit_flags)},
                )
            else:
                deps = (
                    [
                        os.path.relpath(path)
                        for path in generated[src.name][1]["binaries"]
                    ]
                    if src.name in generated
                    else []
                )
                aligned = align_blob_unit(
                    src,
                    out_dir / "aligned",
                    anchors.get(str(obj)),
                )
                rule = "as" if src.name in NATIVE_ARM_UNITS else "as_rom"
                n.build(obj, rule, aligned or str(src), implicit=deps)
            objs.append(obj)

        for src in game_port_sources:
            obj = str(out_dir / "port_game" / (src.stem + ".o"))
            n.build(obj, "cc_game", str(src), order_only=gen_headers, variables={"extra": ""})
            objs.append(obj)

        for src in host_sources:
            obj = str(out_dir / "port_host" / (src.stem + ".o"))
            n.build(obj, "cc_port", str(src))
            objs.append(obj)

        romxlate_obj = str(out_dir / "port_host" / "romxlate.o")
        n.build(romxlate_obj, "cc_port", str(romxlate))
        objs.append(romxlate_obj)

        # Symbol assignments are consumed by lld as a linker-script input.
        objs.append(str(romsyms))

        unstripped = str(out_dir / "libkhcom.unstripped.so")
        n.build(unstripped, "link", objs)

        package_dir = out_dir / "package"
        jni_dir = package_dir / "jniLibs" / "armeabi-v7a"
        asset_dir = package_dir / "assets"
        jni_dir.mkdir(parents=True, exist_ok=True)
        asset_dir.mkdir(parents=True, exist_ok=True)
        stripped = str(jni_dir / "libkhcom.so")
        rommap = str(asset_dir / "rommap.bin")
        n.build(
            stripped,
            "strip_rom",
            unstripped,
            implicit=[
                "tools/android/strip_rom_data.py",
                str(gba_map),
                str(gba_elf),
                str(rom_path),
            ],
            implicit_outputs=[rommap],
            variables={
                "rommap": rommap,
                "rom": str(rom_path),
                "gbamap": str(gba_map),
                "gbaelf": str(gba_elf),
                "gbareadelf": f"{prefix}readelf",
            },
        )

        audit = str(package_dir / "rom_audit.txt")
        n.build(
            audit,
            "audit_rom",
            stripped,
            implicit=["tools/android/audit_rom_free.py", str(rom_path)],
            variables={"rom": str(rom_path)},
        )

        n.build("android-native", "phony", [stripped, rommap, audit])
        n.default("android-native")

    print(f"configured Android ARMv7 build for {version}")
    print("run: ninja -f build.android.ninja")


if __name__ == "__main__":
    main()
