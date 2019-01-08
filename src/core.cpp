// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "core.h"
#include "util.h"
#include "scrypt.h"


uint256 CBlockHeader::GetPoWHash() const
{
    return scrypt_blockhash(CVOIDBEGIN(nVersion));
}