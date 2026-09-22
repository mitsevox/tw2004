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
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-common on",         # EA game code: uninitialised globals are COMMON (UStream.c addresses each array separately)
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
        "lib": "mtx",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(NonMatching, "dolphin/mtx/mtx.c"),
            Object(NonMatching, "dolphin/mtx/mtx44.c"),
        ],
    },
    {
        "lib": "ar",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(NonMatching, "dolphin/ar/arq.c"),
        ],
    },
    {
        "lib": "ai",
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_sdk,
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
            Object(NonMatching, "dolphin/ai.c"),
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
            Object(NonMatching, "dolphin/card/CARDRead.c"),
            Object(Matching, "dolphin/card/CARDWrite.c"),
            Object(Matching, "dolphin/card/CARDMount.c"),
            Object(Matching, "dolphin/card/CARDUnlock.c"),
            Object(Matching, "dolphin/card/CARDCheck.c"),
            Object(Matching, "dolphin/card/CARDCreate.c"),
            Object(Matching, "dolphin/card/CARDDelete.c"),
            Object(Matching, "dolphin/card/CARDFormat.c"),
            Object(Matching, "dolphin/card/CARDRdwr.c"),
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
            Object(NonMatching, "dolphin/dvd/dvdfs.c"),
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
            Object(NonMatching, "dolphin/gx/GXAttr.c"),
            Object(NonMatching, "dolphin/gx/GXBump.c"),
            Object(NonMatching, "dolphin/gx/GXLight.c"),
            Object(NonMatching, "dolphin/gx/GXTexture.c"),
            Object(Matching, "dolphin/gx/GXInit.c"),
            Object(Matching, "dolphin/gx/GXTev.c"),
            Object(Matching, "dolphin/gx/GXStubs.c"),
            Object(Matching, "dolphin/gx/GXFrameBuf.c"),
            Object(Matching, "dolphin/gx/GXPerf.c"),
            Object(Matching, "dolphin/gx/GXDisplayList.c"),
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
            Object(NonMatching, "dolphin/os/OSAlarm.c"),
            Object(NonMatching, "dolphin/os/OSArena.c"),
            Object(NonMatching, "dolphin/os/OSCache.c"),
            Object(NonMatching, "dolphin/os/OSMemory.c"),
            Object(NonMatching, "dolphin/os/OSReset.c"),
            Object(NonMatching, "dolphin/os/OSRtc.c"),
            Object(NonMatching, "dolphin/os/OSTime.c"),
            Object(NonMatching, "dolphin/PPCArch.c"),
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
            Object(NonMatching, "runtime/runtime.c"),
            Object(NonMatching, "runtime/ansi_files.c"),
            Object(NonMatching, "runtime/direct_io.c"),
            Object(NonMatching, "runtime/file_io.c"),
            Object(NonMatching, "runtime/FILE_POS.c"),
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
            Object(Matching, "runtime/__mem.c"),
            Object(Matching, "runtime/__va_arg.c"),
            Object(Matching, "runtime/buffer_io.c"),
            Object(Matching, "runtime/mem.c"),
            Object(Matching, "runtime/misc_io.c"),
            Object(Matching, "runtime/s_copysign.c"),
            Object(Matching, "runtime/s_modf.c"),
            Object(Matching, "runtime/sscanf.c"),
            Object(Matching, "runtime/wchar_io.c"),
        ],
    },
    {
        # EA game code whose original source file is not yet known.
        "lib": "unsorted",
        "mw_version": "GC/2.5",
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": [
            Object(Matching, "unsorted/code_80007BC4.c"),
            Object(Matching, "unsorted/code_800080D0.c"),
            Object(Matching, "unsorted/code_800082F8.c"),
            Object(NonMatching, "UStream.c"),
            Object(NonMatching, "Golfer.c"),
            Object(NonMatching, "Ball.c"),
            Object(NonMatching, "Swing.c"),
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
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
