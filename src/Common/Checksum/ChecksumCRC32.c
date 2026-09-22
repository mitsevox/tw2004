// EA shared file library, built with SN ProDG (GCC 2.95) without optimization.
// Original path (from the assert string in the binary): ../../../Source/Common/Checksum/ChecksumCRC32.c
// Function and field names are ours; the original names are unknown.

typedef struct {
    unsigned long size;
    void*         data;
} CRC32Result;

typedef struct {
    unsigned long* table;      // 256-entry lookup table, allocated in CRC32_Init
    CRC32Result    result;
    unsigned long  crc;        // running value, starts at 0xFFFFFFFF
    void*          allocator;
} CRC32State;

// Memory functions of the host game (CodeWarrior side). Signatures inferred from the calls.
extern void* fn_801220D4(void* allocator, unsigned long size, unsigned long align, const char* file, int line);
extern void  fn_80122128(void* allocator, void* p, unsigned long size, unsigned long align);

CRC32State gCRC = { 0, { 4, 0 }, 0xFFFFFFFF, 0 };   // read directly by SharedFileIO.c, so not static
static unsigned long gCRCResult;

unsigned long CRC32_Entry(unsigned long index) {
    unsigned char k;
    unsigned long c = index;
    for (k = 8; k != 0; k--) {
        if (c & 1) c = (c >> 1) ^ 0xEDB88320;
        else       c = c >> 1;
    }
    return c;
}

void CRC32_BuildTable(unsigned long* table) {
    unsigned long i;
    for (i = 0; i <= 255; i++) table[i] = CRC32_Entry(i);
}

int CRC32_Init(void* allocator) {
    if (gCRC.table != 0) return 5;
#line 214 "../../../Source/Common/Checksum/ChecksumCRC32.c"
    gCRC.table = fn_801220D4(allocator, 1024, 4, __FILE__, __LINE__);
    if (gCRC.table == 0) return 1;
    gCRC.allocator = allocator;
    CRC32_BuildTable(gCRC.table);
    gCRC.crc = 0xFFFFFFFF;
    gCRC.result.size = 4;
    gCRC.result.data = 0;
    return 0;
}

int CRC32_Shutdown(void) {
    if (gCRC.table == 0) return 6;
    fn_80122128(gCRC.allocator, gCRC.table, 1024, 4);
    gCRC.table = 0;
    return 0;
}

int CRC32_IsInitialised(void) {
    return gCRC.table != 0;
}

int CRC32_Reset(void) {
    if (gCRC.table == 0) return 6;
    gCRC.crc = 0xFFFFFFFF;
    return 0;
}

int CRC32_Update(const unsigned char* data, unsigned long len) {
    unsigned long i;
    unsigned long idx;
    unsigned long crc = gCRC.crc;
    if (gCRC.table == 0) return 6;
    if (data == 0) return 2;
    // A `for` loop here makes GCC 2.95 -O0 emit a stray nop before it; a while loop does not.
    i = 0;
    while (i < len) {
        idx = (data[i] ^ crc) & 0xFF;
        crc = ((crc >> 8) & 0x00FFFFFF) ^ gCRC.table[idx];
        i++;
    }
    gCRC.crc = crc;
    return 0;
}

int CRC32_Finalise(CRC32Result** out) {
    if (gCRC.table == 0) return 6;
    if (out == 0) return 2;
    gCRCResult = ~gCRC.crc;
    gCRC.result.data = &gCRCResult;
    gCRC.result.size = 4;
    *out = &gCRC.result;
    return 0;
}
