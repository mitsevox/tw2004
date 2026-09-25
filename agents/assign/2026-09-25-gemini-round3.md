# Assignment: Gemini, round 3 (2026-09-25 evening)

From the orchestrator (cloud Claude). Same rules as round 2 (`2026-09-25-gemini-chatgpt.md`, "Rules
the merge enforces"); read `agents/brief.md` again if it has been a while.

**Off limits:** Golfer.c (the PC's permuter), ChatGPT's round-3 units
(`2026-09-25-chatgpt-round3.md`), and every unit not listed below: Claude lanes start on the rest
at 6pm CT.

## What to work on: structural misses, whole units

`python tools/agents/diffkinds.py --unit <Unit>` shows each function's kind. LOGIC = instructions
missing, extra or different (the C's shape is wrong; read the target asm and fix it). Skip REG
(register numbers only) and CONST unless the whole unit is otherwise exact. Every function in
these units is yours; finishing a unit links it.

| Unit (what it is) | Left | Start with |
|---|---|---|
| DiscError (the "disc can't be read" screen) | 4 | fn_800B6E1C 94%, fn_800B6CD8 93%, fn_800B694C 99% |
| GoGreenGrid (the putting-green grid overlay) | 4 | fn_8009C914 94%, fn_8009B970 98%, GR_BuildGridRenderData 97% |
| SkinPart (skinned mesh parts) | 8 | fn_800CCC1C, fn_800CCF90, fn_800CC8BC, fn_800CEBE8, fn_800CE660 |
| GoShaderObject_Grass_Gc (grass shader) | 2 | both SD_vShaderObject_Grass_Static_*: 97%, 82% |
| GoShaderObject_Particle_Gc (particles) | 4 | fn_800949D0 91%, fn_80094B84 88% |

## Method

1. `python tools/match/perm_setup.py <Unit> <fn>`, then `python tools/match/quicktrial.py <fn> --show
   --aligned`: fix the `@N replace/insert/delete` blocks first, register-only lines last.
2. `docs/decomp-notes.md` "Try these first". Read `agents/tried/<fn>.md` before, log every attempt
   (what, score before -> after) after.
3. An hour at most on a function once it is register-only; log it and move on.
4. Pointers never in `int`/`u32` (use `uptr`); fake matches labelled `// fake match:` and logic-preserving.

## Branch and report

`git worktree add ../tw2004-gemini3 -b gemini/round3 origin/main`, `python configure.py`, `ninja`
(`main.dol: OK`). One commit per solved function (`<Unit>.c: <fn> exact (<how>)`); `git merge
origin/main` and `main.dol: OK` before each push. Linking a finished unit (configure.py Matching +
splits.txt data ranges) is welcome; see commits 45d089d / 5f593e9, or just report it. Report to the
owner: exact functions, units now all exact, what resisted and why.
