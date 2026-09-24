# Findings pending for decomp-notes (fill-in run CP1)
## map-07
- cmpwi on a param compare only when both param and the struct field it's stored to are int (GoARAM fn_800B65C0 99.33 u32 / 97.22 int param only / 100 both int) verified
- chained `p->a->pPrev = p->a->pNext = p->a;` = load, store self, reload, store (GoARAM fn_800B5C40 99.6->100) verified
- popping list head: `pHeap->pSpare = pHeap->pSpare->pNext` (reads through head field again, not local) (fn_800B5F4C 92.8->100) verified
- value compared inside loop computed into a local first statement before the loop (fn_800B5E88 94.3->100) verified
- tool: unit .data range ends at its own 8-byte alignment, not next object's 32-byte align (GoARAM link) verified
- porting: fn_800B67EC reads r3 left by fn_800B6844/fn_800B68B4 (callers pass nothing) observed -> assigned map-07
