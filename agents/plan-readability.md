# Plan: readability (from 2026-09-27)

The match is 100% and stays so (`main.dol: OK` on every commit). The job now: a person or a fresh AI
can open any file and understand it. Owner: one focus at a time, one pass per piece of code, no
second passes, no rule from the matching phase carried over without asking.

## The unit of work: one file, one complete pass

A lane takes one file (or a small group from one area) and leaves it DONE, in batches of at most
30 functions (fresh agent per batch, so context stays small and names stay accurate):

1. **Every function named**: EA's name where TW07/TW06/Madden/EA text confirms it (T1/T2), else
   read from the code (T3). A function nobody can explain is listed with why, not guessed.
2. **Every function's comment right**: kept if right, rewritten if wrong / stale / vague, added if
   the name does not say everything. Labels (`fake match:`, `port:`, `EA bug:`) kept.
3. **Every function reviewed on record**: a comment the lane keeps gets a KEEP row, so "reviewed"
   is measured, not assumed.
4. **Its locals** read as words (TW07's local names where it has the function; e.g. Ball.c
   Physics_HandleCollision's fE, fF...).
5. **The file's header comment** says what the file is (TW07's file name where known).
6. **Its data**: the globals the file defines get names and comments (`gName`), same evidence rules.
7. **Its fake matches and pragmas**: each has a `fake match:` label saying why (inventory, not
   rewrite; replacing a fake match with EA's form is matching work, later).

Struct fields live in shared headers: the lane that owns an AREA (below) does its headers' fields
once, at the end of the area, so two lanes never rename the same struct.

## Areas (from src/README.md's subsystems) and order

Most-called and most-read first: memory/math/render core -> golfer (char, skin, animation) -> round
flow and game modes -> HUD and front end -> audio -> cameras -> course/terrain -> saves (MC, EASB) ->
the SDK-facing and library code (TibExt, IStudio: EA names from Madden 2003) -> `src/unsorted/`
(place each sweep file into its real unit as its functions get understood).
A file already partly named in rounds 1-3 is simply next in its area: its pass covers everything
still missing, including the comment review those rounds skipped. No separate backfill passes.

## The feedback loop

- **Measure** (`python tools/match/hotnames.py --units`): per file, named / commented / reviewed /
  done, and the call-site coverage line. Published with the dashboard numbers.
- **Pick** the next file in the area order (highest unnamed call sites first inside an area).
- **Do** it: lane writes batch files (names, comments, KEEPs); hand edits (headers, file comments)
  listed in its report.
- **Gate**: `tools/match/name.py` replays each batch on main: evidence row per name, labels kept,
  lines rewrapped, lint clean, `main.dol: OK`, or nothing applied. The orchestrator reads the diff.
- **Learn**: each round's pipeline friction is fixed in the tools before the next round (never
  worked around by hand twice); wrong leads and pairing errors go to the findings file.
- **Rules check**: at every phase change (and before each round), re-read CLAUDE.md, brief.md and the
  role file for rules from an earlier phase; ask the owner about any; never carry one silently.

## After the code areas (scheduled, not parked)

- `docs/gameplay.md`: rewritten from the named, commented code (its matching-era claims were never
  checked); `docs/hypotheses.md` merged into it.
- `README.md`: the front door, rewritten for someone arriving now (it still says 26% matched).
- Port-hazards doc: every `port:` and `EA bug:` label, sorted by effect (raw material also in
  agents/findings/audit-reports/, which is removed once distilled).
- Compiler-flag audit: per-unit flags grouped by library; one-file outliers marked as likely fake
  matches.
- Fake-match rework (matching lanes): replace labelled fakes with EA's form where one is found.

## Pacing

6 lanes at a time (owner, 2026-09-28; was 4); batches of at most 30 functions; check usage before each round. Lanes hand in
batch files; the orchestrator replays them serially on main (lane branches conflict on callers).
