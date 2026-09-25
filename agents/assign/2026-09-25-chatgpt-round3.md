# Assignment: ChatGPT, round 3 (2026-09-25 evening)

From the orchestrator (cloud Claude). Replaces the round-2 list. Same rules as round 2
(`2026-09-25-gemini-chatgpt.md`, "Rules the merge enforces"); read `agents/brief.md` again if it
has been a while. Golfer.c stays off limits (the PC's permuter).

## What changed: structural misses only

Round 2 went to 99%-plus near-misses; most of those differ only in register numbers or a constant,
which is roulette. `python tools/agents/diffkinds.py` sorts every non-exact function by what kind of
difference it has:

- **LOGIC**: instructions missing, extra or different. The C's structure is wrong (a loop form, a
  missing inline or temp, a wrong type, a condition written another way). Reading the target
  assembly and fixing the shape usually solves these.
- CONST: only a constant or an offset differs (a type or struct detail).
- REG: only register numbers differ. **Do not spend this round on REG or CONST functions.**

Everything below is LOGIC, lightly worked, and **the last function of its unit** (exact = the unit
links), smallest first.

## Your list, in order

| Unit (what it is) | Function | Now | Size |
|---|---|---|---|
| GoShaderObjectCommon_MorphAn (morph animation shader) | fn_800976A8 | 88% | 292 B |
| LLPictInt (picture decoder) | PictInt_Decode | 93% | 440 B |
| goballfx (ball effects render) | BFX_vRender | 90% | 564 B |
| CamSpline (camera paths) | fn_800C7A9C | 93% | 632 B |
| LogoTexture | fn_8010FC3C | 78% | 800 B |
| hlaudtrackstm (audio streams; with Stm_Tick at 99.1% the other one) | fn_800AB860 | 84% | 84 B |
| GoShaderObjectContainer_OBFD (2 left) | fn_8006FDD4, then fn_8006F650 | 85%, 96% | 112 B, 1.2 KB |
| uiArc | fn_80102AC8 | 94% | 3 KB |

## How to work a LOGIC function

1. `python tools/match/perm_setup.py <Unit> <fn>`, then
   `python tools/match/quicktrial.py <fn> --show --aligned`: the `@N replace/insert/delete` blocks are
   the structural differences. Fix those first; ignore register-only lines until the instruction
   stream lines up.
2. Read the target assembly for that stretch and ask what C produces it. `docs/decomp-notes.md`
   "Try these first" lists the shapes CodeWarrior is picky about.
3. Once no structural lines remain, the function is REG/CONST: try the usual levers for an hour at
   most, then log it and move on.
4. Log every attempt in `agents/tried/<fn>.md` (what, score before -> after), solved or not.

## Branch and report

Work in your worktree on a new branch `codex/round3` (from origin/main). One commit per solved
function, `git merge origin/main` and `main.dol: OK` before each push. Report to the owner: exact
functions, units now all exact, what resisted and why.
