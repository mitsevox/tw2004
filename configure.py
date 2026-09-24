#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GW4E69",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# In a git worktree (".git" is a file), use the main checkout's downloaded tools instead of
# downloading into build/: the worktrees' build tool folders are links to the main checkout, and a
# download there would rewrite files other builds are using.
if Path(".git").is_file():
    import subprocess

    _common = subprocess.run(
        ["git", "rev-parse", "--path-format=absolute", "--git-common-dir"],
        capture_output=True, text=True,
    ).stdout.strip()
    _main_build = Path(_common).parent / "build"
    for _attr, _rel in (("dtk", "tools/dtk.exe"), ("objdiff", "tools/objdiff-cli.exe"),
                        ("sjiswrap", "tools/sjiswrap.exe"), ("compilers", "compilers"),
                        ("binutils", "binutils")):
        if getattr(args, _attr) is None and (_main_build / _rel).exists():
            setattr(args, _attr, _main_build / _rel)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
# Confirmed against the original binary: -proc gekko, -fp hardware, -O4.
# Not yet confirmed: -O4,p vs -O4,s, -inline mode, -str, -enum. The rest are template defaults.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline smart",  # EA code shows no automatic inlining; see docs/compiler.md
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    # EA built with data pooling off: each global is addressed on its own. With pooling on,
    # GameTargets.c fn_800F266C reaches its three tables through one base (74%); every other
    # matched function is unaffected.
    '-pragma "pool_data off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    # EA's uninitialised globals sit in .bss in link order, before the SDK's .bss, so they were not
    # COMMON (commons link after every .bss). With pool_data off, -common changes no function.
    "-common off",
    "-use_lmw_stmw on",   # EA game code: prologues use stmw/lmw, not _savegpr
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    "-DVERSION=0",  # SDK revision selector for extern/sdk, see include/GameVersions.h
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
# Nintendo prebuilt the SDK with an older compiler than EA used for the game (see docs/compiler.md).
# Settings taken from the Metroid Prime project, which links the same SDK build.
# Mirrors the Prime project's cflags_base exactly. Not derived from our cflags_base on purpose:
# the SDK needs -inline auto, the EA code needs -inline smart.
cflags_sdk = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-i include",
    "-i extern/sdk/include",
    "-i extern/sdk/libc",
    f"-i build/{config.version}/include",
    "-DVERSION=0",  # SDK revision selector for extern/sdk, see include/GameVersions.h
    "-DCARD_PATCH_2003=1",  # this game's CARD library is the Apr 2 2003 patch (see docs/tw2004-notes.md)
]

# MetroTRK (extern/trk, from the Pikmin 2 decompilation): its own flags, no game includes
cflags_trk = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-w off",
    "-O4,p",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",
    "-i extern/trk/include",
    "-i extern/trk/include/stl",
    "-DVERSION_GPVE01",
    "-use_lmw_stmw on",
    "-common off",
    "-inline auto",
    "-inline deferred",
    "-sdata 0",
    "-sdata2 0",
    "-rostr",  # string literals go to .rodata, as in this game's TRK
]

cflags_runtime = [
    *cflags_sdk,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-char signed",
    "-inline deferred,auto",
]

# REL flags
cflags_rel = [
    *cflags_base,
    "-sdata 0",
    "-sdata2 0",
]

# Compiler / linker baseline. See docs/compiler.md for the evidence.
# GC/2.5 (mwcc 2.4.7 build 105, Feb 2003) is the working baseline. GC/2.6 and GC/2.7
# produced identical output in every test; GC/2.0 differs in ~2% of functions
# (byte masking / bit packing) and is the fallback if such a function refuses to match.
config.linker_version = "GC/2.5"


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_base,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
# EA's shared file library was built with SN ProDG (GCC 2.95) without optimization. See
# docs/compiler.md. tools/prodg/prodgcc.py drives the compiler; tools/project.py selects it when
# mw_version starts with "ProDG/".
cflags_gcc = [
    "-O0",
    "-nostdinc",
    "-I include",
]

