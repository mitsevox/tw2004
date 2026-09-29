# The map of the game's source

What each part of Tiger Woods PGA Tour 2004 (GameCube) does, and which files in `src/` make it.
Every file's first comment says what it is and where its name comes from; this page only groups
them. Claims here come from those header comments (audited 2026-09-24) or from reading the code.
Anything not proven is marked *(unverified)*. `docs/gameplay.md` was never audited: check the code
before trusting a claim from it.

Not EA game code, and not mapped here: the Nintendo SDK, the MSL C library, the Metrowerks runtime
and MetroTRK are built from public decomps in `extern/` (the `progress_category="sdk"` libraries in
`configure.py`); `src/dolphin/gba/` is the SDK's GBA library.

## Start here

| To see | Read |
|---|---|
| The entry point | `main()` in `unsorted/sweep_800055D8.c`: boot list (`sweep_80005520.c`), `GoEntry.c`'s loop, shutdown list (`sweep_80005590.c`) |
| The game's state machine | `GoEntry.c` `fn_800083A4`: a switch on `gSession.nGameType` (table below) |
| One frame | `gomainloop.c`: `fn_8006D8E8` is the frame loop, `fn_8006DBD4` picks the game type's frame, `fn_8006D27C` draws a round's frame, `fn_8006DDA8` runs every module's hooks |
| A shot, start to finish | `gomainloop.c` calls `fn_8005D2F8` (`Code8005D2E4.c`) each frame, which runs `GOLFERSTATE_Update` (`StateGolfer.c`): each player's golfer state (`stateFunc.c`, `STATEFUNC_*`). `STATEFUNC_Swing*` drives `Swing.c`, which launches the ball with `Physics_ShotImpact` (`Ball.c`); `GM_BallHit` and `GM_Update` (`GameMode.c`) keep the round's turns and strokes |
| A round's rules | `GameRound.c` `GM_SetModeType` fills `gpGame`'s callbacks (`GameState`, `include/golfer.h`) with defaults, then calls the game mode's own setup (`GameMode*.c`) |
| The menus | `uiProcessInterface.c` runs the UI; the screens run on EA's UI Studio library (`UISApi.c` is the game's side, `UISStack.c` `UISStackProcess` the script interpreter); menu messages land in `FE_MessageTable.c`, a round's in `GameUICommands.c` |
| Loading anything from disc | `UStream.c` (the chunked `.hog`/`.gcb` streamer; `docs/formats/ctrl-container.md`), handlers registered per chunk type, file lists in `streammanagerhole.c` |
| Shared headers | `include/engine.h` (services), `include/game.h` (round, modes), `include/golfer.h` (players, golfers, `GameState`), `include/character.h`, `include/camera.h` |

