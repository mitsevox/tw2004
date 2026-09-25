# Assignment: Gemini and ChatGPT, 2026-09-25

**SUPERSEDED for ChatGPT by `2026-09-25-chatgpt-round3.md`** (its rules section below still applies).

## UPDATE (late afternoon): Gemini is out of usage; ChatGPT takes this order

Gemini's units are released to ChatGPT. Park Stm_Tick (two rounds, 91 -> 99.10%, not exact: log
where you are in agents/tried/Stm_Tick.md and move on). Work top-down; each item links a unit
or is one step from it:

1. **uiProcessInterface fn_8008F820**: the unit's LAST function (Gemini did fn_8008F488). Exact =
   the unit links; link it (configure.py Matching + splits.txt data ranges, see the LLFileIO_Gc and
   UStream commits 45d089d / 5f593e9 for how) or just report and the orchestrator links it.
2. **SunFlr_Gc fn_8009A708, fn_8009A3F4**: both 99%, the unit's last two (sweep best 3 on one).
3. **Swing SW_vImpact** (100%, sweep best 6), then SW_KillVibration.
4. Your original list from GameModeBestBall down, then Gemini's (Ball Physics_HandleCollision,
   DepthField, Code8002EE1C, MC, Glows).

Same rules as below. Pointers never go in `int`/`s32`/`u32` locals: use `uptr` (64-bit port);
the merge caught one in hwsRender_Gc.

From the orchestrator (cloud Claude). You two share one machine; this file splits the work so you
never touch the same unit. Local Claude has the PC and is running the permuter on Golfer
AI_ChooseTarget: Golfer.c is off limits to both of you.

## Before anything

1. `git fetch origin && git pull origin main`. Read `agents/brief.md` (the rules), then
   `docs/decomp-notes.md` "Try these first".
2. **Work in your own checkout, not the shared one.** Same machine + same files = tangled
   commits (it already happened on SkinPart fn_800CDF80: you both solved it at once).
   - Gemini: `git worktree add ../tw2004-gemini -b gemini/round2 origin/main`
   - ChatGPT: `git worktree add ../tw2004-chatgpt -b codex/round2 origin/main`
   Run `python configure.py` and `ninja` inside yours; it must end with `main.dol: OK`.
3. Before a function: read `agents/tried/<fn>.md`. After it (solved or not): add your attempts
   under "Attempts" (what you tried, score before -> after). Do not repeat a listed attempt unless
   you combine it with something new.

## Your units (only these; the other agent's list is not yours)

Ranked by value: the first ones are 1 function from linking the whole unit.

### Gemini

| unit | function(s) left | now |
|---|---|---|
| Ball (ball physics) | Physics_HandleCollision | 99% |
| hwsRender_Gc | fn_801132C4 | 99% (sweep best 3) |
| SunFlr_Gc (sun flare) | fn_8009A3F4, fn_8009A708 | 99%, 99% |
| uiProcessInterface | fn_8008F488, fn_8008F820 | 96%, 100% (sweep 3 / 22) |
| PsMgr | fn_800A2A80 | 98% |
| DepthField | DF_vDrawBufferToScreen | 98% |
| Code8002EE1C | Session_SetupProfiles | 96% |
| MC | fn_8009F8C8 | 96% |
| GoShaderObject_Glows_Gc | fn_80098408 | 95% |
| stretch: LLDisp_Gc | fn_80006A98 | 78% |
| stretch: GameMode22 | fn_801264B8 | 86% |

### ChatGPT

| unit | function(s) left | now |
|---|---|---|
| Swing | SW_vImpact, SW_KillVibration | 100% (sweep 6), 93% |
| GameModeBestBall | fn_800E8A68 | 95% |
| uiArc | fn_80102AC8 | 94% |
| CamSpline | fn_800C7A9C | 93% |
| LLPictInt | PictInt_Decode | 93% |
| LLDynTex | fn_8010A930 | 91% |
| goballfx | BFX_vRender | 90% |
| UIStudio | fn_80166098, fn_80168918 | 87%, 97% |
| hlaudtrackstm | fn_800AB860, Stm_Tick | 85%, 91% |
| stretch: LogoTexture | fn_8010FC3C | 78% |
| stretch: GoShaderObjectCommon_MorphAn | fn_800976A8 | 88% |
| stretch: GoShaderObjectContainer_OBFD | fn_8006F650, fn_8006FDD4 | 96%, 85% |

Everything else (the 99.9% ones: ska_shared, TerrainData, StaticCam, uiText, hlaudmovie,
hwsBurn, Rain, BreakLine, Earnings, startUp, LLTex, and the big units) is held: they were worked
hard already. Ask the owner before switching to anything not in your list.

## Tools (no permuter on the Mac; these work without it)

```
python tools/match/perm_setup.py <Unit> <fn>        # snapshot for quicktrial / leversweep
python tools/match/quicktrial.py <fn> --show --aligned
python tools/match/leversweep.py <Unit> <fn> --minutes 6 -j 4
python tools/match/trial.py ...                      # confirm in the real unit (docs/workflow.md)
```

A function with a lot left (below ~95%) is usually a structure problem (a missing inline, a wrong
loop form, a type): read the target assembly and fix the shape before trying orders and levers.

## Rules the merge enforces (a branch that breaks one is rejected)

- Only exactly 100% counts, and `main.dol: OK` on every commit.
- **No assembly.** Not an `asm` function, not an `asm {}` block, not under `#ifdef`. The merge
  script refuses it (tools/agents/asmgate.py).
- Fake matches are allowed only when labelled `// fake match: <why>`, the logic is unchanged, and
  it stays portable C. Identity helper naming: `static inline <type> fn_<caller address>_Read(...)`.
- Matching notes only: never rename anything, never rewrite an existing comment.
- Do not edit headers or shared files (configure.py, splits.txt, symbols.txt) except to link a
  unit you finished; say so in the commit.
- Never edit C or headers through the shell (sed, heredocs, python -c): use your editor tools.
- Never delete files. No commit trailers (no Co-Authored-By, no "Generated with").

## When you finish (or at the owner's stop time)

1. One commit per solved function (message: `<Unit>.c: <fn> exact (<how>)`), ledger updated.
2. `git merge origin/main`, `ninja` -> `main.dol: OK`, push your branch (`gemini/round2`,
   `codex/round2`).
3. Report to the owner in a few lines: exact functions, units that are now all exact, what you
   tried on the rest. The orchestrator merges.
