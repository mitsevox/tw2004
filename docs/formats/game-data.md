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

**Column names (inferred, not yet confirmed from code).** The front-end data
(`Data/Fend/FEnd.gcb`, `DATS` object) carries the attribute screen's names and tooltips, and a
debug options menu that lists twelve attributes in this order:

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

Twelve names, and the record has two blocks of twelve. Applying the debug order to the data
fits two signatures: column 9 (SPEED) is 80 for every golfer in both blocks (one exception at
85), and the created golfer's column 11 (LUCK) starts at a neutral 50 while everything else
starts at 10-25. The player screen shows nine of the twelve; AGGRESSION, IQ and SPEED are
hidden and are the obvious CPU-golfer knobs - and the created golfers have them too (10, 10, 80).
`CharSliders.c` (`0x8010D614`) will confirm the order and say what block B is.

Block A with the names applied (block B in parentheses where it differs a lot):

    Woods        100  90 100  95  85 100  90  95 100  80  85 100   (B: all 100)
    Daly         100 100  90  75  70  75  85  85  90  80  60  75   (B: 100 then 33-40s)
    Singh         95  95  90 100  80  95  95  95  95  80 100  90   (B: 75, 40s, DRVACC 100)
    Player One    25  20  10  10  10  10  10  10  10  80  10  50   (B: 20 then 10s)

Block B is flatter (Goosen is 71 across the board, Gulbis 50, Montgomerie 39) as if generated
from one rating; candidates are a difficulty variant or the values used when the pro is the
CPU opponent rather than the played golfer.

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

Tools
-----

    python tools/research/ctrl_dump.py loadonce.gcb --out DIR      # writes NNN_stat_.bin etc.
    python tools/research/txg_export.py DIR/012_BALF_.bin flags/   # the flag textures
