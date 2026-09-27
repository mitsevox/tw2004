# Naming lane (from 2026-09-27)

Goal: a human or a fresh AI can read the code. Every function gets a name, and a comment when the
name alone does not say it, from ONE careful reading of the function. The byte match
(`main.dol: OK`) proves nothing broke; it does not prove a name right, so read carefully.

## Your territory
Your prompt gives one or two source files and a batch size (15-25 functions). Stay inside them:
another lane has the other files. Order: `python tools/match/hotnames.py --unit <Unit>` (most
call sites first: one name fixes every caller). `python tools/match/hotnames.py --units` shows
your files' named / commented / done counts; report them before and after.

## Per function
1. Read it whole, its callers (`python tools/match/callgraph.py <Unit>` lists calls, callers,
   globals and strings) and the struct fields it touches (their comments in include/).
2. EA's name first: `agents/findings/2026-09-27-name-pairing.tsv` (n1's TW07/TW06 pairing; use
   confidence A and B only, C is noise) and `docs/reference-builds/tw07-ps3/cu/<File>.txt`. An EA
   name the code confirms is tier T2, codes E2.
3. Otherwise name it from what it does: tier T3, codes E6 (E4 when it rests on named fields, E3 for
   a wrapper). EA's style: `System_Verb`, reuse the prefix EA or the file already uses (`Mem_`,
   `Vec_`/`Vec3_`, `Mtx_`, `Quat_`, `RenderState_`, `Ter_`, `GM_`, `MC_`, `SitDev_`, `Emi_`, `GUI_`,
   `FE_`, `EASB_`). Say what it does, not how. No address, no "maybe": the tier says how sure.
   A copy of a helper another file already has gets the file's prefix: `GolfCam_Vec3Sub`.
4. Comment (column 8) when the name does not say everything a caller needs: units, ranges, what
   0/NULL means, bit layouts, side effects, which game feature it serves. One or two sentences, plain
   words, true to the code. Short getters with a clear name need none. A function that already has
   a comment keeps it; if you think it is wrong, list it in your report (do not edit it).
5. Skip rather than guess wrong: raw sweep code (`src/unsorted/`), a function you cannot explain
   after reading its callers. List skips in the report with why.

## Applying (one command, all or nothing)
Write the batch to your scratch folder, tab-separated (docs in `tools/match/name.py`):
`address  current_name  new_name  tier  codes  evidence  purpose  [comment]`
(for a function that already has a name and only needs a comment: new_name = current_name).
```
python tools/match/name.py <scratch>/batch.tsv --check
python tools/match/name.py <scratch>/batch.tsv --by "<your lane> (<model>)"
```
It renames everywhere, updates the Markdown docs, adds the comments, rewraps long lines, logs every
name in `config/GW4E69/name_sources.tsv`, builds and checks `main.dol: OK`, or puts everything back.
Then read `git diff` (every name and comment in context), fix any lint line it lists with the
Edit tool, and commit: `names: <Unit> batch N (<count> names, <count> comments)` plus `main.dol: OK`.
Never edit C through the shell. Never add an AI footer or Co-Authored-By line.

## Report
Before/after of `hotnames.py --units` for your files and the coverage line, the names you are
least sure of (with why), comments you think are wrong, skipped functions. Stop after your batch.
