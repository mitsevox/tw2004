# Data linking pass, lane B (the .data blocks at or above 0x80190000)

Owner, evidence, result for each block of agents/assign/2026-09-27-datalink.md "Lane B".

| Block | Bytes | Owner | Evidence | Result |
|---|---|---|---|---|
| auto_05_80191440_data | 64 | CamSpline.c | `lbl_80191440` (f32[4][4] Catmull-Rom basis) used only by CamSpline fn_800C7480/fn_800C7898; sits between GoGolfCam's .data (ends 0x80191440) and GoBreakLine's (0x80191480), CamSpline links between them | linked, DOL OK |
| auto_05_80191520_data | 552 | AnimStream.c | `lbl_80191520` (s32[128]) and `lbl_80191720` (char[40]) used only by AnimStream fn_800CB700/fn_800CB868/fn_800CB8F0; contiguous with AnimStream's .data (ends 0x80191520); defined after fn_800CB668 so its strings stay first | linked, DOL OK |
