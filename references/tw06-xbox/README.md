# Tiger Woods PGA Tour 06 Xbox PDB inventory

Generated from the July 12, 2005 Xbox beta `default.pdb`, paired with debug `default.exe` and
`default.xbe`.

- `type-index.txt`: 7,764 named type records.
- `module-index.txt`: 830 object modules.
- `source-files.txt`: 1,598 unique source/header paths from DBI file information.
- `module-source-files.json`: DBI module-to-source-file association.
- `key-types/`: targeted reconstructions for six types already relevant to TW2004.

The key extracts cover `PhysicsBall_t`, `SW_sSwingData`, `GameModeDriverPGATour`, `GolferData_t`,
`GamePlayer`, and `FE_CrAP_Asset_t`. They are convenience snapshots, not a substitute for querying
the original PDB with the project's `tpiread.py`.

Indexes and targeted declarations were generated with
[ReSym 0.4.0](https://github.com/ergrelet/resym). A complete `dump-all` did not terminate on this
PDB, and ReSym warned about several zero-sized Xbox SDK aliases. Consequently, array dimensions in
affected SDK types should not be trusted without a targeted query. The named Tiger types above
reconstructed successfully.

The DBI source-file list was read with `pdbparse` 1.5 after correcting a local Python 3 expression
bug in its file-name parser. The older library cannot parse this PDB's full TPI stream and was not
used for type reconstruction.
