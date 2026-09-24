# Name and comment audit of EA game code (from 2026-09-24)

The user's rule: every existing function name and comment in EA game code is an UNVERIFIED CLAIM
until this audit passes it. No fresh names or new comments anywhere until the audit reaches 100%.
SDK / Dolphin / MSL code is out of scope. Progress: `python tools/match/auditprogress.py`.

Per unit, three agents: LANE 1 (Sonnet, evidence) and LANE 2 (Opus, behaviour) work BLIND and never
see each other's output; the RECONCILER (Opus) sees everything and writes the batch. The
orchestrator spot-checks and applies it with audit_apply.py (gated).

## Hard rules (every role)
- Read-only: never edit any file in a checkout or worktree. Write only in your scratch folder.
- Run tools in YOUR worktree (given in your prompt). Never the main checkout. No sed/heredoc/`python -c`.
- `date` at the start; stop at your checkpoint.
- NEVER use rm or delete any file, not even your own temp files (the user gets a scary approval prompt).

## BLINDNESS (lanes 1 and 2)
You must not see the current name or any comment of the functions you audit. So:
- NEVER open files under src/ or include/ with Read, Grep or type/cat. Your ONLY windows are:
  - `python tools/match/blindview.py <fn_ address or name> [--uses]` (the function, comments removed,
    its own name and every unaudited game function name shown as fn_<address>; --uses: the lines
    that call it);
  - `python tools/match/blindview.py --type <TypeName>` (a struct/enum, comments removed);
  - `python tools/match/callgraph.py <unit> --blind` (leaves-first list: calls, callers, globals,
    string literal TEXT, TW06 hints).
- The function list and order come from `callgraph.py <unit> --blind`: audit in that order.
- Global names, struct field names, parameter names and type names are still visible: they are
  CLAIMS written by earlier lanes, not facts. Say when a conclusion rests on one.
- A callee shown as fn_<address> is unaudited: open it with blindview if you need what it does, in
  ANY unit (allowed; it is not the function you judge). blindview takes fn_XXXXXXXX, 0xXXXXXXXX or
  XXXXXXXX. Type names come from the signatures and declarations you see (`--type <that name>`).
- Filtering tool output: use Grep on a saved file in your scratch folder, never sed/heredocs.

## TW07: EA's own names (from 2026-09-24 09:10)
EA's TW07 PS3 debug build (2006, same engine, 3 years newer) gives EA's real function names, per
source file, in source order, with parameters and locals. Two inputs:
- `docs/reference-builds/tw07-ps3/pairs_blind.tsv`: a machine pairing of OUR addresses with TW07
  functions (address, unit, tw07_name, tw07_file, line, score, conf high/med/low, evidence: shared
  strings, float constants, paired callees/callers, size ratio (PS3 is ~1.8x GameCube), gap = by
  position between two pairs). Measured precision against names we trust: high 26/26, med 66/68,
  low ~85%. It is a CANDIDATE, never a fact. (Blind lanes: this file is allowed; it does not show
  our current names.)
- `docs/reference-builds/tw07-ps3/cu/<SourceFile>.txt`: every TW07 function of that file in source order:
  line, PS3 address, size, signature (parameter names/types), locals, what it inlines. index.tsv
  lists the files. Grep them for a name.
Engine drift is real: a TW07 function can have changed job, gained parameters (C++ `this`, new
features) or moved files. Judge the pairing on OUR code.

## LANE 1 (evidence)  -> <scratch>/lane1.tsv
Per function: what the EVIDENCE says, not what the code looks like.
Columns: `address  name_from_evidence (or -)  subsystem  codes  evidence`
- E1 EA's text: a string the function uses that names its job. A bare file name / line (asserts,
  allocations) names the UNIT only.
- E2 TW06: a `tw06:` hint in callgraph, AND the code agrees on something concrete.
- E2b TW07: the same name appears in TW07's (PS3, 2006) symbol list, docs/reference-builds/tw07-ps3/
  function-symbols.txt (Grep it; `::` there is `_` here), AND the code agrees on something
  concrete. Era-adjacent, not proof: engine drift is real (a name may have moved to another job).
  A name that TW07 does NOT have is not evidence against it, only a reason to look harder.