`gSession.nGameType` (the outer loop in `GoEntry.c`): 0 boot decision, 1 start-up screens,
10 start the front end, 3 front end, 4 start a round (`GO_vInitIG`), 5 load a hole (`Code8006F438.c`),
6 play (the frame loop), 7 after the frame loop ends (back to 5 when another hole load is asked
for, else 8), 8 shut the round down, 12 reset the console, 13 leave, 9 an error. Each type's
step is a call into `gomainloop.c` (`GO_vInitFE`, `GO_vInitIG`, `fn_8006D8E8`...).
The game *mode* (stroke play, skins...) is a different number, `gpGame->nMode` (`Game_GetMode()`);
see [Game modes](#game-modes).

## Glossary

| Term | Meaning |
|---|---|
| `LL` | EA's low-level layer: the GameCube side of display, files, textures, fonts, pictures, video (`LLDisp_Gc.c`, `LLFileIO_Gc.c`...). `_Gc` marks a GameCube-specific file (EA's own suffix) |
| `U` | EA's utility layer (`UStream`, `UMemPool`, `UKernel`, `UObject`, `UFont`...), in EA's 2002 source tree too. What the letter stands for is *(unverified)* |
| `Go` | EA's prefix on the golf engine's files (`GoEntry`, `gomainloop`'s `GO_vInitIG`, `GoTerrain`, `GoCamera`...; TW06 keeps them under `golf/`). The meaning of the letters is *(unverified)* |
| `GoShaderObject*` | the "shader objects": course geometry drawn by type through a table of hooks (`lbl_80188E88`, engine.h `ModuleHooks`), one row per kind (prelit/UV-animated, particles, glows, rain, grass...) |
| `hws` | EA's prefix on the skin-description files (`hwsBurn`, `hwsRender_Gc`...); meaning *(unverified)* |
| `ska` / `SKA` | the skeletal animation code (`skalib.c`, `ska_shared.c`, `SKABlendNode`) |
| CrAP | Create-A-Player, the golfer editor (`FE_CrAP*`, `fe_craputils.c`) |
| SitDev | the situation scripts: commentary, sounds and music triggered by what happens in a shot (`SitDev*.c`) |
| UIS / UI Studio | EA Tiburon's IStudio menu library (`UIS*.c`, `UIStudio.c`) |
| EASB | EA Sports Bio, EA's cross-game player profile on the memory card |
| `fn_8XXXXXXX`, `lbl_8XXXXXXX` | a function or global not named yet, by its address |
| Name tiers | in `config/GW4E69/name_sources.tsv`: T1/T2 EA's own name (its text in the binary, or TW06/TW07/Madden 2003 confirmed by the code); T3 named from a careful reading of the code |
| `n290`, `pfn3C`, `u04`, `b0F` | struct fields not named yet: a type letter and the field's offset |
| `// fake match:` | C written a non-natural way only to reproduce EA's bytes; logic unchanged |
| `// EA bug:` / `// port:` | a bug in EA's code (kept); a hazard for a later PC port (32-bit pointers, endianness) |
| section / data-order notes | why globals are defined in reverse order etc. (CodeWarrior lays out `.bss`/`.sbss` last-defined-first) |
| Generated units | `AXComp.c` and `DSPCode.c` (SDK data) and the game's asset fragments are generated from your own `main.dol` at build time (`tools/build/gendata.py`), never committed (`docs/workflow.md` "Units generated from main.dol") |
| sweep / fold | `unsorted/sweep_*.c` are small functions found by `sweep.py` whose original file is unknown; `fold.py` later merges sweep files into a unit |

In the file tables, **Name** says where the file name comes from (the file's header comment has the
details): **EA** EA's own name in this binary (an assert or file string); **T6** / **T7** EA's file
name in Tiger Woods 2006 / 2007, whose functions match these; **T03** / **T05** a source path in
Tiger Woods 2003 / 2005 (EA's file string there); **M03** Madden NFL 2003's debug
symbols; **ours** a name we chose for what the file does; **ph** a placeholder named after its
address (`CodeXXXXXXXX.c`, `unsorted/*`), its original file unknown.
Files are listed in link order (address order) within each group.

## Subsystems

- [Boot and game flow](#boot-and-game-flow)
- [Core services: memory, math, files, streaming, input](#core-services)
- [Rendering](#rendering)
- [Shader objects and effects](#shader-objects-and-effects)
- [Cameras](#cameras)
- [The course: terrain, objects, collision](#the-course)
- [Ball physics](#ball-physics)
- [Golfers: players, AI, swing, golfer states](#golfers)
- [Characters: models, skeletons, animation](#characters)
- [The round](#the-round)
- [Game modes](#game-modes)
- [Career: PGA TOUR season, calendar, ladder, money](#career)
- [Situation scripts and commentary](#situation-scripts-and-commentary)
- [Front end and UI](#front-end-and-ui)
- [EA UI Studio library](#ea-ui-studio-library)
- [Movies](#movies)
- [Audio](#audio)
- [Save data, memory card, EA Sports Bio, GBA link](#save-data)
- [Disc](#disc)
- [Unplaced](#unplaced)

### Boot and game flow

`main` initialises the systems, runs the outer loop in `GoEntry.c` and shuts everything down.
The outer loop moves between start-up screens, the front end and rounds (`gSession.nGameType`);
`gomainloop.c` starts and stops every system for each of those and runs the frame loop.

| File | Name | What it is |
|---|---|---|
| unsorted/sweep_80005520.c | ph | the boot-time init list `main` calls first (memory, ARAM, streamer buffers, session...) |
| unsorted/sweep_80005580.c | ph | one boot-time setter (clears `*lbl_80281510`) |
| unsorted/sweep_80005590.c | ph | the shutdown list `main` calls last |
| unsorted/sweep_800055D4.c | ph | an empty function the shutdown list calls |
| unsorted/sweep_800055D8.c | ph | `main()` |
| GoEntry.c | EA | the outer loop over `gSession.nGameType` |
| Code8002EE1C.c | ph | the session (`Session_Init`, players, golfers, options) |
| gomainloop.c | T6 | the main loop: every system's start-up and shut-down, the frame update, the render passes |
| Code8006F438.c | ph | the hole loader: streams a hole's files in and sets the systems up for it |
| BootCourse.c | ours | the demo rounds (`DEMO_Start`: four fixed set-ups) and a 120-second idle timer `GoEntry.c` tests |
| ScreenClear.c | ours | a black full-screen quad for 1, 2 or 30 frames |

### Core services

Memory, math, random numbers, the file and stream loading, the controllers and the clocks: the
services the rest of the game calls (most are declared in `include/engine.h`). Most of it sits
early in link order (0x80005628-0x80013720), between the LL rendering files.

| File | Name | What it is |
|---|---|---|
| LLMem_Gc.c | ours (after TW06 `llmem_xbox.c`) | memory copy, fill and compare |
| LLFileIO_Gc.c | EA | disc file reads, from a queue run by a reader thread |
| Quaternion.c | ours | quaternion rotations and float helpers (maybe the tail of GoEntry.c) |
| StaticMemory.c | ours | the game's own heap (one ~20 MB block, sorted free spans) |
| UMemPool.c | EA | pools of fixed-size nodes; 4x4 matrix and paired-single math, atan2f, logarithm |
| urandom.c | T6 | three random-number streams (lagged Fibonacci), uniform and normal floats |
| ObjList.c | ours | the list of loaded stream objects, found by type and id |
| LoadData.c | ours | stream handlers: `'txf2'` texture banks, the `'load'` object, the loading-screen file; `Game_GetMode` |
| UStream.c | EA | the streaming asset loader (CTRL/SHOC/SONO chunk files `.hog`/`.gcb`), async DVD reads into 18 buffers |
| streammanagerhole.c | T6 | the stream file lists (front end, start-up, in game, characters, hole) and their handler lists |
| Controller_Gc.c | ours | the four controllers: buttons, sticks, triggers, rumble |
| ByteSwap.c | ours | copies little-endian disc data while swapping byte order |
| LLTime.c | T7 | the clock and its stopwatch counters (`TI_*`) the main loop times frames with |
| VecMath.c | ours (TW06: `vec4flt_*`) | paired-single vector and matrix helpers used everywhere |
| llrtclock.c | T6 | the real-time clock as a date and as text |
| GoARAM.c | EA | audio RAM: set-up, heap and the DMA queue (front end, memory card and animation park data there) |

### Rendering

Setting up the display, render cameras and viewports, textures and fonts, and drawing the course,
lights, shadows and screen effects through the GameCube's GX. Characters' skins are drawn by the
files in [Characters](#characters).

| File | Name | What it is |
|---|---|---|
| LLDisp_Gc.c | EA | the display: video mode, image buffer, CPU/GPU sync, copying each frame out |
| LLObj_Gc.c | EA | a model's mesh tree from its stream data; bounding-sphere visibility tests |
| unsorted/code_800080D0.c | ph | vertex format set-up and the per-type function-table dispatch |
| unsorted/code_800082F8.c | ph | camera and drawable-object accessors (`Vec3Copy`) |
| LLTex.c | EA | texture banks from `'txf '` objects; byte-code scripts that drive texture items |
| LLTexGrp.c | EA | the list of loaded texture banks; find a texture by name hash |
| LLFont.c | EA | the font renderer (`'sfn '` fonts) |
| UFont.c | EA | the text layer over LLFont.c: six font slots, queued or immediate text |
| Code80012ED0.c | ph | render helpers: a render camera's screen rectangle, renderer state setters |
| unsorted/sweep_800136F4.c | ph | two tiny functions next to GoRenderCtx_Gc.c *(unverified which file)* |
| GoRenderCtx_Gc.c | EA | the render camera (lens, frame buffer, screen rectangle, matrices) and screen state |
| Code80015470.c | ph | the renderer's state cache and the buffer pool |
| Code80016198.c | ph | the 2D view: viewport, screen size, vertex output |
| GxUtil.c | ours | a 256x224 copy of the screen as a texture, rectangle alpha clear, texture object set-up |
| GoRenderSurface.c | EA | five off-screen render surfaces |
| GoTerrain.c | EA | the terrain renderer: the hole's ground and objects in draw lists by distance and LOD; animated trees, crowd, flag |
| GoPostFx.c | EA | screen effects over each view (a colour, two effects from a screen copy) |
| DepthField.c | T6 | depth-of-field blur |
| UObject3D.c | EA | a model from a stream object; a recorded display list of renderer state |
| GoFrameBuf.c | EA | a frame buffer's size and scale |
| GoLighting.c | EA | the scene's lights: a pool of 25, loaded into GX (four point lights and an ambient) |
| Code8006F154.c | ph | the terrain colour blended by the camera's heading |
| GoCamera.c | EA | a render camera's lens: field of view, position, target, matrices |
| GoViewport.c | EA | a viewport (13 floats) |
| GoLightFogEnv.c | T7 | the hole's lighting environments (`LF_*`) |
| goballfx.c | T6 | four sets of five lights, and the ball's ground marker |
| GoObjShadow.c | ours | object shadows drawn with the "shadow" texture (called from GoDynObj.c) |
| SunFlr_Gc.c | EA | the sun flare's GameCube part: depth around the sun, how much is visible |
| unsorted/sweep_8009A844.c | ph | a screen-rectangle point to frame-buffer units (next to the sun flare) |
| Code8009A928.c | ph | the glows' frame hooks for gomainloop.c |
| Code8009AA28.c | ph | the sun flare's set-up and per-view update; the glow queue's add |
| unsorted/sweep_8009B314.c | ph | a sun-flare state setter and a node free *(unverified which file)* |
| Code8009B340.c | ph | a list of fading glow nodes |
| GoGreenGrid.c | EA | the grid drawn over the putting green |
| shadow.c | EA | the golfer's shadow, drawn into a 256x256 texture with its own camera |
| Code800BA940.c | ph | the glows around each view's ball |
| GoBreakLine.c | EA | the putt's break line on the green |
| LLDynTex.c | EA | textures rewritten while shown (golfer models, user logos), streamed in from the character file |
| GoGrass.c | EA | the grass: its stream handler, parameters, a 256x256 grass texture drawn with its own camera |

### Shader objects and effects

Course geometry is drawn as "shader objects", a table of hook rows by kind (`lbl_80188E88`,
engine.h `ModuleHooks`). Particles, rain and the ball's effects sit here too.

| File | Name | What it is |
|---|---|---|
| Code8006F608.c | ph | the hole's weather: rolls per-course choices and starts/stops effects, the rain among them (PsMgr.c) |
| GoShaderObjectContainer_OBFData_Gc.c | EA | copies a shader object's chunks into GPU-ready buffers |
| unsorted/sweep_8006FF2C.c | ph | one of row 0's hooks (sets a vertex array, then calls sweep_80070168.c) |
| ShaderRow0.c | ours | row 0's hooks on a dynamic rendering buffer |
| unsorted/sweep_80070168.c | ph | calls a display list, then `fn_800124CC` (LLFont.c) |
| DynamicRenderingBuffer.c | ours | vertices a shader object rewrites each frame (EA's header name `GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h`) |
| Code80070EC4.c | ph | rows 4 and 2's static hooks (row 2 plays a morph animation) |
| GoShaderObject_PrelitUVAnimation_Gc.c | EA | row 5: prelit geometry with scrolling textures; init/close of every shader type |
| GoShaderObjectCommon_ShaderObjectsData_Gc.c | EA | builds the display lists that draw shader objects |
| GoShaderObjectCommon_TexAnimManager_Gc.c | EA | scrolling-texture matrices |
| GoShaderObject_Particle_Gc.c | EA | the particle shader; also the main-memory heap made from the OS arena |
| unsorted/sweep_8009554C.c | ph | six empty functions (GoEntry.c, gomainloop.c and char.c call them) |
| GoShaderObjectCommon_MorphAnimManager_Gc.c | EA | the list of morph animations |
| unsorted/sweep_800977CC.c | ph | frees a buffer at +0xC0 *(unverified which file)* |
| unsorted/sweep_80097E98.c | ph | frees `lbl_80281F78`; emits two vertices *(unverified which file; sits before the glows)* |
| GoShaderObject_Glows_Gc.c | EA | the glows' shapes and drawing |
| UFstPart.c | EA | particle emitters |
| PsMgr.c | EA | weather particle effects (only the rain on GameCube) |
| PsBallFx.c | EA | the ball's particle effects and sand trail |
| GoShaderObject_Rain_Gc.c | EA | rain streaks and splashes |
| GoShaderObject_Grass_Gc.c | ours (after TW06) | the grass shader object: grass shells over the terrain |
| ShaderRow19.c | ours | row 19's hooks (row 0's code, another primitive kind) |

### Cameras

Each view on screen has a camera controller. The golf cameras pick a mode for each moment of a
shot, and camera scripts move the camera between the shots and sequences the dynamic-camera files
describe; static cameras and fly-bys come from the course data.

| File | Name | What it is |
|---|---|---|
| ViewController.c | ours (after TW06 `viewControllerID`) | the four views: render camera and camera controller each |
| GoDynamicCam.c | EA | camera shots, sequences and choices from the camera files, picked by situation |
| gocamscripts.c | T6 | camera scripts: move from shot to shot, keep above ground, aim at ball and pin |
| GoCamCont.c | T6 | each view's camera controller: camera mode, idle state |
| GoStaticCam.c | EA | the course's static cameras and fly-by paths (`'Cact'`, `'CAMC'` objects) |
| GoCamTuningVars.c | EA | the camera tuning values |
| GoComicCam.c | EA | the comic-book camera: the shot shown in panels |
| GoGolfCam.c | EA | the golf cameras: each mode's init and per-frame process |
| CamSpline.c | ours | Catmull-Rom spline paths for cameras |

GameEffects.c (the GameBreaker letterbox and slow motion) is in [The round](#the-round).

### The course

The hole's ground, its objects (trees, crowd, animals, course objects) and its collision data,
all arriving as stream chunks when a hole loads.

| File | Name | What it is |
|---|---|---|
| TerrainData.c | ours | the hole's networks (free-drop areas, out of bounds, situation zones) from `'Cnet'` objects; outline tests |
| GoDynObj.c | EA | the course's own objects around the dynamic objects: `'TEO '`/`'BALL'` handlers, per-player objects, drawing |
| UObject.c | EA | a drawable object: three matrices, a model with up to four LODs |
| UKernel.c | EA | the kernel's list of the course's dynamic objects |
| GoDynObjBase.c | ours | dynamic object type 0 and the types' table; type 2 turns at a steady speed |
| GoAnimalActors.c | T6 | the animals on the course |
| GoDynObjTypes.c | ours | dynamic object types 6 and 9 |
| GoTerrainCollision.c | T6 | the ground as collision data: drops, heights, normals, surfaces; the ball against ground, pin and objects |
| TerrainGround.c | ours | the triangle, height and surface type under a point |
| CourseData.c | ours | the course table (`'CRI '`: 18 holes' par, wind, tees) and built rounds (`'CMPS'`) |

Also here by subject: `GoTerrain.c` (drawing it), `GoGrass.c`, `Code8006F438.c` (the hole loader).

### Ball physics

The ball's flight, bounce and roll, in yards and seconds, against the surface table.

| File | Name | What it is |
|---|---|---|
| MaterialTypes.c | T6/T7 | data only: the surface table `gSurfaceTypes` |
| Ball.c | ours | flight, roll, collision, the cup, simulation (`Physics_*`, `Ball_*`) |
| Wind.c | T7 | the wind |
| startUp.c | EA | (also) the ball-against-object test Ball.c uses; see [Audio](#audio) |

### Golfers

The players and their golfers: the CPU's shot planning, the clubs, the human's swing, aiming and
ball placement, and the golfer state engine that steps each player through a shot
(`GOLFERSTATE_*`, `STATEFUNC_*`: pre-shot, set-up, swing, watching the ball, the green...).

| File | Name | What it is |
|---|---|---|
| ai_brain.c | T6 | the CPU's shot rehearsal (`AI_RehearseShot`, TW06's `AIBrain_Think`) |
| Code8002BBB0.c | ph | the AI's targets (`AI_Targets*`), aim and distance nudges |
| Code8002C984.c | ph | the club tables: which club suits a shot kind, longer/shorter club, aim angle |
| Golfer.c | ours | the golfers' luck odds (`Luck_*`, `Golfer_IsLucky`) |
| Code8002DB80.c | ph | the caddie's putt tip, the golfer table, players, bags and options |
| Swing.c | EA | the stick swing: phases, miss, power, attributes; club trail, boost display |
| StateGolfer.c | T7 | the golfer state engine: each player's state stack (`GOLFERSTATE_*`) |
| Code8005D2E4.c | ph | a one-slot state machine over `lbl_801883C0`; slot 1 runs `GOLFERSTATE_Update` |
| stateFunc.c | T7 | the golfer states' enter/update/exit callbacks (`STATEFUNC_*`, 27 states) |
| target.c | T6 | the aim marker, aim-point and ball-placement controls, where a ball may be dropped |
| emotion.c | T6 | the golfers' emotions after a shot, picking the reaction they play |
| Replay.c | EA | shot replay / take-back: saves player and conditions before a shot |
| CaddieTips.c | T7 | tips shown as a swing starts |

### Characters

The animated golfer on screen: building the character from its file, its skeleton, IK and bone
chains, skinned meshes and their parts and morphs, the animation libraries, blending and clip
choice, and the golfer's streamed textures and clothes.

| File | Name | What it is |
|---|---|---|
| char.c | EA | the character object: built from `'CHR '`, animation, bones, ground placement, leg IK, textures and clothes |
| char_tex_manager.c | T6 | `#include`d into char.c: puts the user logos on a golfer's model |
| mtalib.c | EA | animation helpers; `'MAL '` banks of items picked at random |
| ska_shared.c | T6 | decoding clip frames into bone rotations, blending; loading a clip from disc |
| skalib.c | EA | the animation library: SAL libraries and BNK clip banks in three slots, clip choice |
| Skeleton.c | EA | the skeleton: IK chains and weights, per-bone factors |
| Skin.c | EA | a skinned model: bone matrices, morph weights, drawing |
| Code80037AB8.c | ph | split off Skin.c: bone inverse matrices, morph weights from a pose, frees description bits |
| animblender.c | T6 *(name unproven)* | blend trees (`SKABlendNode`) and the animation player (`TSKATime`) |
| CharAnim.c | ours (TW06: `char_state.c`) | the golfer's animation state: clip-group blends, idle and emotion updates, queued state changes |
| Code80095564.c | ph | with a single view, frees every golfer's body skin |
| AnimStream.c | EA | clip streaming for groups 1 and 5 (turned off in every case); base-40 name codes |
| SkinPart.c | EA | a skin's parts (e.g. the glove), variants, options and sets |
| CharSliders.c | EA | body sliders: definitions, bone and morph-target blending |
| hwsBurn.c | EA | "burns" a skin description into one block |
| hwsMaterial_Gc.c | EA | a skin's materials and their textures |
| hwsOverride_Gc.c | EA | per-mesh overrides of a skin's meshes |
| hwsRender_Gc.c | EA | the GameCube skin renderer |
| DynChain.c | EA | bone chains that swing on their own (hair, cloth) |
| SkinMorph.c | EA | morph targets applied in a 16.16 fixed-point work area |
| SkinBurn.c | EA | "burns" a skin: drops unused parts, packs the rest |

### The round

A round's bookkeeping and flow: turns and strokes, hole set-up, the HUD's flow, messages to the
front end, statistics, the hole contests and the GameBreaker effects. From GameHoleContests.c on,
these files and the game modes sit together (0x800D9E14-0x80102AC8).

| File | Name | What it is |
|---|---|---|
| HoleScore.c | ours (TW06 `analysisutilities.c`, medium) | per-player round analysis: distances, lie, streaks by score |
| GameHoleContests.c | ours | longest drive, closest to the pin, hole-in-one contests |
| GameEffects.c | T6 | slow motion, the GameBreaker, heartbeat rumble, time rate |
| GameMode.c | T7 | turns, strokes, mulligans, post-shot reaction, walking to the ball (`GM_*`) |
| GameRound.c | ours | the round set-up: a game mode's callbacks, holes, stroke limit, mixed-course rounds |
| GameUI.c | ours | the in-round display flow: HUD messages, end-of-hole and end-of-round screens |
| GameMessages.c | ours | the game's messages to the front end (a message id plus values) |
| GameAnalysis.c | T6 | round statistics per player; a tip quoting one |

### Game modes

Each mode fills `gpGame`'s callbacks (`GameState`) from `GameRound.c` `GM_SetModeType`. Modes by
number (`gpGame->nMode`); several reuse another mode's callbacks.

| Mode | File | Name | What it is |
|---|---|---|---|
| 0 | GameModeStroke.c | T6 | stroke play (modes 9, 12..17, 22, 23 reuse some of its callbacks) |
| 1 | GameModeMatch.c | T6 | match play with sudden-death playoff |
| 2 | GameMode_Skins.c | T6/T7 | skins |
| 4 | LadderedMode.c | T05 | the 25-event ladder's matches |
| 5 | PlayNowMode.c | T03/T05 | the Play Now challenges: 83 (`'PLY '`); also run by mode 24 |
| 6, 7 | GameMode6.c, GameMode7.c | ours | two-player modes on GameMode8.c's code |
| 8 | GameMode8.c | ours | speed golf (time plus 3 per stroke) |
| 9 | GameMode_Practice.c | T7 | practice: chosen holes, the ball placed by hand before every shot |
| 10 | GameModeReplay.c | T6 | replaying a saved shot; the target games' target list |
| 11 | GameMode11.c | ours | the lessons |
| 12 | GameMode12.c | ours | stroke play with points for special surfaces |
| 13..17 | GameMode_SkillZoneBase.c | T7 | the code the target games share |
| 13 | GameMode_SkillZoneTimed.c | T7 | the timed target game |
| 14 | GameMode_SkillZoneCapture.c | T7 | a two-player target game (claim 5 targets) |
| 15 | GameMode_SkillZoneHorse.c | T7 | HORSE on the targets |
| 16 | GameMode_SkillZoneTarget.c | T7 | 20 balls at the targets in any order |
| 17 | GameMode_SkillZoneTargetToTarget.c | T7 | the targets in order with 5 balls |
| 18 | GameModeStableford.c | T6 | modified Stableford |
| 19 | GameModeBestBall.c | T6 | two-against-two best ball stroke play |
| 20 | GameModeFourBall.c | T6 | two-against-two best ball match play |
| 21 | GameModeAlternateShot.c | T6 | two-against-two alternate shot |
| 22 | GameMode22.c | ours | long-drive contest; also the trophy case's text |
| 23 | GameModeDriverPGATour.c | T6 | the PGA TOUR season (see [Career](#career)) |
| 24 | GameModeDriverRTE.c | T6 | real-time events on the calendar (see [Career](#career)) |
| 25 | GameModeBattle.c | T6 | two-player match play; the hole's winner takes a club |
| 26 | GameMode26.c | ours | two-player long-drive contest |

Link order in the block: AlternateShot, Battle, BestBall, FourBall, Match, 5, 9, DriverPGATour,
DriverRTE, Replay, GameTargets, 14, 15, 16, 17, 13, 2, 6, 7, 8, Stableford, 12, Stroke, 11, 4;
GameMode26.c and GameMode22.c sit later.

### Career

The long-term modes and their menus: the PGA TOUR season and its simulated field, the career
calendar with its real-time events, the ladder map, and the money and goals in the save profile.

| File | Name | What it is |
|---|---|---|
| Calendar.c | ours | dates for the tour season: day numbers, weekdays, today's date, date strings |
| Earnings.c | EA (TW2003 source tree) | money and goals: prize table (`'ERN '`), payouts and multipliers, unlock goals, saved replays |
| CalendarScreen.c | ours | the career calendar screen's callbacks |
| FE_Calendar.c | T7 | the calendar's per-mode driver tables and the month grid |
| PGATourSimulation.c | T6 | the tour field, entrant scores, season statistics and rankings |
| fe_stats.c | T6 | the PGA TOUR statistics screen |
| FE_CalendarPopups.c | T7 | the panel of a calendar day's event details |
| GameMode4Menu.c | ours | the ladder map screen's messages |
| LadderMap.c | ours | the ladder map's rules: regions, nodes, cursor moves |

Also: GameModeDriverPGATour.c, GameModeDriverRTE.c, LadderedMode.c (the ladder), PlayNowMode.c
(challenges) in [Game modes](#game-modes), and FE_PGATourMessages.c in the front end.

### Situation scripts and commentary

When something happens in a shot, `event.c` runs that event's handler; most queue the moment for
the situation scripts, which test the situation (lie, score, mode, zones) and fire commentary
lines, sounds and music. The SitDev files sit together at 0x800BB0A8-0x800BD894.

| File | Name | What it is |
|---|---|---|
| event.c | T6 | the game's event handlers (`EVENT_Trigger`) |
| SitDev.c | T7 | the commentary scripts' state block, their loaders, and the event queue they react to |
| SitDevMisc.c | T6/T7 | the ball watcher's state, value draws, mode bits *(file has no header description)* |
| SitDevFile.c | EA | the ball watcher, loading the scripts, running their actions |
| SitDevCommentaryZones.c | T6/T7 | the hole's commentary zones and the zone test |
| SitDevStateVector.c | T6/T7 | the values the scripts test (golfer, shot, ball, hole) |
| SitDevTrigger.c | T6/T7 | running a script entry's actions (commentary, sounds, music) |
| Code80193188.c | ph | data only: a per-value flag table SitDevStateVector.c reads (TW07's `SitDevSharedTables.c` probably) |

### Front end and UI

The menus (front end) and the in-round screens. `uiProcessInterface.c` runs the loaded UI file;
screens send numbered messages that `FE_MessageTable.c` (front end) or `GameUICommands.c` (in a
round) answer. The menu screens themselves run on the [EA UI Studio library](#ea-ui-studio-library).

| File | Name | What it is |
|---|---|---|
| PasswordManager.c | T6 | the cheat codes and their unlocks; a new save profile's set-up |
| fe_craputils.c | T6 | the Create-A-Player data in a save profile, unlocks, name lists |
| FE_Manager.c | EA | the front end's manager: set-up, movies, the profile being edited, the created golfer |
| FE_MessageTable.c | ours | the menus' message table (770 slots) |
| GameUICommands.c | ours | the commands the UI sends during a round (214 slots) |
| FEgolferanim.c | EA | the golfer animated on the menu screens |
| uiLoadFile.c | EA | loads the menu UI's files |
| uiProcessInterface.c | EA | runs the UI: controllers, commands, fades, number formatting |
| Code80090940.c | ph | pictures of the movie entries in the UI file table |
| uiProcessPolygon.c | T7 | the UI's polygon element, the loading screen and its progress bar, the start-up movies |
| uiText.c | ours | a UI text element |
| uiTransform.c | EA | the UI's transform stack |
| uiObject.c | T7 | 3D objects the in-round UI draws: the power boost and spin display |
| uiEATrax.c | T03/T7 | the EA Trax music display (song names sliding in) |
| Code800B90F4.c | ph | MAD picture frame lists (for rcmp_mad_codec.c); the ball models and logo on the Create-A-Player golfer (`'TEO '`, `'BALF'`) |
| uiArc.c | ours | a UI arc or circle element |
| FE_CrAPDB.c | EA | the Create-A-Player database (`'CR_A'`, `'CR_S'`) |
| FE_CrAPMessages.c | ours | the Create-A-Player screens' message handlers |
| FE_PGATourMessages.c | EA | the PGA TOUR menus: leaderboard, schedule, wrap-up, wins |
| FE_LogoDesign.c | EA | the logo editor, and a logo laid out as a texture |

### EA UI Studio library

EA Tiburon's IStudio library (TW2005's paths: `Code/Tiburon/IStudio/`), the runtime the menu
screens are built on: loaded screens, an event stack, byte-code scripts, rate functions. Built
with one library-wide flag set (`UIS_CFLAGS` in `configure.py`). Header: `include/frontend/uistudio.h`.

| File | Name | What it is |
|---|---|---|
| UISEvent.c | EA | the event stack and the rate functions |
| UISStack.c | M03 | the script interpreter (`UISStackProcess`) |
| UIStudio.c | EA | the core: loading, activating, unloading screens; sending events |
| UISApi.c | ours | the game's calls into the studio: set-up, callbacks, per-frame run, screen changes |
| UISScreen.c | ours | drawing a screen's nodes, event handlers, text formatting |

### Movies

The movie player (intro, credits, golfer bios) and EA's MAD picture and movie format.

| File | Name | What it is |
|---|---|---|
| LLPict_Gc.c | EA | a MAD picture/movie frame into I8 textures; the decoder's frames |
| LLVideo.c | EA | the movie player: MPG2 chunks queued for the decoder |
| LLPictInt.c | EA | decodes a `"MADk"` picture file |
| rcmp_mad_codec.c | EA | the MAD decoder: block decoder, inverse DCT, frames |

Also: `hlaudmovie.c` (a movie's sound), `uiProcessPolygon.c`, `Code800B90F4.c` (frame lists).

### Audio

EA's sound engine: a table of playing sounds, emitters placed in the world, tracks that are
either sequenced or streamed from disc, voices on the GameCube's hardware, and the game's side
that drives it all. The block sits at 0x800A3E3C-0x800AF324 (uiObject.c among it), with the
boot-time voice code in `startUp.c` right after and AudLock.c / UAudMemStack.c a little later. Headers: `include/core/gameaudio.h`, `audtrack.h`, `audcontainers.h`, `startup.h`.

| File | Name | What it is |
|---|---|---|
| GameAudio.c | ours | the game's side: starts the engine, drives emitters for course, mode and pin |
| AudTable.c | ours | 256 playing sounds; 3D volume, pan and doppler |
| HLAudMaster.c | T7 | master settings: 32 submix volumes and mutes |
| hlaudmovie.c | T6 | a movie's stereo sound; sound banks, stream file and buffer |
| hlaudtrack.c | T6 | 32 tracks, priority list for stealing |
| hlaudtrackseq.c | T6 | the sequencer (event-played tracks) |
| hlaudtrackstm.c | T6 | streamed tracks (music, long sounds) through ARAM |
| hlaudvoice.c | T6 | voices over startUp.c's hardware voices |
| hlaudemitter.c | T6 | 256 emitter instances |
| UAudContainers.c | T6 | list, queue and pool containers |
| AudReverb.c | ours | the aux A effect: reverb or delay |
| startUp.c | EA | boot-time systems: 50 hardware voice wrappers, the mixer callback, ARAM heap, built-in sounds, boot memory-card checks, the start-up UI commands, the `'LEGL'` pictures |
| AudLock.c | ours | the sound engine's mutex and semaphore |
| UAudMemStack.c | EA | the sound engine's 384 KB stack allocator |

### Save data

Save profiles and their unlocks, the memory card, EA's shared file library (tag files with CRC32
and an XOR cipher), EA Sports Bio, and the Game Boy Advance link.

| File | Name | What it is |
|---|---|---|
| user.c | EA | allocates the five save profiles and the shared unlocks |
| MC_Gc.c | EA | the GameCube memory card layer (CARD library) |
| MC.c | EA | the game's side of the memory card: find, load, save |
| TibExt.c | EA | glue between EA's libraries and the game: memory, clock, memory-card callbacks |
| gbacable.c | EA | the GBA link cable: link state and commands |
| EASportsBio.c | EA | the game's side of EA Sports Bio |
| EASBStorage.c | ours | the storage half of the EA Sports Bio library (may be several EA files) |
| EASB.c | ours (the library's file tag) | EA Sports Bio's public calls |
| Common/Checksum/ChecksumCRC32.c | EA | EA shared file library (ProDG GCC build): CRC32 |
| Common/Cipher/CipherXOR.c | ours | EA shared file library: an XOR cipher |
| Common/SharedFileIO/SharedFileIO.c | EA | EA shared file library: the cross-platform save I/O |
| NGC/SharedFileIO/llSharedFileIO.c | EA | EA shared file library: its GameCube layer |
| Common/TagFile/TagFile.c | EA | EA shared file library: the tag-file save container |

The `Common/` and `NGC/` files are built with SN ProDG (GCC 2.95), unoptimised, as EA's
`EASharedFileLib` library in `configure.py`.

### Disc

| File | Name | What it is |
|---|---|---|
| DiscError.c | ours | the disc-error screens, drawn straight into the frame buffer with an 8x8 font |
| Code800B7210.c | ph | split off DiscError.c: the messages, drawing one for a drive status, the per-frame drive check |
| DiscCheck.c | ours | which disc is in the drive; disc changes; course files per disc |

### Unplaced

Small sweep files whose file and subsystem are not known:

| File | What it is |
|---|---|
| unsorted/sweep_8003944C.c | a function returning 0, between GoPostFx.c and GoDynamicCam.c |
| unsorted/sweep_80095744.c, sweep_80095780.c, sweep_80095798.c, sweep_800957B0.c, sweep_800957D8.c | setters and getters of an object's +0x18..+0x2C fields, between Code80095564.c and CharAnim.c |

In the SDK and runtime address range, not EA code: `unsorted/sweep_801338E0.c` (EXI2 / debugger
stubs), `sweep_801654D0.c`, `sweep_801654F4.c`, `sweep_80165524.c` (Metrowerks critical sections;
`progress_category="sdk"`), and, by their addresses *(unverified)*, `sweep_8013B3C4.c` (an empty
function between the DVD files), `sweep_8014CA80.c` (between OSSync and OSThread) and
`sweep_80155F40.c` (MSL's critical-region stubs).
