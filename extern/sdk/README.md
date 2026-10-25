Dolphin SDK and Metrowerks runtime source, taken from the Metroid Prime decompilation
(https://github.com/PrimeDecomp/prime, commit ca286f453d30c201304a82939fa8929181cc9d30).
Both games link the same Sep 5 2002 SDK build, so most of this matches TW2004 byte for byte.
Files are only wired into the build (configure.py) once they have been verified to match here.

Changed for this game:
- `runtime/uart_console_io.c`: this game's `__write_console` skips the UART on a TDEV console
  (`OSGetConsoleType() & OS_CONSOLE_TDEV`) and always passes the text on to the debugger;
  `libc/uart.h` (new) declares the TRK serial routines it calls.
- `dolphin/os/OS.c`: `__OSVersion` is global and `ClearArena` static, as in this game's symbols.
- `runtime/printf.c`, `runtime/FILE_POS.c`: `printf`, `vprintf`, `ftell` and `fseek` take the stdio
  lock (critical region 2) around their work; `libc/critical_regions.h` (new) declares it.
  `printf.c` is built with GC/2.5 (see configure.py), and so is `runtime/qsort.c` (unchanged).
- `runtime/critical_regions.gamecube.c` (new, ours): MSL's three empty critical-region functions,
  written in reverse of their link order, as `-inline deferred` emits them.
- `dolphin/amcstubs/AmcExi2Stubs.c`, `dolphin/odenotstub/odenotstub.c` (new, ours): the retail
  SDK's AMC debugger stubs (EXI2_*, AMC_IsStub) and the weak Hu_IsStub, each its own library. (`dolphin/db/odenotstub.c`, from extern/gauntlet, is
  in fact the OdemuExi2 debugger driver; this one, with only Hu_IsStub, is the stub library.)
- `runtime/extras.c` (new, ours): `stricmp`, the only function of MSL's extras the linker kept.
- `dolphin/gx/GXMisc.c`: `GXResetWriteGatherPipe` (ours), which Prime's copy lacks.
- `dolphin/card/CARDRead.c`: `CARDRead` (ours), the synchronous wrapper Prime's copy lacks.
- `dolphin/gx/GXFifo.c`: `GXSaveCPUFifo`, `__GXSaveCPUFifoAux` (global: `GXEndDisplayList` calls
  it) and `GXGetFifoStatus` (ours), which Prime's copy lacks.