- E3 wrapper: the body is essentially a call to an SDK/library function or an AUDITED (visible) name.
- E4 data: the globals/fields it reads and writes (say which, and that they are claims).
- E5 role: how audited callers use it (which slot, which branch).
If the evidence does not support a name, write `-` and say what is missing.
TW07 (E2b): look up EVERY function's address in tw07_pairs_blind.tsv. With a candidate, open its
TW07 file list and check it against the blind view: parameter count and types, the callees you can
see, strings, the neighbours' pairings. Supported -> name_from_evidence = the TW07 name (`::` -> `_`),
codes `E2b(<conf>)` + what you checked. Contradicted -> `-` and say why ("TW07 takes 3 args, ours 0").
No candidate: you may Grep the TW07 lists for a name the strings/callees suggest (say so).

## LANE 2 addendum (TW07)
Write the behaviour and your own guess FIRST, from the code alone. Only then open the TW07 candidate
(tw07_pairs_blind.tsv + the file list) and add a last column `tw07`: `fits` / `partial` / `no` +
one line why (does the behaviour support every word of EA's name? the parameters?). Never change
your behaviour text after reading TW07.

## LANE 2 (behaviour)  -> <scratch>/lane2.tsv
Per function: read the blind view (and --uses for how callers use the result). No name first:
Columns: `address  behaviour  guess (or -)  confidence (high/med/low)  notes`
- behaviour: one or two plain sentences of what the code DOES: the math, the loops, the state it
  changes, what it returns ("walks the 6 slots and returns the first NULL one, else -1").
- guess: only if the behaviour makes the role clear. notes: anything odd (dead code, a branch that
  looks inverted, a type that looks wrong -> "TYPE:" prefix, for the later type pass).

## RECONCILER  -> <scratch>/batch/{renames.tsv, comments.tsv, ledger.tsv, meta.txt}, report.md
You see lane1.tsv, lane2.tsv, the current source (read src/include freely) and
config/GW4E69/name_sources.tsv / docs/tw06-names.md. For EVERY function of the unit:
1. NAME (current name vs the two lanes):
   - Keep a name that both lanes support, even if a lane worded it differently. Do not rename for
     taste; rename only when the name is WRONG, MISLEADING or claims more than the behaviour shows.
   - Tiers: T1 = E1 names the job, or a TW06/TW07 name (E2/E2b) the behaviour confirms. T2 = both lanes
     agree on subsystem and action, backed by EA/TW06/SDK-named things or two independent lines,
     and lane 2's behaviour supports every word of the name. T3 = both lanes agree on the behaviour
     but the evidence rests on our own names or deduction: the name becomes `fn_<ADDR>_<Guess>`
     (e.g. fn_80082E40_BallSpinDecay). D = contradicted by lane 2, or neither lane supports it:
     back to `fn_<ADDR>`. A current `Name_<ADDR>` (old T3 style) becomes `fn_<ADDR>_<Name>` or better.
   - A function currently `fn_<ADDR>` STAYS unnamed (decision `unnamed`): no fresh naming until the
     audit is done. Put the lanes' agreed guess in the ledger notes for later.
     EXCEPTION (user, 2026-09-24): it takes EA's TW07 name (decision `renamed`, tier T1, codes E2b)
     when BOTH blind lanes independently support the TW07 pairing (lane 1: the TW07 candidate's
     evidence holds; lane 2: the behaviour fits every word of EA's name). `::` becomes `_`. Never
     our own guess, never a TW07 name only one lane supports (ledger note instead).
     SAME EXCEPTION for EA's OWN TEXT (E1, orchestrator's call with the user's go-ahead, 2026-09-24
     ~11:05): an unnamed function takes the name EA's text in this binary gives it (a panic/assert/
     OSReport string naming the function itself) when BOTH lanes agree the text names THIS function,
     not a caller or callee (decision `renamed`, tier T1, codes E1).
   - TW07 (read docs/reference-builds/tw07-ps3/pairs.tsv, which also shows our current name):
     lane 1 supports the TW07 candidate (E2b) AND lane 2 says `fits` -> the function takes EA's
     TW07 name, tier T1, codes E2b, whether it was named, misnamed or unnamed (our invented names
     like Swing_* give way to EA's SW_*). `partial` or one lane only -> keep the current decision
     and put the TW07 name in the ledger notes. A TW07 name already used by another of our
     functions -> rename.py refuses: list it in report Findings (the orchestrator resolves swaps).
     `::` -> `_`; template names are never used.
   - Names follow the unit's prefix and house style (`_Init`/`_Shutdown`, TW06 names verbatim).
2. COMMENTS: every comment in the function and the one above it (plus the unit's header comment,
   once per unit): does it match what the code does?
   - accurate -> leave it. Wrong or claims more than the code shows -> rewrite it to what BOTH lanes
     agree the code does (comments.tsv). Unknown/unsupported and not fixable -> remove it.
   - NEVER touch (not behaviour claims): `fake match`, `EA bug`, `port:`, asm addresses, notes on
     matching / registers / data order / link order / sections, `(our name)` / `(EA's name ...)`
     unit-name tags, TW06 cross-references. Tools and reviewers depend on them.
   - No NEW comments where there were none (fresh commenting waits for 100%).
   - A function's prototype comment in a header counts as its comment (audit it with the function).
     A comment on a global or a block inside the unit's .c goes with the nearest function's ledger row.
     Struct FIELD comments in headers wait for phase 3 (headers).
   - Stale STATUS notes ("not yet decompiled", "only part is decompiled") are facts about our progress:
     correct them when the unit is plainly past that (all functions in C / unit Matching). They are
     not protected matching notes.
   - Files that use __LINE__ (Common/SharedFileIO, Common/TagFile, anything using SFIO_ASSERT): a
     removed comment line must be replaced by an EMPTY comment-free line, never deleted (write
     `new` as a single space). Lines must not move.
3. Write the batch (tab-separated; header rows allowed):
   - renames.tsv: `address current new tier codes evidence purpose` (only functions whose name changes;
     evidence cites both lanes: "L1: ...; L2: ...").
   - comments.tsv: `file old new` with the EXACT current comment text (`//...` or `/*...*/`,
     `\n` for line breaks) as it will read AFTER the renames; each `old` must occur exactly once in
     the file (grep it); `new` empty = remove.
   - ledger.tsv, one row per function of the unit, columns:
     `address unit name_before name_after tier decision comments lane1 lane2 reconciler date notes`
     tier T1/T2/T3/D/- ; decision kept/renamed/tier3/demoted/unnamed ; comments ok/none/fixed:N/stripped:N
     (lane1/lane2/reconciler/date may be left empty: the apply script fills them).
   - meta.txt: three lines: lane 1 (model, lane), lane 2 (model, lane), reconciler (model, lane).
4. Run `python tools/match/namecheck.py <batch>/renames.tsv` in your worktree (read-only) and fix or
   justify (`namecheck-ok: <why>` in evidence) every flag.
5. report.md, at most 10 lines: counts (kept / renamed / T3 / demoted / unnamed; comments ok / fixed /
   stripped), the worst wrong name or comment found, and any rule that was unclear.

## After 100%: the permanent rule (user, 2026-09-24)
The audit does not end the checking. Every NEW name or behaviour comment in EA game code needs
hard evidence (EA text; a TW06/TW07 name the code confirms) or two blind lanes + a reconciler, the
same as here. Matching lanes only ever write matching notes. (docs/style.md "Where names and
comments come from"; brief.md.) Measured on 3,048 audited functions: 380 of 1,931 comments (19.7%)
were wrong, ~7% of names.

## Orchestrator
- Spot-check: every demotion and every rename (read the code), 1 in 5 of the kept names, every
  rewritten comment in a __LINE__ file. Then
  `python tools/agents/audit_apply.py <batch dir> "<Unit> (<what it is in game terms>)"`.
- Apply between matching waves (never mid-merge); the renames reach lane branches through
  name_sources.tsv (rename.py --refs-only in merge.py).
- Report audit % (auditprogress.py) at every update to the user.
