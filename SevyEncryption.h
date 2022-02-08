/**
 * @file SevyEncryption.h
 * @author Yves Yuri (yyosilva@hotmail.com)
 * @brief
 * @version 0.1.0
 * @date 2022-01-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef _SevyEncryption_h
#define _SevyEncryption_h

#include <Arduino.h>
#include "SevyEncryption.h"
#include "SevyEncryptionBytes.h"

#define N_ROW 4
#define N_COL 4
#define N_BLOCK (N_ROW * N_COL)
#define N_MAX_ROUNDS 14
#define KEY_SCHEDULE_BYTES ((N_MAX_ROUNDS + 1) * N_BLOCK)
#define SUCCESS (0)
#define FAILURE (-1)

class SevyEncryption
{

private:
    int round;

    byte key[16];
    byte encOutput[16];
    byte decOutput[32];
    byte encInput[32];
    byte decInput[16];

    byte setKey(byte key[]);
    byte encryptAES128Ecb(byte plain[N_BLOCK], byte cipher[N_BLOCK]);
    byte decryptAES128Ecb(byte cipher[N_BLOCK], byte plain[N_BLOCK]);

    void copyBytes(byte *AESt, byte *src, byte n);
    void clean();

    static byte sBox(byte x);
    static byte isBox(byte x);
    static void shiftSubRows(byte st[N_BLOCK]);
    static void copyKey(byte *d, byte *s, byte *k);
    static void invShiftSubRows(byte st[N_BLOCK]);
    static void mixSubColumns(byte dt[N_BLOCK], byte st[N_BLOCK]);
    static void invMixSubColumns(byte dt[N_BLOCK], byte st[N_BLOCK]);

    byte keySched[KEY_SCHEDULE_BYTES];
    byte arrPad[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};

public:
    String encrypt(String plainText, const char* pass);
    String decrypt(String encryptedText, const char* pass); 
};

#endif
