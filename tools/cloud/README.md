# Running this project in a cloud session

Everything the agents use is in the repo (`CLAUDE.md`, `agents/`, `docs/`, `tools/`), except the
game's `main.dol`, which never goes in git. A cloud session gets it from the same private build
container CI uses, with a read-only token the owner creates.

## Owner: one-time (the session never sees the token's value)

1. GitHub -> Settings -> Developer settings -> Personal access tokens -> Tokens (classic) ->
   Generate new token (classic). Scope: `read:packages` only. Give it an expiry you are happy with.
2. In the cloud environment's settings (claude.ai/code -> the environment for this repo ->
   environment variables / secrets), add `TW_BUILD_TOKEN` = that token.
3. Start a session on the repo and tell it: "run tools/cloud/setup.sh, then follow CLAUDE.md".

## Session: setup and a speed test

```
bash tools/cloud/setup.sh      # main.dol, compilers (wibo), permuter + m2c beside the repo, build
```
It must end with `build/GW4E69/main.dol: OK`. Then time, and record in `agents/state.md`: a full
build from clean (`ninja -t clean && time ninja`), one `trial.py` loop on a near-miss, and a
5-minute `permute.py` run (iterations reported). Locally (Windows, 20 cores) a full build took
about a minute; the first cloud box (4 cores) took 27 s. `ninja -t clean` ends with "Directory not
empty" for `build/compilers` and `build/binutils`: harmless, the game objects are gone and the
downloaded tools stay.

## Notes

- Lanes: `python tools/agents/new_agent.py <lane>` makes worktrees in `../tw2004-agents/` with
  symlinks to the main checkout's compilers. Scratch folders are `../scratch/tw/agents/<lane>/`:
  they vanish with the session, so the orchestrator copies lane notes into `agents/notes/` before
  the session ends.
- Some `tools/match/` scripts still call Windows `.exe` paths directly; making them Linux-clean is
  the first task in `agents/state.md`.
- Commits: plain messages, never a Co-Authored-By line or AI footer (CLAUDE.md).
