Game data objects (`loadonce.gcb`, `Data/Fend/FEnd.gcb`)
==========================================================

Status: **located and partly decoded from the files.** These are the small tables the gameplay
hypotheses care about. Every container carries an `RLst` object that maps each object type to
the original file name the tool packed, so the names below are EA's own.

| Type | Original file | Contents |
|------|---------------|----------|
| `stat` | `DATA\STATS_GC.BIN` | **golfer attribute table** (below) |
| `rcrd` | `DATA\RECORDS_GC.BIN` | course records: score, golfer surname |
| `TRAX` | `DATA\EATRAX.BIN` | EA TRAX song list |
| `TCM ` / `TCMS` | `DATA\CHALLENG_GC.BIN` / `WORLDTOUR_SYMBOL.STR` | World Tour map: unlock rules ("DEFEAT JOHN DALY AT SAHALEE CC TO UNLOCK THIS TOUR STOP") |
| `PLY ` / `PLYs` | `DATA\PLAYNOW_GC.BIN` / `PLAYNOW.STR` | Play Now scenarios ("Lucky 7", "2 Down Comeback") |
| `ERN ` | (earnings) | trophy-ball / achievement list with point values |
| `PGST` | (PGA stats) | real PGA TOUR pros (Allan, Allenby, Ames, ...) with float stats: the season leaderboard |
| `PGAc` `PGAt` `PGAp` `PGAn` | (PGA calendar/tour/purse/names) | season schedule: 19 events, names ("Kapalua Championships", "AT&T Pebble Beach National Pro-Am"), purses |
| `RTEc` `RTEs` `RTEn` | (real-time events) | calendar events keyed to the console clock: "New Year's Day", "Natalie Gulbis' Birthday", "Full Moon" |
| `CRI ` | | 22 KiB numeric table, unknown |
| `CMPS` | | 252 bytes of small ints, unknown |
| `BIO ` | (FEnd.gcb) | golfer bio cards (below) |
| `BALF` | (FEnd.gcb) | despite the name, a `TXG ` group of 32 flag textures, 64x32 CMPR |
| `LITE` | (FEnd.gcb) | 5 front-end lights: type, colour (4 floats), direction; last is ambient 0.2 |
| `eagm` | (FEnd.gcb) | EA game-ID list for the EA SPORTS Bio feature (`GDTE` = Def Jam Vendetta ...) |

Golfer attribute table (`stat`, STATS_GC.BIN)
--------------------------------------------

u16 header (0), then 34 records of 320 bytes (record i at `2 + i*320`):

    0x000 char first[32]
    0x020 char last[32]
    0x040 char nickname[32]      "NA" when none
    0x060 u8  a, b              small numbers (0x19 0x18 for Woods, 0x07 0x00 for "Player One")
    0x062 u8  c, d, e           1 1 1 for everyone seen so far
    0x065 u8  0
    0x066 u8  attrA[12]         0..100, see column names below
    0x072 u8  attrB[12]         0..100, same columns, second set (meaning unknown)
    0x07E u8  misc[15]          0..4 each (all 0 for the created golfers); flags at 0x08D..
    ...   rest unknown

**Column names (confirmed from code).** The front-end data (`Data/Fend/FEnd.gcb`, `DATS`
object) carries the attribute screen's names and tooltips and a debug options menu that lists
twelve attributes; the attribute screen (`FE_GolferAttributes`, `0x800BBB10`) reads them through
the accessor below with indices 0,1,3,4,5,6,7,10,11 in this order, which fixes the mapping:

    0 POWER            "Increases distance on full swing shots."
    1 POWER BOOST      "Increases maximum power boost on your backswing."
    2 AGGRESSION       (hidden - not on the player screen)
    3 BALL STRIKING    "Improves iron accuracy."
    4 DRIVING ACCURACY "Improves accuracy off the tee and distance control."
    5 APPROACH         "Improves chip & pitch shot accuracy."
    6 PUTTING          "Improves putting accuracy."
    7 RECOVERY         "Improves accuracy from sand and rough."
    8 IQ               (hidden)
    9 SPEED            (hidden)
    10 SPIN            "Increases maximum spin you can generate."
    11 LUCK            "Increases % of favorable lies and bounces in the trees."

