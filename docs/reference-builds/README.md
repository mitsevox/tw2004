# Related-build debug references

Generated inventories from later Tiger Woods builds. These are research evidence, not proof that
TW2004 used the same name, type, layout, or implementation. Promote a finding into the decomp only
after it agrees with the GameCube binary; record accepted mappings in `../tw06-names.md`.

No game executable, PDB, SELF, XBE, ELF, disc image, or linker map is stored here. The files in this
directory are compact symbol/type/source inventories generated from separately held reference
artifacts.

## Contents

- `tw06-ps2/`: CCC `stdump` exports from `EA_DASH.ELF`. This is EA dashboard and middleware code,
  not the stripped main game executable.
- `tw06-xbox/`: source/module/type indexes and selected type reconstructions from `default.pdb`.
- `tw07-ps3/`: source, function, section, type, and DWARF-quality inventories from the reconstructed
  `Dev_Golf_Release_PS3.elf`.

## Input fingerprints

| Artifact | SHA-256 |
|---|---|
| TW06 PS2 archive | `6b07229c695360db8d0c4201e0cdcef668147118a10bb1f725771452ef21527a` |
| `EA_DASH.ELF` | `176b2dee8c8bc2f7cd35f9c668936de659bbbca5b44ba2c1fe3abae06705e0c0` |
| `MAPFILE.TXT` | `01ac3c3ed7eeba32530b3db4feaf3d0c9a8bc3cd2d5e860872f7d4257a0c1928` |
| `SLUS_212.64` | `ab3598e25e3fd0cba6b8ecc726e9f81ba095af35f8954864b791516071347d74` |
| TW06 Xbox archive | `dd7909e978d2dcda35e5f6f19d3033bf0e0a7df78856dac96f04bf616eed60db` |
| `default.pdb` | `ffb8f40a0a9f9f1dc41012451dd53caf2c1c79fda065b855af595f2599af17f4` |
| `default.exe` | `461787eee8612e774f4a5faec9a0960a894a95dc9dcea733fa4321813573b1a2` |
| `default.xbe` | `d604ad7afbf6404fba760968969bdf942ca007e52c3874397f1a60262e714f01` |
| TW07 PS3 archive | `cae74b0b527bdb2ba6153b7bedd86a050ed962001ade8e05fccc39fa65e04041` |
| `Dev_Golf_Release_PS3.self` | `d1aa0d63870428bb44ad45deffb87e11c5361e0fd852803dade421d17b1bd08f` |
| reconstructed `Dev_Golf_Release_PS3.elf` | `c6e219c312fe14957abdcdc3d8141033bbc689925d8d5487d48020063ec584ab` |
