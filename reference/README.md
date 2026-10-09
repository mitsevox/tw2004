# Reference material from related EA builds

Symbol, type and source-file inventories extracted from other EA games that shipped with debug
information, used as evidence for EA's names, types and file layout. A finding here is a lead, not
proof: it goes into the decomp only once the GameCube code agrees with it, and every name taken
from here is logged in `config/GW4E69/name_sources.tsv`.

No executable, PDB, ELF, disc image or linker map is stored here, only the inventories made from
them.

## The builds

| Folder | Build | What it gives |
|---|---|---|
| `tw07-ps3/` | Tiger Woods PGA Tour 07, PS3 debug build (full DWARF) | Function order, signatures and locals per source file (`cu/`), and a machine pairing with our functions (`pairs.tsv`). The richest source of EA names |
| `tw06-xbox/` | Tiger Woods PGA Tour 06, Xbox beta PDB (2005-07-12) | Source files, modules and type records. Same studio and engine; partly C++ by then |
| `tw06-ps2/` | Tiger Woods PGA Tour 06, PS2 (2005-07-12) | The `EA_DASH.ELF` dashboard's symbols: EA's shared save and file libraries |
| `madden2003-ps2/` | Madden NFL 2003, PS2 prototype (EA Tiburon) | The UI Studio library our `UIS*` units are: signatures, locals, types, and the script opcodes |
| `nascar2005-gc/` | NASCAR 2005, GameCube (EA Tiburon, DWARF) | Its compilation units; it turned out to have no UI Studio |
| `007eon-ps2/` | 007 Everything or Nothing, PS2 (unstripped) | The few SDK and runtime functions it shares with this game |
| `tw2003-gc/`, `tw2005-gc/` | Tiger Woods 2003 and 2005, GameCube retail | File names, strings and function address pairs with this game |

The TW06 and TW07 names are two and three years newer than this game, and parts of those builds
were rewritten in C++ (the game modes, the Apt front end). A name from them says "this function
does the same job", which the code has to confirm.

## EA's source tree

The Tiger Woods PGA Tour 2003 Xbox release builds (September 2002) carry no symbols, but their
executable still holds 64 source paths, which show how EA laid out the code under
`C:\Dev\TigerCode\Code\`. The files this game names in its own asserts sit here:

| Folder | Files |
|---|---|
| `Golf\AI\` | `Swing.c` |
| `Golf\Animation\` | `Skeleton.c`, `Skin.c`, `char.c`, `char_skin.c`, `MTA.c`, `mtalib.c`, `skalib.c` |
| `Golf\Audio\Engine\Utils\` | `UAudMemStack.c` |
| `Golf\Cameras\` | `GoDynamicCam.c`, `GoComicCam.c`, `GoStaticCam.c` |
| `Golf\Entry\` | `GoEntry.c`, `startUp.c` |
| `Golf\FrontEnd\` | `FE_Manager.c` |
| `Golf\GameMode\` | `CareerMode.c`, `CourseInfo.c`, `Earnings.c`, `PlayNowMode.c`, `TournamentMode.c` |
| `Golf\Hi-Rendering\` | `GoCamera.c`, `GoFrameBuf.c`, `GoViewport.c`, `GoDynObj.c`, `GoLighting.c`, `GoTerrain.c` (and `Xbox\GoRenderCtx_Xbox.c`) |
| `Golf\Lo-Rendering\Shader\` | `GoShaderObject_{Glows,Particle,Rain}_Xbox.c`, `GoShaderObjectContainer_OBFData_Xbox.c` |
| `Golf\Memory Card\` | `MC.c` (and `Xbox\MC_Xbox.c`) |
| `Golf\RCMP\` | `rcmp_mad_codec.c` |
| `Golf\SFX\` | `PsBallFx.c`, `PsMgr.c`, `UFstPart.c`, `shadow.c` |
| `Golf\SitDev\` | `SitDev.c` |
| `Golf\UI runtime\` | `FETextureMgr.c`, `FEgolferanim.c`, `uiEATrax.c`, `uiLoadFile.c`, `uiProcessInterface.c`, `uiTransform.c` |
| `Legacy\LL\` | `LLFont.c`, `LLPictInt.c`, `LLTex.c`, `LLTexGrp.c`, `LLVideo.c` (and the `Xbox\` platform files) |
| `Legacy\Lib\` | `UMath.c`, `UDynMemPool.c`, `UHeap.c`, `UMemPool.c`, `UObject.c`, `UObject3D.c` |
| `Legacy\SPECIF\` | `UFont.c`, `UKernel.c`, `UStream.c` |

Platform files end in `_Xbox` there; this game's asserts name the `_Gc` versions (`LLObj_Gc.c`,
`MC_Gc.c`, `GoRenderCtx_Gc.c`...).

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
| Madden NFL 2003 PS2 prototype `SLUS_205.29` | `8af61dbc3bfa6aa3cedda7fff695d524ac91af955777c2039d26db99d6c5fb29` |
