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

u16 header, then 34 records of 320 bytes:

    0x000 char first[32]
    0x020 char last[32]
    0x040 char nickname[32]      "NA" when none
    0x060 u8[8] zero
    0x068 u8  a, b              small numbers (0x19 0x18 for Woods, 0x07 0x00 for "Player One")
    0x06A u8  c, d, e           1 1 1 / 2 2 2 / 0 0 0: a category (gender? model set?)
    0x06D u8  0
    0x06E u8  attr[18]          0..100
    ...   rest zero / unknown

Attribute rows (0..100), column meaning still unknown - the accessors in `CharSliders.c`
(`0x8010D614`) will name them:

    Woods          100 90 100 95 85 100 90 95 100 80 85 100 100 100 100 100 100 100
    "Sunday" Woods 100 100 100 100 95 100 100 100 100 80 95 100 100 100 100 100 100 100
    Daly           100 100 90 75 70 75 85 85 90 80 60 75 100 40 33 40 33 33
    Singh           95 95 90 100 80 95 95 95 95 80 100 90 75 40 40 40 100 40
    Player One      25 20 10 10 10 10 10 10 10 80 10 50 20 10 10 10 10 10   (created golfer start)

Two things stand out for hypothesis 4: column 9 is 80 for everyone including the created
golfer (a fixed value, so probably not a slider), and the last six columns look like a
second group (the fictional golfers vary them, the pros are mostly flat).

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
