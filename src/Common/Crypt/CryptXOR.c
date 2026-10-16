// CryptXOR.c: the XOR cipher of EA's shared file library, which the tag file uses to scramble each
// record of the EA Sports Bio save and its header: every byte is XORed with the next byte of a
// repeating key.

#include "Common/TagFile.h"

static const u8* _CryptXOR_pKeyStart = 0;
static const u8* _CryptXOR_pKeyEnd = 0;   // the key's last byte
static const u8* _CryptXOR_pKeyCursor = 0;   // the key byte the next data byte is XORed with
static CryptErrorE _CryptXOR_Status = CRYPT_ERROR_NOTINITIALIZED;

static CryptErrorE _CryptXOR_Init(void* pAllocator);
static CryptErrorE _CryptXOR_Shutdown(void);
static u8 _CryptXOR_IsInitialized(void);
static CryptErrorE _CryptXOR_Start(const u8* pKey, u32 uKeySize);
static CryptErrorE _CryptXOR_Encrypt(void* pBuffer, u32 uBufferSize);
static CryptErrorE _CryptXOR_Decrypt(void* pBuffer, u32 uBufferSize);
static CryptErrorE _CryptXOR_Stop(void);

static const CryptTypeT _CryptXOR_Type = {
    _CryptXOR_Init,
    _CryptXOR_Shutdown,
    _CryptXOR_IsInitialized,
    _CryptXOR_Start,
    _CryptXOR_Encrypt,
    _CryptXOR_Decrypt,
    _CryptXOR_Stop,
};

// Readies the cipher for a key. Nothing is allocated, so the allocator goes unused.
static CryptErrorE _CryptXOR_Init(void* pAllocator) {
    if (_CryptXOR_Status != CRYPT_ERROR_NOTINITIALIZED) return CRYPT_ERROR_INITIALIZED;
    _CryptXOR_Status = CRYPT_ERROR_NOTSTARTED;
    return CRYPT_ERROR_NONE;
}

// Forgets the key and leaves the cipher uninitialised; always succeeds.
static CryptErrorE _CryptXOR_Shutdown(void) {
    _CryptXOR_pKeyStart = 0;
    _CryptXOR_pKeyEnd = 0;
    _CryptXOR_pKeyCursor = 0;
    _CryptXOR_Status = CRYPT_ERROR_NOTINITIALIZED;
    return CRYPT_ERROR_NONE;
}

static u8 _CryptXOR_IsInitialized(void) {
    return _CryptXOR_Status != CRYPT_ERROR_NOTINITIALIZED;
}

// Takes the key the data is XORed with and starts at its first byte. The key is not copied, so it
// must outlive the encryption. Calling it again replaces the key.
static CryptErrorE _CryptXOR_Start(const u8* pKey, u32 uKeySize) {
    if (pKey == 0 || uKeySize == 0) return CRYPT_ERROR_INVALIDPARAMETERS;
    if (_CryptXOR_Status == CRYPT_ERROR_NOTINITIALIZED) return _CryptXOR_Status;
    _CryptXOR_pKeyStart = pKey;
    _CryptXOR_pKeyEnd = pKey + uKeySize - 1;
    _CryptXOR_pKeyCursor = pKey;
    _CryptXOR_Status = CRYPT_ERROR_STARTED;
    return CRYPT_ERROR_NONE;
}

// XORs each byte of the buffer with the next byte of the key, going back to the key's first byte
// after its last. The place in the key carries over to the next call, so data can be encrypted in
// pieces. Refused, with the current status, unless the key was just started or the last call also
// encrypted.
static CryptErrorE _CryptXOR_Encrypt(void* pBuffer, u32 uBufferSize) {
    u32 uOffset;
    if (pBuffer == 0 || uBufferSize == 0) return CRYPT_ERROR_INVALIDPARAMETERS;
    if (_CryptXOR_Status != CRYPT_ERROR_STARTED && _CryptXOR_Status != CRYPT_ERROR_ENCRYPTING) {
        return _CryptXOR_Status;
    }
    uOffset = 0;
    while (uOffset < uBufferSize) {
        ((u8*)pBuffer)[uOffset] = ((u8*)pBuffer)[uOffset] ^ *_CryptXOR_pKeyCursor;
        if (_CryptXOR_pKeyCursor == _CryptXOR_pKeyEnd) {
            _CryptXOR_pKeyCursor = _CryptXOR_pKeyStart;
        } else {
            _CryptXOR_pKeyCursor++;
        }
        uOffset++;
    }
    _CryptXOR_Status = CRYPT_ERROR_ENCRYPTING;
    return CRYPT_ERROR_NONE;
}

// The same XOR as _CryptXOR_Encrypt, which undoes it. Refused, with the current status, unless the
// key was just started or the last call also decrypted.
static CryptErrorE _CryptXOR_Decrypt(void* pBuffer, u32 uBufferSize) {
    u32 uOffset;
    if (pBuffer == 0 || uBufferSize == 0) return CRYPT_ERROR_INVALIDPARAMETERS;
    if (_CryptXOR_Status != CRYPT_ERROR_STARTED && _CryptXOR_Status != CRYPT_ERROR_DECRYPTING) {
        return _CryptXOR_Status;
    }
    uOffset = 0;
    while (uOffset < uBufferSize) {
        ((u8*)pBuffer)[uOffset] = ((u8*)pBuffer)[uOffset] ^ *_CryptXOR_pKeyCursor;
        if (_CryptXOR_pKeyCursor == _CryptXOR_pKeyEnd) {
            _CryptXOR_pKeyCursor = _CryptXOR_pKeyStart;
        } else {
            _CryptXOR_pKeyCursor++;
        }
        uOffset++;
    }
    _CryptXOR_Status = CRYPT_ERROR_DECRYPTING;
    return CRYPT_ERROR_NONE;
}

// Ends a run of encrypting or decrypting: the key goes back to its first byte, ready for the next
// buffer. Does nothing when no key is set; always succeeds.
static CryptErrorE _CryptXOR_Stop(void) {
    if (_CryptXOR_Status == CRYPT_ERROR_STARTED || _CryptXOR_Status == CRYPT_ERROR_ENCRYPTING
        || _CryptXOR_Status == CRYPT_ERROR_DECRYPTING) {
        _CryptXOR_pKeyCursor = _CryptXOR_pKeyStart;
        _CryptXOR_Status = CRYPT_ERROR_STARTED;
    }
    return CRYPT_ERROR_NONE;
}

// The cipher's functions as a table, which EASBStorage.c hands to TagFile_Init to encrypt the
// save's records.
const CryptTypeT* CryptXORQueryType(void) {
    return &_CryptXOR_Type;
}
