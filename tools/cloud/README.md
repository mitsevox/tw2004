# Building in a cloud session

The game's `main.dol` never goes in git. A cloud session gets it from the same private build
container CI uses, with a read-only token the owner creates.

## Owner: one-time (the session never sees the token's value)

1. GitHub -> Settings -> Developer settings -> Personal access tokens -> Tokens (classic) ->
   Generate new token (classic). Scope: `read:packages` only. Give it an expiry you are happy with.
2. In the cloud environment's settings (claude.ai/code -> the environment for this repo ->
   environment variables / secrets), add `TW_BUILD_TOKEN` = that token.

## Session

```
bash tools/cloud/setup.sh      # main.dol, compilers (wibo), decomp-permuter beside the repo, build
```
It must end with `build/GW4E69/main.dol: OK`. `ninja -t clean` ends with "Directory not empty" for
`build/compilers` and `build/binutils`: harmless, the game objects are gone and the downloaded tools
stay.

`tools/build/hosttools.py` holds everything that differs between Windows and Linux (tool paths,
wibo/wine, how the compiler reports paths, stopping a process tree), so every tool runs unchanged
on both.
