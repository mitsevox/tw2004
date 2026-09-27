# Data linking pass, lane B (the .data blocks at or above 0x80190000)

Owner, evidence, result for each block of agents/assign/2026-09-27-datalink.md "Lane B".

| Block | Bytes | Owner | Evidence | Result |
|---|---|---|---|---|
| auto_05_80191440_data | 64 | CamSpline.c | `lbl_80191440` (f32[4][4] Catmull-Rom basis) used only by CamSpline fn_800C7480/fn_800C7898; sits between GoGolfCam's .data (ends 0x80191440) and GoBreakLine's (0x80191480), CamSpline links between them | linked, DOL OK |
| auto_05_80191520_data | 552 | AnimStream.c | `lbl_80191520` (s32[128]) and `lbl_80191720` (char[40]) used only by AnimStream fn_800CB700/fn_800CB868/fn_800CB8F0; contiguous with AnimStream's .data (ends 0x80191520); defined after fn_800CB668 so its strings stay first | linked, DOL OK |
| auto_05_801917D0_data | 568 | CourseData.c | the course-name table `lbl_80191990` (char*[30]) and its 28 strings; used by FE_MessageTable, EventInfo, GameMode22, GameMode4Menu, GameUICommands, FE_PGATourMessages (none of them next to it); contiguous with CourseData's .data (jump table, ends 0x801917D0); its two short names "Skillz"/"NA" are .sdata 0x80281550-0x80281560 (lane D's auto_07_80281550_sdata, 16 B, taken with it: same object), right after Calendar's .sdata (CourseData has no other .sdata); Earnings' data follows both. Defined at the end of CourseData.c (after fn_800D30B4's jump table). A separate data-only EA file between CourseData and Earnings would give the same bytes. | linked with the .sdata, DOL OK |
| auto_05_8019C988_data | 56 | runtime/locale.c (new unit, MSL, data only) | `__lconv` (MSL's locale.c, extern/sdk/runtime/locale.c, unchanged); used by strtold; sits between ctype.c's .data and printf.c's, MSL's locale.c links between FILE_POS.c and mbstring.c. Its strings "." and "" / "C" are .rodata 0x80185158-0x80185160 (lane A's auto_04_80185158_rodata, 8 B, taken with it: same object) | new Object in configure.py (MSL_C lib) + splits unit with .rodata and .data, DOL OK |
