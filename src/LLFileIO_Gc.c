// LLFileIO_Gc.c (EA's name, from its asserts): the disc file reads. Only the path conversion and an
// empty function are decompiled so far.
#include "game_types.h"

void fn_80005BE8(const char* szSrc, char* szDst);
void fn_800060DC(void);

// Copies a file path in the disc's form: letters upper case, backslashes as slashes.
void fn_80005BE8(const char* szSrc, char* szDst) {
    while (*szSrc != 0) {
        if (*szSrc >= 'a' && *szSrc <= 'z') {
            *szDst = *szSrc - ('a' - 'A');
        } else if (*szSrc == '\\') {
            *szDst = '/';
        } else {
            *szDst = *szSrc;
        }
        szSrc++;
        szDst++;
    }
    *szDst = 0;
}

void fn_800060DC(void) {
}
