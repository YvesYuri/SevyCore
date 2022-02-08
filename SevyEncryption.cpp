/**
 * @file SevyEncryption.cpp
 * @author Yves Yuri (yyosilva@hotmail.com)
 * @brief
 * @version 0.1.0
 * @date 2022-01-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "SevyEncryption.h"

#define WPOLY 0x011B
#define DPOLY 0x008D

#define f2(x) ((x)&0x80 ? (x << 1) ^ WPOLY : x << 1)
#define d2(x) (((x) >> 1) ^ ((x)&1 ? DPOLY : 0))

byte SevyEncryption::sBox(byte x)
{
  return pgm_read_byte(&sFwd[x]);
}

byte SevyEncryption::isBox(byte x)
{
  return pgm_read_byte(&sInv[x]);
}

void SevyEncryption::copyKey(byte *d, byte *s, byte *k)
{
  for (byte i = 0; i < N_BLOCK; i += 4)
  {
    *d++ = *s++ ^ *k++;
    *d++ = *s++ ^ *k++;
    *d++ = *s++ ^ *k++;
    *d++ = *s++ ^ *k++;
  }
}

void SevyEncryption::shiftSubRows(byte st[N_BLOCK])
{
  st[0] = sBox(st[0]);
  st[4] = sBox(st[4]);
  st[8] = sBox(st[8]);
  st[12] = sBox(st[12]);

  byte tt = st[1];
  st[1] = sBox(st[5]);
  st[5] = sBox(st[9]);
  st[9] = sBox(st[13]);
  st[13] = sBox(tt);

  tt = st[2];
  st[2] = sBox(st[10]);
  st[10] = sBox(tt);
  tt = st[6];
  st[6] = sBox(st[14]);
  st[14] = sBox(tt);

  tt = st[15];
  st[15] = sBox(st[11]);
  st[11] = sBox(st[7]);
  st[7] = sBox(st[3]);
  st[3] = sBox(tt);
}

void SevyEncryption::invShiftSubRows(byte st[N_BLOCK])
{
  st[0] = isBox(st[0]);
  st[4] = isBox(st[4]);
  st[8] = isBox(st[8]);
  st[12] = isBox(st[12]);

  byte tt = st[13];
  st[13] = isBox(st[9]);
  st[9] = isBox(st[5]);
  st[5] = isBox(st[1]);
  st[1] = isBox(tt);

  tt = st[2];
  st[2] = isBox(st[10]);
  st[10] = isBox(tt);
  tt = st[6];
  st[6] = isBox(st[14]);
  st[14] = isBox(tt);

  tt = st[3];
  st[3] = isBox(st[7]);
  st[7] = isBox(st[11]);
  st[11] = isBox(st[15]);
  st[15] = isBox(tt);
}

void SevyEncryption::mixSubColumns(byte dt[N_BLOCK], byte st[N_BLOCK])
{
  byte j = 5;
  byte k = 10;
  byte l = 15;
  for (byte i = 0; i < N_BLOCK; i += N_COL)
  {
    byte a = st[i];
    byte b = st[j];
    j = (j + N_COL) & 15;
    byte c = st[k];
    k = (k + N_COL) & 15;
    byte d = st[l];
    l = (l + N_COL) & 15;
    byte a1 = sBox(a), b1 = sBox(b), c1 = sBox(c), d1 = sBox(d);
    byte a2 = f2(a1), b2 = f2(b1), c2 = f2(c1), d2 = f2(d1);
    dt[i] = a2 ^ b2 ^ b1 ^ c1 ^ d1;
    dt[i + 1] = a1 ^ b2 ^ c2 ^ c1 ^ d1;
    dt[i + 2] = a1 ^ b1 ^ c2 ^ d2 ^ d1;
    dt[i + 3] = a2 ^ a1 ^ b1 ^ c1 ^ d2;
  }
}

void SevyEncryption::invMixSubColumns(byte dt[N_BLOCK], byte st[N_BLOCK])
{
  for (byte i = 0; i < N_BLOCK; i += N_COL)
  {
    byte a1 = st[i];
    byte b1 = st[i + 1];
    byte c1 = st[i + 2];
    byte d1 = st[i + 3];
    byte a2 = f2(a1), b2 = f2(b1), c2 = f2(c1), d2 = f2(d1);
    byte a4 = f2(a2), b4 = f2(b2), c4 = f2(c2), d4 = f2(d2);
    byte a8 = f2(a4), b8 = f2(b4), c8 = f2(c4), d8 = f2(d4);
    byte a9 = a8 ^ a1, b9 = b8 ^ b1, c9 = c8 ^ c1, d9 = d8 ^ d1;
    byte ac = a8 ^ a4, bc = b8 ^ b4, cc = c8 ^ c4, dc = d8 ^ d4;

    dt[i] = isBox(ac ^ a2 ^ b9 ^ b2 ^ cc ^ c1 ^ d9);
    dt[(i + 5) & 15] = isBox(a9 ^ bc ^ b2 ^ c9 ^ c2 ^ dc ^ d1);
    dt[(i + 10) & 15] = isBox(ac ^ a1 ^ b9 ^ cc ^ c2 ^ d9 ^ d2);
    dt[(i + 15) & 15] = isBox(a9 ^ a2 ^ bc ^ b1 ^ c9 ^ dc ^ d2);
  }
}

byte SevyEncryption::setKey(byte key[])
{
  byte hi;
  int keylen = 16;
  round = 10;
  hi = (round + 1) << 4;
  copyBytes(keySched, key, keylen);
  byte t[4];
  byte next = keylen;
  for (byte cc = keylen, rc = 1; cc < hi; cc += N_COL)
  {
    for (byte i = 0; i < N_COL; i++)
      t[i] = keySched[cc - 4 + i];
    if (cc == next)
    {
      next += keylen;
      byte ttt = t[0];
      t[0] = sBox(t[1]) ^ rc;
      t[1] = sBox(t[2]);
      t[2] = sBox(t[3]);
      t[3] = sBox(ttt);
      rc = f2(rc);
    }
    else if (keylen == 32 && (cc & 31) == 16)
    {
      for (byte i = 0; i < 4; i++)
        t[i] = sBox(t[i]);
    }
    byte tt = cc - keylen;
    for (byte i = 0; i < N_COL; i++)
      keySched[cc + i] = keySched[tt + i] ^ t[i];
  }
  return SUCCESS;
}

void SevyEncryption::clean()
{
  for (byte i = 0; i < KEY_SCHEDULE_BYTES; i++)
    keySched[i] = 0;
  round = 0;
}

void SevyEncryption::copyBytes(byte *d, byte *s, byte nn)
{
  while (nn >= 4)
  {
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    *d++ = *s++;
    nn -= 4;
  }
  while (nn--)
    *d++ = *s++;
}

byte SevyEncryption::encryptAES128Ecb(byte plain[N_BLOCK], byte cipher[N_BLOCK])
{
  if (round)
  {
    byte s1[N_BLOCK], r;
    copyKey(s1, plain, (byte *)(keySched));

    for (r = 1; r < round; r++)
    {
      byte s2[N_BLOCK];
      mixSubColumns(s2, s1);
      copyKey(s1, s2, (byte *)(keySched + r * N_BLOCK));
    }
    shiftSubRows(s1);
    copyKey(cipher, s1, (byte *)(keySched + r * N_BLOCK));
  }
  else
    return FAILURE;
  return SUCCESS;
}

byte SevyEncryption::decryptAES128Ecb(byte plain[N_BLOCK], byte cipher[N_BLOCK])
{
  if (round)
  {
    byte s1[N_BLOCK];
    copyKey(s1, plain, (byte *)(keySched + round * N_BLOCK));
    invShiftSubRows(s1);

    for (byte r = round; --r;)
    {
      byte s2[N_BLOCK];
      copyKey(s2, s1, (byte *)(keySched + r * N_BLOCK));
      invMixSubColumns(s1, s2);
    }
    copyKey(cipher, s1, (byte *)(keySched));
  }
  else
    return FAILURE;
  return SUCCESS;
}

String SevyEncryption::encrypt(String plainText, const char *pass)
{
  String res;

  for (byte i = 0; i < strlen(pass); i++)
  {
    key[i] = pass[i];
  }

  for (byte i = 0; i < plainText.length(); i++)
  {
    encInput[i] = plainText[i];
  }

  setKey(key);

  encryptAES128Ecb(encInput, encOutput);

  for (byte i = 0; i < 16; i++)
  {
    char c[4];
    sprintf(c, "%02x", encOutput[i]);
    res += String(c); 
  }

  clean();

  return res;
}

String SevyEncryption::decrypt(String encryptedText, const char *pass)
{
  byte bTmp = 'x';
  char cTmp[] = "12";
  int len = encryptedText.length();
  int vector = 0;
  int n = 0;

  for (byte i = 0; i < strlen(pass); i++)
  {
    key[i] = pass[i];
  }

  for (int i = 0; i < len; i += 2)
  {
    cTmp[0] = encryptedText[i];
    cTmp[1] = encryptedText[i + 1];
    n = strtol(cTmp, NULL, 16);
    bTmp = (char)n;
    decInput[vector] = bTmp;
    vector++;
  }

  setKey(key);

  decryptAES128Ecb(decInput, decOutput);

  String res = String((char *)decOutput);

  clean();

  return res;
}