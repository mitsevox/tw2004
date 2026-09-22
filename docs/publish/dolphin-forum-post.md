Forum post draft - Dolphin forums, "Cheats, Hacks, & Game Patches"
=====================================================================

Status: posted 2026-09-22 under the forum handle "mits", awaiting moderator approval. Thread URL: (add when live)

Title
-----

[GC] Tiger Woods PGA Tour 2004 (GW4E69) - Widescreen culling fix (Gecko)

Body
----

Tiger Woods PGA Tour 2004 has no widescreen support and no widescreen code that I could find,
on the wiki or anywhere else. Dolphin's widescreen hack gets you a 16:9 view, but the game
still culls against its 4:3 view, so trees, grass and objects cut out in the outer ~12.5% of the
screen on each side whenever the camera moves. This code fixes that.

[code]
$Widescreen Culling Fix [mits]
04007EC0 3BE00004
04007EF0 3BE00004
[/code]

Game: Tiger Woods PGA Tour 2004 (USA), ID GW4E69, Rev 0. Both discs share the same main.dol,
so the code works on either disc. It will NOT work on the PAL release (GW4P69) - different binary,
different addresses.

How to use:
- Enable cheats (Config > General).
- Game properties > Gecko Codes > tick the code.
- Game properties > Graphics > Enhancements: tick Widescreen Hack. Set aspect ratio to Auto or
  Stretch to Window.

What it does: the game has one function that tests an object's bounding sphere against the
camera's view volume and returns "inside", "outside", or "touching an edge". The code changes the
result of the left and right edge tests from "outside" to "touching an edge", so objects in the
extra widescreen area are still drawn. Near, far, top and bottom culling are untouched, so this
is not a blanket "draw everything" patch. Each line replaces one instruction (li r31,2 -> li r31,4);
the original word at both addresses is 3BE00002 if you want to verify.

What it does not do: the menus and HUD stay stretched, same as any widescreen-hack setup.

Setup: Dolphin 2606a (official build from dolphin-emu.org), Vulkan backend. Windows 11 Pro 24H2,
Intel Core i7-12700K, NVIDIA GeForce RTX 3080, 64 GB DDR5. Default settings apart from the
widescreen hack and the code above.

Testing: A/B on that setup. Code off with the widescreen hack on:
obvious pop-in at both screen edges on any camera movement. Code on: full 16:9, no pop-in, no
glitches in normal play, hole flyovers and replays, on both discs. I have not played every course
yet, so if you see anything odd please post the course and hole.

How it was found: I'm new to this. I started a decompilation project for the game and this was
the first function we got to match byte for byte, so the patch comes from reading the actual C
rather than searching memory. Notes and the decompiled function are here: <repo link>

If a wiki editor wants to add this to the game's page, please do.
