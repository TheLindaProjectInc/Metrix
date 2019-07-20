// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_ISMINE_H
#define BITCOIN_WALLET_ISMINE_H

#include "key.h"
#include "script/standard.h"
#include "stealth.h"

#include <stdint.h>

typedef std::vector<unsigned char> valtype;

class CKeyStore;
class CScript;

/** IsMine() return codes */
enum isminetype {
    ISMINE_NO = 0,
    ISMINE_WATCH_ONLY = 1,
    ISMINE_SPENDABLE = 2,
    ISMINE_ALL = ISMINE_WATCH_ONLY | ISMINE_SPENDABLE
};

/** used for bitflags of isminetype */
typedef uint8_t isminefilter;

bool IsDERSignature(const valtype& vchSig, bool haveHashType = true);
isminetype IsMine(const CKeyStore& keystore, const CScript& scriptPubKey);
isminetype IsMine(const CKeyStore& keystore, const CTxDestination& dest);

#endif // BITCOIN_WALLET_ISMINE_H