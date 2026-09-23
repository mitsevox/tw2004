#ifndef COMMON_SHAREDFILEIO_H
#define COMMON_SHAREDFILEIO_H

// EA shared file library (memory-card save system). Names with the SFIO prefix and Hungarian
// notation come from assert strings in the binary; everything else is ours.

#include "game_types.h"

typedef u8             BOOL;   // byte-sized: SFIOIsInitialized() is truncated to 8 bits after use
#define TRUE  1
#define FALSE 0

enum {
    SFIO_DEVICE_FIRST = 0,
    SFIO_DEVICE_LAST = 1,
    SFIO_DEVICE_INVALID = -1
};
#define SFIO_DEVICE_MASK(e) ((u16)(1 << (u16)(e)))

// Retail build: the assert handler is a 4-byte stub, but every call site survives.
extern void fn_8012214C(const char* pFile, int uLine, const char* pExpr, int unused);
#define SFIO_ASSERT(cond) { u8 bSkip = 0; if (!((cond) | bSkip)) fn_8012214C(__FILE__, __LINE__, #cond, 0); }

BOOL SFIOIsInitialized(void);

#endif