config.libs = [
    {
        "lib": "sonicheroes_sdk_0",
        "mw_version": "GC/1.3.2",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-str reuse,pool,readonly", "-common off", "-inline deferred,auto", "-char signed", "-i extern/sonicheroes/include", "-i extern/sonicheroes/include/dolphin", "-i extern/sonicheroes/include/MSL_C", "-i extern/sonicheroes/src", "-ir extern/sonicheroes/src/dolphin", "-i extern/sonicheroes/Include"],
        "progress_category": "sdk",
        "src_dir": "extern/sonicheroes",
        "objects": [
            Object(Matching, "src/MSL_C/strtold.c"),
            Object(Matching, "src/MSL_C/strtoul.c"),
        ],
    },
    {
        "lib": "gauntlet_sdk_1",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/gauntlet/include", "-i extern/gauntlet/include/dolphin", "-i extern/gauntlet/src", "-ir extern/gauntlet/src/dolphin", "-i extern/gauntlet/src/MSL", "-i extern/gauntlet/Include"],
        "progress_category": "sdk",
        "src_dir": "extern/gauntlet",
        "objects": [
            Object(Matching, "src/dolphin/db/odenotstub.c", extra_cflags=["-inline deferred"]),
        ],
    },
    {
        "lib": "ffcc_sdk_2",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/ffcc/include", "-i extern/ffcc/include/dolphin", "-i extern/ffcc/src", "-i extern/ffcc/include/TRK_MINNOW_DOLPHIN", "-ir extern/ffcc/src", "-i extern/ffcc/Include", "-DBUILD_VERSION=0", "-DVERSION_GCCE6W"],
        "progress_category": "sdk",
        "src_dir": "extern/ffcc",
        "objects": [
            Object(Matching, "src/ax/AXAlloc.c"),
            Object(Matching, "src/os/OSAlloc.c"),
            Object(Matching, "src/card/CARDWrite.c"),
        ],
    },
    {
        "lib": "tww_sdk_3",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/tww/include", "-i extern/tww/src", "-ir extern/tww/src/dolphin", "-i extern/tww/src/PowerPC_EABI_Support/MetroTRK", "-i extern/tww/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include", "-i extern/tww/src/PowerPC_EABI_Support/Runtime/Inc", "-DVERSION=0"],
        "progress_category": "sdk",
        "src_dir": "extern/tww",
        "objects": [
            Object(Matching, "src/dolphin/pad/Pad.c"),
        ],
    },
    {
        "lib": "ffcc_sdk_4",
        "mw_version": "GC/2.5",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-str reuse,pool,readonly", "-common off", "-inline deferred,auto", "-char signed", "-i extern/ffcc/include", "-i extern/ffcc/include/dolphin", "-i extern/ffcc/src", "-i extern/ffcc/include/TRK_MINNOW_DOLPHIN", "-ir extern/ffcc/src", "-i extern/ffcc/Include", "-DBUILD_VERSION=0", "-DVERSION_GCCE6W"],
        "progress_category": "sdk",
        "src_dir": "extern/ffcc",
        "objects": [
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/ansi_fp.c"),
        ],
    },
    {
        "lib": "ffcc_sdk_0",
        "mw_version": "GC/1.3.2",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-str reuse,pool,readonly", "-common off", "-inline deferred,auto", "-char signed", "-i extern/ffcc/include", "-i extern/ffcc/include/dolphin", "-i extern/ffcc/src", "-i extern/ffcc/include/TRK_MINNOW_DOLPHIN", "-ir extern/ffcc/src", "-i extern/ffcc/Include", "-DBUILD_VERSION=0", "-DVERSION_GCCE6W"],
        "progress_category": "sdk",
        "src_dir": "extern/ffcc",
        "objects": [
            Object(Matching, "src/Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(Matching, "src/Runtime.PPCEABI.H/GCN_mem_alloc.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/abort_exit.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/alloc.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/ansi_files.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/direct_io.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/file_io.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/mem_funcs.c", mw_version="GC/2.5"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/string.c"),
            Object(Matching, "src/MSL_C/PPCEABI/bare/H/s_atan.c"),
        ],
    },
    {
        "lib": "ffcc_sdk_1",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/ffcc/include", "-i extern/ffcc/include/dolphin", "-i extern/ffcc/src", "-i extern/ffcc/include/TRK_MINNOW_DOLPHIN", "-ir extern/ffcc/src", "-i extern/ffcc/Include", "-DBUILD_VERSION=0", "-DVERSION_GCCE6W"],
        "progress_category": "sdk",
        "src_dir": "extern/ffcc",
        "objects": [
            Object(Matching, "src/vi/vi.c"),
            Object(Matching, "src/ax/AX.c"),
            Object(Matching, "src/ax/AXAux.c"),
            Object(Matching, "src/ax/AXCL.c"),
            Object(Matching, "src/ax/AXOut.c"),
            Object(Matching, "src/ax/AXSPB.c"),
            Object(Matching, "src/ax/AXVPB.c"),
            Object(Matching, "src/ax/AXProf.c"),
            Object(Matching, "src/axfx/reverb_hi.c"),
            Object(Matching, "src/axfx/delay.c"),
            Object(Matching, "src/axfx/axfx.c"),
            Object(Matching, "src/mix/mix.c"),
            Object(Matching, "src/dsp/dsp.c"),
            Object(Matching, "src/dvd/dvdFatal.c"),
            Object(Matching, "src/gx/GXDisplayList.c"),
            Object(Matching, "src/gx/GXTransform.c"),
            Object(Matching, "src/os/OSFont.c"),
            Object(Matching, "src/os/OSLink.c"),
            Object(Matching, "src/os/OSSemaphore.c"),
            Object(Matching, "src/os/__ppc_eabi_init.c"),
            Object(Matching, "src/gba/GBA.c"),
            Object(Matching, "src/card/CARDBios.c"),
            Object(Matching, "src/card/CARDRdwr.c"),
        ],
    },
    {
        "lib": "tww_sdk_2",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/tww/include", "-i extern/tww/src", "-ir extern/tww/src/dolphin", "-i extern/tww/src/PowerPC_EABI_Support/MetroTRK", "-i extern/tww/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include", "-i extern/tww/src/PowerPC_EABI_Support/Runtime/Inc", "-DVERSION=0"],
        "progress_category": "sdk",
        "src_dir": "extern/tww",
        "objects": [
            Object(Matching, "src/dolphin/dvd/dvd.c"),
            Object(Matching, "src/dolphin/os/OSMemory.c"),
            Object(Matching, "src/dolphin/os/OSThread.c"),
        ],
    },
    {
        "lib": "tww_sdk_3",
        "mw_version": "GC/1.3.2",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-str reuse,pool,readonly", "-common off", "-inline deferred,auto", "-char signed", "-i extern/tww/include", "-i extern/tww/src", "-ir extern/tww/src/dolphin", "-i extern/tww/src/PowerPC_EABI_Support/MetroTRK", "-i extern/tww/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include", "-i extern/tww/src/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include", "-i extern/tww/src/PowerPC_EABI_Support/Runtime/Inc", "-DVERSION=0"],
        "progress_category": "sdk",
        "src_dir": "extern/tww",
        "objects": [
            Object(Matching, "src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Src/mbstring.c", mw_version="GC/1.3"),
            Object(Matching, "src/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Double_precision/e_pow.c"),
        ],
    },
    {
        "lib": "tp_sdk_4",
        "mw_version": "GC/1.2.5n",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract off", "-i extern/tp/include", "-i extern/tp/libs/dolphin/include", "-ir extern/tp/libs/dolphin/src", "-i extern/tp/libs", "-i extern/tp/libs/PowerPC_EABI_Support/MetroTRK", "-i extern/tp/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include", "-i extern/tp/libs/PowerPC_EABI_Support/Runtime/Inc", "-i extern/tp/libs/dolphin/Include", "-DVERSION=0", "-DSDK_REVISION=2"],
        "progress_category": "sdk",
        "src_dir": "extern/tp",
        "objects": [
            Object(Matching, "libs/dolphin/src/ar/arq.c"),
            Object(Matching, "libs/dolphin/src/card/CARDOpen.c"),
            Object(Matching, "libs/dolphin/src/card/CARDStat.c"),
            Object(Matching, "libs/dolphin/src/card/CARDStatEx.c"),
        ],
    },
    {
        "lib": "ffcc_sdk_5",
        "mw_version": "GC/1.3.2",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-rostr", "-common off", "-inline deferred,auto", "-char signed", "-sdata 0", "-sdata2 0", "-i extern/ffcc/include", "-i extern/ffcc/include/dolphin", "-i extern/ffcc/src", "-i extern/ffcc/include/TRK_MINNOW_DOLPHIN", "-ir extern/ffcc/src", "-i extern/ffcc/Include", "-DBUILD_VERSION=0", "-DVERSION_GCCE6W"],
        "progress_category": "sdk",
        "src_dir": "extern/ffcc",
        "objects": [
            Object(Matching, "src/TRK_MINNOW_DOLPHIN/dolphin_trk_glue.c"),
        ],
    },
    {
        "lib": "tp_sdk_6",
        "mw_version": "GC/1.3.2",
        "cflags": ["-nodefaults", "-proc gekko", "-align powerpc", "-enum int", "-fp hardware", "-Cpp_exceptions off", "-O4,p", "-inline auto", "-pragma \"cats off\"", "-pragma \"warn_notinlined off\"", "-maxerrors 1", "-nosyspath", "-RTTI off", "-str reuse", "-DNDEBUG=1", "-D__GEKKO__", "-fp_contract on", "-use_lmw_stmw on", "-str reuse,pool,readonly", "-common off", "-inline deferred,auto", "-char signed", "-i extern/tp/include", "-i extern/tp/libs/dolphin/include", "-ir extern/tp/libs/dolphin/src", "-i extern/tp/libs", "-i extern/tp/libs/PowerPC_EABI_Support/MetroTRK", "-i extern/tp/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C++/MSL_Common/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/MSL_Common_Embedded/Math/Include", "-i extern/tp/libs/PowerPC_EABI_Support/MSL/MSL_C/PPC_EABI/Include", "-i extern/tp/libs/PowerPC_EABI_Support/Runtime/Inc", "-i extern/tp/libs/dolphin/Include", "-DVERSION=0", "-DSDK_REVISION=2"],
        "progress_category": "sdk",
        "src_dir": "extern/tp",
        "objects": [
        ],
    },
    {
        "lib": "TRK_MINNOW_DOLPHIN",
        "mw_version": "GC/1.3.2",
        "cflags": cflags_trk,
        "progress_category": "sdk",
        "src_dir": "extern/trk",
        "objects": [
            Object(Matching, "MetroTRK/__exception.s"),
            Object(Matching, "MetroTRK/mainloop.c"),
            Object(Matching, "MetroTRK/nubevent.c"),
            Object(Matching, "MetroTRK/nubinit.c"),
            Object(Matching, "MetroTRK/msg.c"),
            Object(Matching, "MetroTRK/msgbuf.c"),
            # gTRKInputPendingPtr is a common symbol: the linker puts it in .sbss (0x80282A10)
            Object(Matching, "MetroTRK/serpoll.c", extra_cflags=["-common on"]),
            Object(Matching, "MetroTRK/usr_put.c"),
            Object(Matching, "MetroTRK/dispatch.c"),
            Object(Matching, "MetroTRK/msghndlr.c"),
            Object(Matching, "MetroTRK/support.c"),
            Object(Matching, "MetroTRK/mutex_TRK.c"),
            Object(Matching, "MetroTRK/notify.c"),
            Object(Matching, "MetroTRK/flush_cache.c"),
            Object(Matching, "MetroTRK/mem_TRK.c"),
            Object(Matching, "MetroTRK/targimpl.c"),
            Object(Matching, "MetroTRK/targsupp.s"),
            Object(Matching, "MetroTRK/mpc_7xx_603e.c"),
            Object(Matching, "MetroTRK/dolphin_trk.c"),
            Object(Matching, "MetroTRK/main_TRK.c"),
            Object(Matching, "MetroTRK/targcont.c"),
            Object(Matching, "MetroTRK/target_options.c"),
            Object(Matching, "MetroTRK/mslsupp.c"),
            Object(Matching, "MetroTRK/UDP_Stubs.c"),
            # the ddh and gdev drivers use small data (gIsInitialized is in .sbss); the rest of TRK does not
            Object(Matching, "MetroTRK/ddh/main.c", extra_cflags=["-sdata 8", "-sdata2 8"]),
            Object(Matching, "MetroTRK/CircleBuffer.c"),
            Object(Matching, "MetroTRK/gdev/main.c", extra_cflags=["-sdata 8", "-sdata2 8"]),
            Object(Matching, "MetroTRK/MWTrace.c"),
        ],
    },
    {
        "lib": "vi",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
        ],
    },
    {
        "lib": "mtx",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/mtx/mtx.c"),
            Object(Matching, "dolphin/mtx/mtx44.c"),
        ],
    },
    {
        "lib": "ar",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/ar/ar.c"),
        ],
    },
    {
        "lib": "ai",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/ai.c"),
        ],
    },
    {
        "lib": "EASharedFileLib",
        "mw_version": "ProDG/3.5",
        "cflags": cflags_gcc,
        "progress_category": "game",
        "objects": [
            Object(Matching, "Common/Checksum/ChecksumCRC32.c"),
            Object(Matching, "Common/Cipher/CipherXOR.c"),
            Object(Matching, "Common/SharedFileIO/SharedFileIO.c"),
            Object(Matching, "NGC/SharedFileIO/llSharedFileIO.c"),
            Object(Matching, "Common/TagFile/TagFile.c"),
        ],
    },
    {
        "lib": "si",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/si/SIBios.c"),
            Object(Matching, "dolphin/si/SISamplingRate.c"),
        ],
    },
    {
        "lib": "dsp",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/dsp/dsp_task.c"),
            Object(Matching, "dolphin/dsp/dsp_debug.c"),
        ],
    },
    {
        "lib": "pad",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/pad/PadClamp.c"),
        ],
    },
    {
        "lib": "exi",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/exi/EXIBios.c"),
            Object(Matching, "dolphin/exi/EXIUart.c"),
        ],
    },
    {
        "lib": "db",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/db.c"),
        ],
    },
    {
        "lib": "card",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/card/CARDBlock.c"),
            Object(Matching, "dolphin/card/CARDDir.c"),
            Object(Matching, "dolphin/card/CARDRead.c"),
            Object(Matching, "dolphin/card/CARDMount.c"),
            Object(Matching, "dolphin/card/CARDUnlock.c"),
            Object(Matching, "dolphin/card/CARDCheck.c"),
            Object(Matching, "dolphin/card/CARDCreate.c"),
            Object(Matching, "dolphin/card/CARDDelete.c"),
            Object(Matching, "dolphin/card/CARDFormat.c"),
            Object(Matching, "dolphin/card/CARDRename.c"),
        ],
    },
    {
        "lib": "dvd",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/dvd/fstload.c"),
            Object(Matching, "dolphin/dvd/dvdlow.c"),
            Object(Matching, "dolphin/dvd/dvdfs.c"),
            Object(Matching, "dolphin/dvd/dvderror.c"),
            Object(Matching, "dolphin/dvd/dvdqueue.c"),
            Object(Matching, "dolphin/dvd/dvdidutils.c"),
        ],
    },
    {
        "lib": "gx",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/gx/GXPixel.c"),
            Object(Matching, "dolphin/gx/GXFifo.c"),
            Object(Matching, "dolphin/gx/GXMisc.c"),
            Object(Matching, "dolphin/gx/GXAttr.c"),
            Object(Matching, "dolphin/gx/GXBump.c"),
            Object(Matching, "dolphin/gx/GXLight.c"),
            Object(Matching, "dolphin/gx/GXTexture.c"),
            Object(Matching, "dolphin/gx/GXInit.c"),
            Object(Matching, "dolphin/gx/GXTev.c"),
            Object(Matching, "dolphin/gx/GXStubs.c"),
            Object(Matching, "dolphin/gx/GXFrameBuf.c"),
            Object(Matching, "dolphin/gx/GXPerf.c"),
            Object(Matching, "dolphin/gx/GXGeometry.c"),
        ],
    },
    {
        "lib": "os",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(Matching, "dolphin/os/OS.c"),
            Object(Matching, "dolphin/os/OSError.c"),
            Object(Matching, "dolphin/os/OSContext.c"),
            Object(Matching, "dolphin/os/OSReboot.c"),
            Object(Matching, "dolphin/os/OSAlarm.c"),
            Object(Matching, "dolphin/os/OSArena.c"),
            Object(Matching, "dolphin/os/OSCache.c"),
            Object(Matching, "dolphin/os/OSReset.c"),
            Object(Matching, "dolphin/os/OSRtc.c"),
            Object(Matching, "dolphin/os/OSTime.c"),
            Object(Matching, "dolphin/PPCArch.c"),
            Object(Matching, "dolphin/os/OSResetSW.c"),
            Object(Matching, "dolphin/os/OSInterrupt.c"),
            Object(Matching, "dolphin/os/OSAudioSystem.c"),
            Object(Matching, "dolphin/os/OSMutex.c"),
            Object(Matching, "dolphin/os/OSSync.c"),
            Object(Matching, "dolphin/os/__start.c"),
        ],
    },
    {
        "lib": "MSL_C.PPCEABI.bare.H",
        "mw_version": "GC/1.3",
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            # built with the game's compiler, like ansi_fp.c: GC/1.3 misses long2str, float2str and three more
            Object(Matching, "runtime/printf.c", mw_version="GC/2.5"),
            Object(Matching, "runtime/runtime.c"),
            Object(Matching, "runtime/FILE_POS.c"),
            Object(Matching, "runtime/ctype.c"),
            Object(Matching, "runtime/e_acos.c"),
            Object(Matching, "runtime/e_asin.c"),
            Object(Matching, "runtime/e_atan2.c"),
            Object(Matching, "runtime/e_fmod.c"),
            Object(Matching, "runtime/e_log.c"),
            Object(Matching, "runtime/e_rem_pio2.c"),
            Object(Matching, "runtime/k_cos.c"),
            Object(Matching, "runtime/k_rem_pio2.c"),
            Object(Matching, "runtime/k_sin.c"),
            Object(Matching, "runtime/k_tan.c"),
            Object(Matching, "runtime/s_cos.c"),
            Object(Matching, "runtime/s_floor.c"),
            Object(Matching, "runtime/s_frexp.c"),
            Object(Matching, "runtime/s_ldexp.c"),
            Object(Matching, "runtime/s_sin.c"),
            Object(Matching, "runtime/s_tan.c"),
            Object(Matching, "runtime/w_acos.c"),
            Object(Matching, "runtime/w_asin.c"),
            Object(Matching, "runtime/w_atan2.c"),
            Object(Matching, "runtime/w_fmod.c"),
            Object(Matching, "runtime/w_log.c"),
            Object(Matching, "runtime/w_pow.c"),
            Object(Matching, "runtime/extras.c"),
            Object(Matching, "runtime/__mem.c"),
            Object(Matching, "runtime/__va_arg.c"),
            Object(Matching, "runtime/buffer_io.c"),
            Object(Matching, "runtime/mem.c"),
            Object(Matching, "runtime/misc_io.c"),
            Object(Matching, "runtime/s_copysign.c"),
            Object(Matching, "runtime/s_modf.c"),
            Object(Matching, "runtime/qsort.c", mw_version="GC/2.5"),
            Object(Matching, "runtime/wchar_io.c"),
            Object(Matching, "runtime/uart_console_io.c"),
        ],
    },
    {
        "lib": "pikmin2_msl",
        "mw_version": "GC/2.5",
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "src_dir": "extern/pikmin2",
        "objects": [
            Object(Matching, "src/MSL_C/scanf.c"),
        ],
    },
    {
        # EA game code whose original source file is not yet known.
        "lib": "unsorted",
        "mw_version": "GC/2.5",
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": [
            Object(NonMatching, "LLFileIO_Gc.c"),
            Object(NonMatching, "LLDisp_Gc.c"),
            Object(NonMatching, "LLObj_Gc.c"),
            Object(Matching, "unsorted/code_800080D0.c"),
            Object(Matching, "unsorted/code_800082F8.c"),
            Object(NonMatching, "GoEntry.c"),
            Object(NonMatching, "Quaternion.c"),
            Object(NonMatching, "StaticMemory.c"),
            Object(Matching, "UMemPool.c"),
            Object(NonMatching, "urandom.c"),
            Object(Matching, "ObjList.c"),
            Object(Matching, "LoadData.c"),
            Object(NonMatching, "TerrainData.c"),
            Object(NonMatching, "UStream.c"),
            Object(NonMatching, "LLTex.c"),
            Object(NonMatching, "LLTexGrp.c"),
            Object(NonMatching, "LLFont.c"),
            Object(NonMatching, "UFont.c"),
            Object(NonMatching, "GoRenderCtx_Gc.c"),
            Object(NonMatching, "streammanagerhole.c"),
            Object(NonMatching, "ViewController.c"),
            Object(NonMatching, "char_tex_manager.c"),
            Object(NonMatching, "char.c"),
            Object(NonMatching, "mtalib.c"),
            Object(NonMatching, "ska_shared.c"),
            Object(NonMatching, "Skeleton.c"),
            Object(NonMatching, "Golfer.c"),
            Object(NonMatching, "GoRenderSurface.c"),
            Object(NonMatching, "LLPict_Gc.c"),
            Object(NonMatching, "GoTerrain.c"),
            Object(NonMatching, "Skin.c"),
            Object(NonMatching, "GoPostFx.c"),
            Object(NonMatching, "GoDynamicCam.c"),
            Object(NonMatching, "gocamscripts.c"),
            Object(NonMatching, "DepthField.c"),
            Object(NonMatching, "UObject3D.c"),
            Object(NonMatching, "GoDynObj.c"),
            Object(NonMatching, "UObject.c"),
            Object(Matching, "UKernel.c"),
            Object(NonMatching, "GoAnimalActors.c"),
            Object(NonMatching, "GoTerrainCollision.c"),
            Object(NonMatching, "Ball.c"),
            Object(NonMatching, "LLPictInt.c"),
            Object(NonMatching, "user.c"),
            Object(NonMatching, "PasswordManager.c"),
            Object(NonMatching, "Swing.c"),
            Object(NonMatching, "GoCamCont.c"),
            Object(NonMatching, "skalib.c"),
            Object(NonMatching, "GoStaticCam.c"),
            Object(NonMatching, "target.c"),
            Object(NonMatching, "emotion.c"),
            Object(NonMatching, "Replay.c"),
            Object(NonMatching, "gomainloop.c"),
            Object(NonMatching, "GoFrameBuf.c"),
            Object(NonMatching, "GoLighting.c"),
            Object(Matching, "Code8006F154.c"),
            Object(NonMatching, "GoShaderObjectContainer_OBFData_Gc.c"),
            Object(NonMatching, "GoShaderObject_PrelitUVAnimation_Gc.c"),
            Object(NonMatching, "animblender.c"),
            Object(NonMatching, "GoShaderObjectCommon_ShaderObjectsData_Gc.c"),
            Object(NonMatching, "LLVideo.c"),
            Object(NonMatching, "ByteSwap.c"),
            Object(NonMatching, "GoCamera.c"),
            Object(NonMatching, "GoViewport.c"),
            Object(NonMatching, "GoShaderObjectCommon_TexAnimManager_Gc.c"),
            Object(Matching, "FE_Manager.c"),
            Object(NonMatching, "FE_MessageTable.c"),
            Object(NonMatching, "GameUICommands.c"),
            Object(NonMatching, "FEgolferanim.c"),
            Object(NonMatching, "uiLoadFile.c"),
            Object(NonMatching, "uiProcessInterface.c"),
            Object(NonMatching, "fe_movies.c"),
            Object(Matching, "uiTransform.c"),
            Object(NonMatching, "goballfx.c"),
            Object(NonMatching, "GoObjShadow.c"),
            Object(NonMatching, "GoShaderObject_Particle_Gc.c"),
            Object(NonMatching, "CharAnim.c"),
            Object(NonMatching, "GoShaderObjectCommon_MorphAnimManager_Gc.c"),
            Object(NonMatching, "GoCamTuningVars.c"),
            Object(NonMatching, "GoShaderObject_Glows_Gc.c"),
            Object(NonMatching, "UFstPart.c"),
            Object(NonMatching, "BootCourse.c"),
            Object(NonMatching, "SunFlr_Gc.c"),
            Object(NonMatching, "Code8009AA28.c"),
            Object(NonMatching, "GoGreenGrid.c"),
            Object(NonMatching, "MC_Gc.c"),
            Object(NonMatching, "MC.c"),
            Object(NonMatching, "PsMgr.c"),
            Object(NonMatching, "PsBallFx.c"),
            Object(NonMatching, "GameAudio.c"),
            Object(NonMatching, "AudTable.c"),
            Object(NonMatching, "hlaudmovie.c"),
            Object(NonMatching, "hlaudtrackstm.c"),
            Object(Matching, "UAudContainers.c"),
            Object(NonMatching, "uiobject.c"),
            Object(NonMatching, "startUp.c"),
            Object(NonMatching, "shadow.c"),
            Object(NonMatching, "GoComicCam.c"),
            Object(NonMatching, "GoShaderObject_Rain_Gc.c"),
            Object(Matching, "UAudMemStack.c"),
            Object(Matching, "GoARAM.c"),
            Object(NonMatching, "DiscError.c"),
            Object(NonMatching, "rcmp_mad_codec.c"),
            Object(Matching, "Trax.c"),
            Object(NonMatching, "ScreenClear.c"),
            Object(NonMatching, "Code800BA940.c"),
            Object(Matching, "VecMath.c"),
            Object(NonMatching, "SitDevFile.c"),
            Object(NonMatching, "GoGolfCam.c"),
            Object(NonMatching, "CamSpline.c"),
            Object(NonMatching, "GoBreakLine.c"),
            Object(NonMatching, "AnimStream.c"),
            Object(NonMatching, "TerrainGround.c"),
            Object(NonMatching, "SkinPart.c"),
            Object(Matching, "HoleScore.c"),
            Object(NonMatching, "SwingTips.c"),
            Object(NonMatching, "Calendar.c"),
            Object(Matching, "CourseData.c"),
            Object(NonMatching, "Earnings.c"),
            Object(NonMatching, "GameHoleContests.c"),
            Object(Matching, "GameManager.c"),
            Object(Matching, "GameEffects.c"),
            Object(Matching, "GameRound.c"),
            Object(Matching, "GameUI.c"),
            Object(Matching, "GameMessages.c"),
            Object(Matching, "GameAnalysis.c"),
            Object(Matching, "GameModeAlternateShot.c"),
            Object(Matching, "GameModeBattle.c"),
            Object(NonMatching, "GameModeBestBall.c"),
            Object(Matching, "GameModeFourBall.c"),
            Object(Matching, "GameModeMatch.c"),
            Object(NonMatching, "GameMode5.c"),
            Object(Matching, "GameMode9.c"),
            Object(Matching, "GameModeDriverPGATour.c"),
            Object(Matching, "GameModeDriverRTE.c"),
            Object(Matching, "GameModeReplay.c"),
            Object(Matching, "GameTargets.c"),
            Object(Matching, "GameMode14.c"),
            Object(Matching, "GameMode15.c"),
            Object(Matching, "GameMode16.c"),
            Object(Matching, "GameMode17.c"),
            Object(Matching, "GameMode13.c"),
            Object(Matching, "GameMode2.c"),
            Object(Matching, "GameMode6.c"),
            Object(Matching, "GameMode7.c"),
            Object(Matching, "GameMode8.c"),
            Object(Matching, "GameModeStableford.c"),
            Object(Matching, "GameMode12.c"),
            Object(Matching, "GameModeStroke.c"),
            Object(Matching, "GameMode11.c"),
            Object(NonMatching, "GameMode4.c"),
            Object(NonMatching, "uiArc.c"),
            Object(NonMatching, "FE_CrAPDB.c"),
            Object(NonMatching, "FE_CrAPMessages.c"),
            Object(NonMatching, "LLDynTex.c"),
            Object(NonMatching, "GameMode26.c"),
            Object(NonMatching, "CharSliders.c"),
            Object(NonMatching, "FE_PGATourMessages.c"),
            Object(Matching, "FE_LogoDesign.c"),
            Object(Matching, "DiscCheck.c"),
            Object(NonMatching, "hwsBurn.c"),
            Object(NonMatching, "CalendarScreen.c"),
            Object(NonMatching, "hwsMaterial_Gc.c"),
            Object(Matching, "hwsOverride_Gc.c"),
            Object(NonMatching, "hwsRender_Gc.c"),
            Object(NonMatching, "DynChain.c"),
            Object(NonMatching, "GameModeDriver.c"),
            Object(NonMatching, "PGATourSimulation.c"),
            Object(NonMatching, "SkinMorph.c"),
            Object(NonMatching, "fe_stats.c"),
            Object(NonMatching, "EventInfo.c"),
            Object(NonMatching, "llrtclock.c"),
            Object(NonMatching, "GoGrass.c"),
            Object(NonMatching, "GoShaderObject_Grass_Gc.c"),
            Object(NonMatching, "GameMode4Menu.c"),
            Object(NonMatching, "TibExt.c"),
            Object(NonMatching, "gbacable.c"),
            Object(NonMatching, "ShaderRow19.c"),
            Object(NonMatching, "EASportsBio.c"),
            Object(NonMatching, "GameMode22.c"),
            Object(NonMatching, "SkinBurn.c"),
            Object(NonMatching, "EASBStorage.c"),
            Object(NonMatching, "EASB.c"),
            Object(NonMatching, "UISEvent.c"),
            Object(NonMatching, "UIStudio.c"),
            # Built with automatic inlining like UISScreen.c below: fn_80168DB0 88.9 -> 98.7%, no
            # function worse.
            Object(NonMatching, "UISApi.c", extra_cflags=["-inline auto"]),
            # Built with automatic inlining: fn_8016A830 and fn_8016B188 have their own recursion
            # inlined three deep (-inline smart 55.8%, -inline auto 64.7%, nothing worse). Not
            # deferred: -inline auto,deferred emits the functions in reverse order and pastes
            # fn_8016C614/fn_8016C674 and the fn_8016C15C accessors into fn_8016A2D4/fn_8016A510,
            # which the original calls (87 -> 62%, 93 -> 69%).
            Object(NonMatching, "UISScreen.c", extra_cflags=["-inline auto"]),
            Object(Matching, "GoDynObjTypes.c"),
            Object(Matching, "unsorted/sweep_800977CC.c"),
            Object(Matching, "unsorted/sweep_800B1AB0.c"),
            Object(Matching, "unsorted/sweep_8006F568.c"),
            Object(Matching, "unsorted/sweep_80007328.c"),
            Object(Matching, "unsorted/sweep_80007368.c"),
            Object(Matching, "unsorted/sweep_80045F38.c"),
            Object(Matching, "GoDynObjBase.c"),
            Object(Matching, "unsorted/sweep_8006FF2C.c"),
            Object(Matching, "unsorted/sweep_8006FF80.c"),
            Object(Matching, "unsorted/sweep_80095744.c"),
            Object(Matching, "unsorted/sweep_80095780.c"),
            Object(Matching, "unsorted/sweep_80095798.c"),
            Object(Matching, "unsorted/sweep_800957B0.c"),
            Object(Matching, "unsorted/sweep_800957D8.c"),
            Object(Matching, "unsorted/sweep_8009A990.c"),
            Object(Matching, "unsorted/sweep_800AF2DC.c"),
            Object(Matching, "unsorted/sweep_800B2150.c"),

            Object(Matching, "unsorted/sweep_8009A844.c"),
            Object(Matching, "unsorted/sweep_80005520.c"),
            Object(Matching, "unsorted/sweep_80005590.c"),
            Object(Matching, "unsorted/sweep_800055D8.c"),
            Object(NonMatching, "LLMem_Gc.c"),
            Object(Matching, "unsorted/sweep_800072E8.c"),
            Object(Matching, "unsorted/sweep_80045D18.c"),
            Object(Matching, "unsorted/sweep_80045EC0.c"),
            Object(NonMatching, "fe_craputils.c"),
            Object(Matching, "unsorted/sweep_8006F4B4.c"),
            Object(Matching, "unsorted/sweep_8006F518.c"),
            Object(NonMatching, "DynamicRenderingBuffer.c"),
            Object(NonMatching, "Code80070EC4.c"),
            Object(Matching, "unsorted/sweep_80097E98.c"),
            Object(Matching, "unsorted/sweep_8009A928.c"),
            Object(NonMatching, "hlaudvoice.c"),
            Object(Matching, "unsorted/sweep_800B1FBC.c"),
            Object(Matching, "unsorted/sweep_800B20B8.c"),
            Object(Matching, "unsorted/sweep_800B21F0.c"),
            Object(Matching, "AudLock.c"),
            Object(Matching, "unsorted/sweep_8010FF5C.c"),
            Object(Matching, "unsorted/sweep_801654F4.c"),
            Object(Matching, "unsorted/sweep_80005580.c"),
            Object(Matching, "unsorted/sweep_8006F4E0.c"),
            Object(Matching, "unsorted/sweep_8006F608.c"),
            Object(Matching, "unsorted/sweep_8009AA18.c"),

            Object(Matching, "unsorted/sweep_80026B34.c"),
            Object(Matching, "unsorted/sweep_80045F50.c"),
            Object(Matching, "unsorted/sweep_800B223C.c"),
            Object(Matching, "unsorted/sweep_800B208C.c"),
            Object(Matching, "unsorted/sweep_800B2250.c"),
            Object(Matching, "unsorted/sweep_800055D4.c"),
            Object(Matching, "unsorted/sweep_800072E0.c"),
            Object(Matching, "unsorted/sweep_80007320.c"),
            Object(Matching, "unsorted/sweep_800073A8.c"),
            Object(Matching, "unsorted/sweep_80012FA0.c"),
            Object(NonMatching, "GxUtil.c"),
            Object(Matching, "unsorted/sweep_8003944C.c"),
            Object(Matching, "unsorted/sweep_80045D5C.c"),
            Object(Matching, "unsorted/sweep_80045F6C.c"),
            Object(Matching, "unsorted/sweep_8006F4F0.c"),
            Object(Matching, "unsorted/sweep_8006F64C.c"),
            Object(Matching, "unsorted/sweep_8006FFD4.c"),
            Object(Matching, "unsorted/sweep_80070168.c"),
            Object(NonMatching, "uiText.c"),
            Object(Matching, "unsorted/sweep_8009A968.c"),
            Object(NonMatching, "hlaudtrack.c"),
            Object(NonMatching, "hlaudtrackseq.c"),
            Object(NonMatching, "hlaudemitter.c"),
            Object(Matching, "unsorted/sweep_800AF140.c"),
            Object(Matching, "unsorted/sweep_800AF2D8.c"),
            Object(Matching, "unsorted/sweep_800B1AA8.c"),
            Object(Matching, "unsorted/sweep_800B1F9C.c"),
            Object(Matching, "unsorted/sweep_8010FBC4.c"),
            Object(Matching, "unsorted/sweep_8013B3C4.c"),
            Object(Matching, "unsorted/sweep_8014CA80.c"),
            Object(Matching, "unsorted/sweep_801654D0.c"),
            Object(Matching, "unsorted/sweep_80165524.c"),
            Object(Matching, "unsorted/sweep_80012ED0.c"),
            Object(NonMatching, "Controller_Gc.c"),
            Object(Matching, "unsorted/sweep_800136F4.c"),
            Object(Matching, "unsorted/sweep_80045E3C.c"),
            Object(Matching, "unsorted/sweep_80045F14.c"),
            Object(NonMatching, "event.c"),
            Object(NonMatching, "Code80067710.c"),
            Object(Matching, "unsorted/sweep_8006F5F8.c"),
            Object(Matching, "unsorted/sweep_8009554C.c"),
            Object(Matching, "unsorted/sweep_8009B314.c"),
            Object(NonMatching, "Code8009B340.c"),
            Object(Matching, "unsorted/sweep_800AF31C.c"),
            Object(Matching, "unsorted/sweep_800B204C.c"),
            Object(Matching, "unsorted/sweep_800B21D0.c"),
            Object(Matching, "unsorted/sweep_800B22D4.c"),
            Object(Matching, "unsorted/sweep_801338E0.c"),
            Object(Matching, "unsorted/sweep_80155F40.c"),
        ],
    },
    {
        "lib": "Runtime.PPCEABI.H",
        "mw_version": config.linker_version,
        "cflags": cflags_runtime,
        "progress_category": "sdk",  # str | List[str]
        "objects": [
            Object(NonMatching, "Runtime.PPCEABI.H/global_destructor_chain.c"),
            Object(NonMatching, "Runtime.PPCEABI.H/__init_cpp_exceptions.cpp"),
        ],
    },
]


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
    # dtk's gap units below the first SDK unit are uncharted game code: count them in "game"
    from tools.match.autocat import categorise_auto_units
    categorise_auto_units("objdiff.json", f"config/{config.version}/splits.txt")
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