In memory the whole file sits at `gGolferTable` (`0x801CB300`, 34 x 0x140), so a record's
fields are at file offset + 2: block A at record+0x68, block B at +0x74, and the 0..4 bytes at
+0x80 are per-attribute equipment tiers (`Golfer_TierBonus` clamps them to +0..+4 points).
`gCurGolferRecord` (`0x801CB1C0`) is the created golfer being edited; `Golfer_TableSetup`
copies it into slot 30 and, when a debug flag is set, writes 105 into all twelve attributes of
every golfer (the "105%" string on the attribute screen is that cap).

**The accessor.** `Golfer_GetAttribute(pPlayer, k, mode)` (`0x8002E15C`) returns for attribute k:

    mode 0  base:  block B[k] if (pPlayer->field_A08 == 9 && golfer index < 30 &&
                                  (Game_GetMode() == 4 || debug bit)) else block A[k];
                   plus Golfer_TierBonus(tier[k])
    mode 1  modifiers only: the byte at pPlayer+0x148+k
    mode 2  both (the screen's "attributes / modifiers / total")

So block B is the pros' alternative attribute set, used only in game mode 4 for a player whose
field_A08 is 9 (both still to be named). It is flatter than block A (Goosen 71 across, Gulbis
50, Montgomerie 39) - a rating-derived set.

**Who reads what** (87 call sites in 19 functions; file names are the nearest named file in
`filemap.md` and may be wrong for the unnamed ones):

    fn_8002AA74  (28)  POWER IQ AGGRESSION STRIKING APPROACH PUTTING RECOVERY, each in
                       mode 0 and mode 2, twice     <- the only reader of AGGRESSION and IQ
    fn_8002C2DC  (4)   AGGRESSION IQ POWER + one variable index
    fn_8002B59C  (7)   PUTTING RECOVERY SPIN x3 STRIKING APPROACH
    fn_8002D074/fn_8002D994   POWER / LUCK
    fn_80052598, fn_80053594  LUCK                       (UKernel.c region: lies / bounces?)
    fn_800589F8  (6)   PUTTING STRIKING APPROACH RECOVERY SPIN POWER, mode 0   (user.c region)
    Swing.c: fn_8005B250 PUTT APPR RECOV DRVACC; fn_8005C01C BOOST; fn_8005C4B4 SPIN;
             fn_8005C5EC RECOV PUTT APPR DRVACC STRIKING x4; fn_8005C960 PUTT APPR RECOV STRIKING
    fn_80086C78, fn_80086E5C  RECOVERY                   (FE_Manager.c region)
    FE_GolferAttributes       the nine visible ones      (the attribute screen)
    fn_800D18D8  RECOVERY x2;  fn_800FBB30, fn_800FBD2C  SPEED   (SkinPart.c region: animation?)

AGGRESSION and IQ are never read by the swing code; their only readers are the
`0x8002AA74`/`0x8002C2DC` pair, which also sample every shot-relevant attribute twice in both
modes - the shape of a shot-selection routine. Reading those two functions is the next step.

Golfer bio cards (`BIO `)
-------------------------

u32 count (26), then records of 562 bytes:

    0x000 char name[32]
    0x020 char nickname[19]
    0x034 u32 height feet, u32 height inches, u32 weight lb
    0x040 u32 birth month, u32 birth day, u32 birth year   (Woods: 12 30 1975)
    0x04C char hometown[32]
    0x070 char strength[40]     "Power / Forging Drivers", "All Around Player", "Driving Accuracy" ...
    0x098 char bio[...]         the front-end paragraph

CPU aim points (a course chunk, loader `AI_TargetsLoad`)
---------------------------------------------------------

Registered with `Course_RegisterLoader(0, AI_TargetsLoad)`, so it is chunk kind 0 of whatever
container the course loader walks (not yet traced to a file). Per hole:

    0x00 s16 ?
    0x02 s16 count            (at most 25)
    0x04 AITargetDef[count]   0x30 each: f32 x, y, z; u32 ?; s16 links[10] (indices, self = none);
                              12 unknown bytes
    then u8[count][8]         tee set (-1 any), hole (-1 any), skill requirement, aggression
                              requirement, priority flag, shot shape (0 straight, 1/2 curve,
                              3/4 punch/lob, 5/6 big curve), power requirement, pad

How the CPU uses it is in `docs/gameplay.md` (`AI_ChooseTarget`).

Tools
-----

    python tools/research/ctrl_dump.py loadonce.gcb --out DIR      # writes NNN_stat_.bin etc.
    python tools/research/txg_export.py DIR/012_BALF_.bin flags/   # the flag textures
