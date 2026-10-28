# GW4E69-rev0

USA release, disc revision 0. `main.dol` is identical on Disc 1 and Disc 2, SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`, so there is a single code target.

| File | Purpose |
| --- | --- |
| `config.yml` | decomp-toolkit project input: the original, its hash, the symbol and split files |
| `build.sha1` | Expected hash of the relinked executable |
| `symbols.txt`, `splits.txt` | Every symbol name and unit boundary |
| `filemap.json` | The game's original source files in link order |
| `name_sources.tsv` | Frozen log of how the earlier names were found; append-only |
| `evidence.tsv` | The standard evidence table ([format](../README.md)); this project names from the code and `references/`, as `docs/style.md` describes, so it stays empty |
