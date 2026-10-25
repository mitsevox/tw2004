#include "dolphin/DVDPriv.h"
#include "dolphin/os.h"
#include "dolphin/os/OSBootInfo.h"

typedef struct FSTEntry FSTEntry;

struct FSTEntry {
  unsigned int isDirAndStringOff;
  unsigned int parentOrPosition;
  unsigned int nextEntryOrLength;
};

static OSBootInfo* BootInfo;
static FSTEntry* FstStart;
static char* FstStringStart;
static u32 MaxEntryNum;
static u32 currentDirectory = 0;
OSThreadQueue __DVDThreadQueue;
u32 __DVDLongFileNameFlag = 0;

static void cbForReadAsync(s32 result, DVDCommandBlock* block);
static void cbForReadSync(s32 result, DVDCommandBlock* block);
static void cbForSeekAsync(s32 result, DVDCommandBlock* block);
static void cbForSeekSync(s32 result, DVDCommandBlock* block);
static void cbForPrepareStreamAsync(s32 result, DVDCommandBlock* block);
static void cbForPrepareStreamSync(s32 result, DVDCommandBlock* block);

void __DVDFSInit() {
  BootInfo = (OSBootInfo*)OSPhysicalToCached(0);
  FstStart = (FSTEntry*)BootInfo->FSTLocation;

  if (FstStart) {
    MaxEntryNum = FstStart[0].nextEntryOrLength;
    FstStringStart = (char*)&(FstStart[MaxEntryNum]);
  }
}

/* For convenience */
#define entryIsDir(i) (((FstStart[i].isDirAndStringOff & 0xff000000) == 0) ? FALSE : TRUE)
#define stringOff(i) (FstStart[i].isDirAndStringOff & ~0xff000000)
#define parentDir(i) (FstStart[i].parentOrPosition)
#define nextDir(i) (FstStart[i].nextEntryOrLength)
#define filePosition(i) (FstStart[i].parentOrPosition)
#define fileLength(i) (FstStart[i].nextEntryOrLength)

static BOOL isSame(const char* path, const char* string) {
  while (*string != '\0') {
    if (tolower(*path++) != tolower(*string++)) {
      return FALSE;
    }
  }

  if ((*path == '/') || (*path == '\0')) {
    return TRUE;
  }

  return FALSE;
}

s32 DVDConvertPathToEntrynum(char* pathPtr) {
  const char* ptr;
  char* stringPtr;
  BOOL isDir;
  u32 length;
  u32 dirLookAt;
  u32 i;
  const char* origPathPtr = pathPtr;
  const char* extentionStart;
  BOOL illegal;
  BOOL extention;

  dirLookAt = currentDirectory;

  while (1) {

    if (*pathPtr == '\0') {
      return (s32)dirLookAt;
    } else if (*pathPtr == '/') {
      dirLookAt = 0;
      pathPtr++;
      continue;
    } else if (*pathPtr == '.') {
      if (*(pathPtr + 1) == '.') {
        if (*(pathPtr + 2) == '/') {
          dirLookAt = parentDir(dirLookAt);
          pathPtr += 3;
          continue;
        } else if (*(pathPtr + 2) == '\0') {
          return (s32)parentDir(dirLookAt);
        }
      } else if (*(pathPtr + 1) == '/') {
        pathPtr += 2;
        continue;
      } else if (*(pathPtr + 1) == '\0') {
        return (s32)dirLookAt;
      }
    }

    if (__DVDLongFileNameFlag == 0) {
      extention = FALSE;
      illegal = FALSE;

      for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++) {
        if (*ptr == '.') {
          if ((ptr - pathPtr > 8) || (extention == TRUE)) {
            illegal = TRUE;
            break;
          }
          extention = TRUE;
          extentionStart = ptr + 1;

        } else if (*ptr == ' ')
          illegal = TRUE;
      }

      if ((extention == TRUE) && (ptr - extentionStart > 3))
        illegal = TRUE;

      if (illegal)
        OSPanic(__FILE__, 379,
                "DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): "
                "specified directory or file (%s) doesn't match standard 8.3 format. This is a "
                "temporary restriction and will be removed soon\n",
                origPathPtr);
    } else {
      for (ptr = pathPtr; (*ptr != '\0') && (*ptr != '/'); ptr++)
        ;
    }

    isDir = (*ptr == '\0') ? FALSE : TRUE;
    length = (u32)(ptr - pathPtr);

    ptr = pathPtr;

    for (i = dirLookAt + 1; i < nextDir(dirLookAt); i = entryIsDir(i) ? nextDir(i) : (i + 1)) {
      if ((entryIsDir(i) == FALSE) && (isDir == TRUE)) {
        continue;
      }

      stringPtr = FstStringStart + stringOff(i);

      if (isSame(ptr, stringPtr) == TRUE) {
        goto next_hier;
      }
    }

    return -1;

  next_hier:
    if (!isDir) {
      return (s32)i;
    }

    dirLookAt = i;
    pathPtr += length + 1;
  }
}

BOOL DVDFastOpen(s32 entrynum, DVDFileInfo* fileInfo) {
  if ((entrynum < 0) || (entrynum >= MaxEntryNum) || entryIsDir(entrynum)) {
    return FALSE;
  }

  fileInfo->startAddr = filePosition(entrynum);
  fileInfo->length = fileLength(entrynum);
  fileInfo->callback = (DVDCallback)NULL;
  fileInfo->cb.state = DVD_STATE_END;

  return TRUE;
}

BOOL DVDClose(DVDFileInfo* fileInfo) {
  DVDCancel(&(fileInfo->cb));
  return TRUE;
}

BOOL DVDReadAsyncPrio(DVDFileInfo* fileInfo, void* addr, s32 length, s32 offset,
                      DVDCallback callback, s32 prio) {

  if (!((0 <= offset) && (offset < fileInfo->length))) {
    OSPanic(__FILE__, 742, "DVDReadAsync(): specified area is out of the file  ");
  }

  if (!((0 <= offset + length) && (offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE))) {
    OSPanic(__FILE__, 748, "DVDReadAsync(): specified area is out of the file  ");
  }

  fileInfo->callback = callback;
  DVDReadAbsAsyncPrio(&(fileInfo->cb), addr, length, (s32)(fileInfo->startAddr + offset),
                      cbForReadAsync, prio);

  return TRUE;
}
#ifndef offsetof
#define offsetof(type, memb) ((u32) & ((type*)0)->memb)
#endif

static void cbForReadAsync(s32 result, DVDCommandBlock* block) {
  DVDFileInfo* fileInfo;

  fileInfo = (DVDFileInfo*)((char*)block - offsetof(DVDFileInfo, cb));
  if (fileInfo->callback) {
    (fileInfo->callback)(result, fileInfo);
  }
}


#define RoundUp32KB(x) (((u32)(x) + 32 * 1024 - 1) & ~(32 * 1024 - 1))
#define Is32KBAligned(x) (((u32)(x) & (32 * 1024 - 1)) == 0)

