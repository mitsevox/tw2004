File formats
============

Inventory of the data on the two `GW4E69` discs. Nothing here is reverse-engineered
yet; this is the starting map. One page per format gets added as it is understood.

Containers and file types
-------------------------

| Extension | Disc 1 | Disc 2 | Seen in | Status |
|-----------|-------:|-------:|---------|--------|
| `.hog`    | 198 | 171 | course folders under `Data/` | unknown |
| `.gcb`    | 120 | 120 | disc root (`glbdata`, `glbchar`, `fechar`, `startup`, `loadonce`, `malesac`, `femsac`) and `Data/` | unknown |
| `.sac`    | 34  | 34  | `Data/` (character related) | unknown |
| `.fxg`    | 32  | 32  | `Data/` | unknown |
| `.ngc`    | 5   | 5   | `Data/` | unknown |
| `.sab`    | 1   | 1   | `audiostm_gc.sab` at disc root, 463 MiB | unknown |

Counts are for `Data/` recursively, except `.sab`.

Course folders
--------------

| On both discs | Disc 1 only | Disc 2 only |
|---------------|-------------|-------------|
| `01_peb` `03_saw` `04_vol` `09_bay` `10_for` `11_spy` `12_pop` `17_sah` `18_jpn` `20_kap` | `02_pri` `08_dri` `13_hig` `14_sco` `15_tor` `16_sai` | `06_bet` `07_bir` `19_aus` `21_pin` |

Not yet checked: whether the folders present on both discs are byte-identical.

Other `Data/` folders (both discs): `chars`, `charsac`, `charstrm`, `fechars`, `Fend`, `load`, `movies`.
