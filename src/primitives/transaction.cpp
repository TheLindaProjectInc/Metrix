// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/transaction.h"

#include "hash.h"
#include "timedata.h"
#include "tinyformat.h"
#include "utilmoneystr.h"
#include "utilstrencodings.h"

#include <boost/foreach.hpp>

std::string COutPoint::ToString() const
{
    return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0, 10), n);
}

CTxIn::CTxIn(COutPoint prevoutIn, CScript scriptSigIn, uint32_t nSequenceIn)
{
    prevout = prevoutIn;
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

CTxIn::CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn, uint32_t nSequenceIn)
{
    prevout = COutPoint(hashPrevTx, nOut);
    scriptSig = scriptSigIn;
    nSequence = nSequenceIn;
}

std::string CTxIn::ToString() const
{
    std::string str;
    str += "CTxIn(";
    str += prevout.ToString();
    if (prevout.IsNull())
        str += strprintf(", coinbase %s", HexStr(scriptSig));
    else
        str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0, 24));
    if (nSequence != std::numeric_limits<unsigned int>::max())
        str += strprintf(", nSequence=%u", nSequence);
    str += ")";
    return str;
}

CTxOut::CTxOut(const CAmount& nValueIn, CScript scriptPubKeyIn)
{
    nValue = nValueIn;
    scriptPubKey = scriptPubKeyIn;
}

uint256 CTxOut::GetHash() const
{
    return SerializeHash(*this);
}

std::string CTxOut::ToString() const
{
    if (IsEmpty())
        return "CTxOut(empty)";
    return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue), scriptPubKey.ToString());
}

CMutableTransaction::CMutableTransaction() : nVersion(CTransaction::CURRENT_VERSION), nTime(GetAdjustedTime()), nLockTime(0) {}
CMutableTransaction::CMutableTransaction(const CTransaction& tx) : nVersion(tx.nVersion), nTime(tx.nTime), vin(tx.vin), vout(tx.vout), nLockTime(tx.nLockTime) {}

uint256 CMutableTransaction::GetHash() const
{
    return SerializeHash(*this);
}

void CTransaction::UpdateHash() const
{
    *const_cast<uint256*>(&hash) = SerializeHash(*this);
}

CTransaction::CTransaction() : nVersion(CTransaction::CURRENT_VERSION), nTime(GetAdjustedTime()), vin(), vout(), nLockTime(0) {}

CTransaction::CTransaction(const CMutableTransaction& tx) : nVersion(tx.nVersion), nTime(tx.nTime), vin(tx.vin), vout(tx.vout), nLockTime(tx.nLockTime)
{
    UpdateHash();
}

CTransaction& CTransaction::operator=(const CTransaction& tx)
{
    *const_cast<int*>(&nVersion) = tx.nVersion;
    *const_cast<unsigned int*>(&nTime) = tx.nTime;
    *const_cast<std::vector<CTxIn>*>(&vin) = tx.vin;
    *const_cast<std::vector<CTxOut>*>(&vout) = tx.vout;
    *const_cast<unsigned int*>(&nLockTime) = tx.nLockTime;
    *const_cast<uint256*>(&hash) = tx.hash;
    return *this;
}

/** Amount of bitcoins spent by the transaction.
    @return sum of all outputs (note: does not include fees)
 */
CAmount CTransaction::GetValueOut() const
{
    CAmount nValueOut = 0;
    BOOST_FOREACH (const CTxOut& txout, vout) {
        nValueOut += txout.nValue;
        if (!MoneyRange(txout.nValue) || !MoneyRange(nValueOut))
            throw std::runtime_error("GetValueOut() : value out of range");
    }
    return nValueOut;
}

double CTransaction::ComputePriority(double dPriorityInputs, unsigned int nTxSize) const
{
    nTxSize = CalculateModifiedSize(nTxSize);
    if (nTxSize == 0)
        return 0.0;

    return dPriorityInputs / nTxSize;
}

unsigned int CTransaction::CalculateModifiedSize(unsigned int nTxSize) const
{
    // In order to avoid disincentivizing cleaning up the UTXO set we don't count
    // the constant overhead for each txin and up to 110 bytes of scriptSig (which
    // is enough to cover a compressed pubkey p2sh redemption) for priority.
    // Providing any more cleanup incentive than making additional inputs free would
    // risk encouraging people to create junk outputs to redeem later.
    if (nTxSize == 0)
        nTxSize = ::GetSerializeSize(*this, SER_NETWORK, PROTOCOL_VERSION);

    BOOST_FOREACH (const CTxIn& txin, vin) {
        unsigned int offset = 41U + std::min(110U, (unsigned int)txin.scriptSig.size());
        if (nTxSize > offset)
            nTxSize -= offset;
    }
    return nTxSize;
}

std::string CMutableTransaction::ToString() const
{
    std::string str;
    str += IsCoinBase() ? "Coinbase" : (IsCoinStake() ? "Coinstake" : "CTransaction");
    str += strprintf("(hash=%s, nTime=%d, ver=%d, vin.size=%u, vout.size=%u, nLockTime=%d)\n",
                     GetHash().ToString(),
                     nTime,
                     nVersion,
                     vin.size(),
                     vout.size(),
                     nLockTime);
    for (unsigned int i = 0; i < vin.size(); i++)
        str += "    " + vin[i].ToString() + "\n";
    for (unsigned int i = 0; i < vout.size(); i++)
        str += "    " + vout[i].ToString() + "\n";
    return str;
}

std::string CTransaction::ToString() const
{
    std::string str;
    str += IsCoinBase() ? "Coinbase" : (IsCoinStake() ? "Coinstake" : "CTransaction");
    str += strprintf("(hash=%s, nTime=%d, ver=%d, vin.size=%u, vout.size=%u, nLockTime=%d)\n",
                     GetHash().ToString(),
                     nTime,
                     nVersion,
                     vin.size(),
                     vout.size(),
                     nLockTime);
    for (unsigned int i = 0; i < vin.size(); i++)
        str += "    " + vin[i].ToString() + "\n";
    for (unsigned int i = 0; i < vout.size(); i++)
        str += "    " + vout[i].ToString() + "\n";
    return str;
}