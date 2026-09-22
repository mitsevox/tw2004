// EA shared file library (GCC / ProDG, no optimization). Original file name unknown; it sits
// between ChecksumCRC32.c and SharedFileIO.c in link order and has no assert strings.
// A trivial XOR cipher: every byte is XORed with the next byte of a cycling key.

enum {
    CIPHER_SHUTDOWN = 4,   // also returned as an error from SetKey when not initialised
    CIPHER_KEY_SET = 5,
    CIPHER_INITIALISED = 6,
    CIPHER_ENCODED = 7,
    CIPHER_DECODED = 8
};

// Four separate globals, not a struct: each is small enough for the small-data section, which
// is how the binary addresses them.
static const unsigned char* gKeyStart = 0;
static const unsigned char* gKeyEnd = 0;      // last byte of the key
static const unsigned char* gKeyCursor = 0;
static int gState = CIPHER_SHUTDOWN;

int Cipher_Init(void* unused);
int Cipher_Shutdown(void);
int Cipher_IsInitialised(void);
int Cipher_SetKey(const unsigned char* key, unsigned long len);
int Cipher_Encode(unsigned char* data, unsigned long len);
int Cipher_Decode(unsigned char* data, unsigned long len);
int Cipher_Reset(void);

static void* const gCipherInterface[7] = {
    (void*)Cipher_Init,
    (void*)Cipher_Shutdown,
    (void*)Cipher_IsInitialised,
    (void*)Cipher_SetKey,
    (void*)Cipher_Encode,
    (void*)Cipher_Decode,
    (void*)Cipher_Reset,
};

int Cipher_Init(void* unused) {
    if (gState != CIPHER_SHUTDOWN) return 3;
    gState = CIPHER_INITIALISED;
    return 0;
}

int Cipher_Shutdown(void) {
    gKeyStart = 0;
    gKeyEnd = 0;
    gKeyCursor = 0;
    gState = CIPHER_SHUTDOWN;
    return 0;
}

int Cipher_IsInitialised(void) {
    return gState != CIPHER_SHUTDOWN;
}

int Cipher_SetKey(const unsigned char* key, unsigned long len) {
    if (key == 0 || len == 0) return 2;
    if (gState == CIPHER_SHUTDOWN) return gState;
    gKeyStart = key;
    gKeyEnd = key + len - 1;
    gKeyCursor = key;
    gState = CIPHER_KEY_SET;
    return 0;
}

int Cipher_Encode(unsigned char* data, unsigned long len) {
    unsigned long i;
    if (data == 0 || len == 0) return 2;
    if (gState != CIPHER_KEY_SET && gState != CIPHER_ENCODED) return gState;
    i = 0;
    while (i < len) {
        data[i] = data[i] ^ *gKeyCursor;
        if (gKeyCursor == gKeyEnd) gKeyCursor = gKeyStart;
        else gKeyCursor++;
        i++;
    }
    gState = CIPHER_ENCODED;
    return 0;
}

int Cipher_Decode(unsigned char* data, unsigned long len) {
    unsigned long i;
    if (data == 0 || len == 0) return 2;
    if (gState != CIPHER_KEY_SET && gState != CIPHER_DECODED) return gState;
    i = 0;
    while (i < len) {
        data[i] = data[i] ^ *gKeyCursor;
        if (gKeyCursor == gKeyEnd) gKeyCursor = gKeyStart;
        else gKeyCursor++;
        i++;
    }
    gState = CIPHER_DECODED;
    return 0;
}

int Cipher_Reset(void) {
    if (gState == CIPHER_KEY_SET || gState == CIPHER_ENCODED || gState == CIPHER_DECODED) {
        gKeyCursor = gKeyStart;
        gState = CIPHER_KEY_SET;
    }
    return 0;
}

void* const* Cipher_GetInterface(void) {
    return gCipherInterface;
}
