# Tiger Woods PGA Tour 06 PS2 dashboard symbols

The July 12, 2005 PS2 package contains:

- `SLUS_212.64`: stripped main-game ELF.
- `MAPFILE.TXT`: the game linker map already used by the project.
- `EA_DASH.ELF`: unstripped MIPS ELF with `.mdebug` and `.symtab`.

CCC 2.1 identifies STABS records inside `EA_DASH.ELF`'s `.mdebug` section. `stdump` produced:

- `source-files.txt`: 619 source/header entries.
- `types.hpp`: 822 top-level type declarations.
- `functions.cpp`: 7,128 address-bearing function records, including signatures and recoverable
  parameters/locals.
- `globals.cpp`: recoverable global declarations.

The source tree is predominantly EA's Apt/dashboard, EAGL, RealMemCard, SharedLogin, sound, and
other shared packages. Do not treat an `EA_DASH.ELF` address as a game address. Its value is naming
and understanding middleware that may overlap TW2004.

Generated with [CCC 2.1](https://github.com/chaoticgd/ccc):

```sh
stdump identify EA_DASH.ELF
stdump files EA_DASH.ELF -o source-files.txt
stdump types EA_DASH.ELF -o types.hpp
stdump functions EA_DASH.ELF -o functions.cpp
stdump globals EA_DASH.ELF -o globals.cpp
```

CCC warned that it discarded several likely-incorrect Apt function addresses. The declarations
remain useful, but those affected addresses require independent checking.
