/* Gecko C2 hook at 0x80025AD8 (fn_800258B4's shared exit, originally `lmw r24, 0x20(r1)`).
   r27 = the clip group asked for, r3 = the clip picked. For group 9 (the gimme tap-in), look
   through every player's animation library (gPlayers[i].nShotHandle -> +0x3D8; clip pointers at
   +0x120, count at +0x114; a clip's name at +0xA0) for "gplptt12" and return it instead. */
asm void PoolCueHook(void) {
    nofralloc
    cmpwi   r27, 9
    bne     done
    cmplwi  r3, 0
    beq     done
    lis     r4, 0x801C
    ori     r4, r4, 0x66E8          /* gPlayers */
    li      r5, 5                   /* player slots */
outer:
    lwz     r6, 0xC18(r4)           /* shot object */
    cmplwi  r6, 0
    beq     next
    lwz     r6, 0x3D8(r6)           /* animation library */
    cmplwi  r6, 0
    beq     next
    lwz     r7, 0x114(r6)           /* clip count */
    lwz     r8, 0x120(r6)           /* clip pointers */
    cmplwi  r8, 0
    beq     next
inner:
    cmpwi   r7, 0
    ble     next
    lwz     r9, 0(r8)
    cmplwi  r9, 0
    beq     skip
    lwz     r10, 0xA0(r9)
    lis     r11, 0x6770
    ori     r11, r11, 0x6C70        /* "gplp" */
    cmpw    r10, r11
    bne     skip
    lwz     r10, 0xA4(r9)
    lis     r11, 0x7474
    ori     r11, r11, 0x3132        /* "tt12" */
    cmpw    r10, r11
    bne     skip
    lbz     r10, 0xA8(r9)
    cmpwi   r10, 0
    bne     skip
    mr      r3, r9                  /* the pool-cue tap-in */
    b       done
skip:
    addi    r8, r8, 4
    subi    r7, r7, 1
    b       inner
next:
    addi    r4, r4, 0xEF8
    subic.  r5, r5, 1
    bne     outer
done:
    lmw     r24, 0x20(r1)           /* the instruction the hook replaces */
}
