// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef H_BITCOIN_SCRIPT_STANDARD
#define H_BITCOIN_SCRIPT_STANDARD

#include "script/interpreter.h"
#include "script/script.h"

#include <key.h>
#include <stealth.h>

#include <stdint.h>

#include <boost/variant.hpp>

class CKeyID;
class CScript;
class CStealthAddress;

static const unsigned int MAX_OP_RETURN_RELAY = 40; // bytes
extern unsigned nMaxDatacarrierBytes;

// Mandatory script verification flags that all new blocks must comply with for
// them to be valid. (but old blocks may not comply with)
//
// Failing one of these tests may trigger a DoS ban - see ConnectInputs() for
// details.
static const unsigned int MANDATORY_SCRIPT_VERIFY_FLAGS = SCRIPT_VERIFY_NONE;

// Standard script verification flags that standard transactions will comply
// with. However scripts violating these flags may still be present in valid
// blocks and we must accept those blocks.
static const unsigned int STANDARD_SCRIPT_VERIFY_FLAGS = MANDATORY_SCRIPT_VERIFY_FLAGS |
                                                         SCRIPT_VERIFY_STRICTENC |
                                                         SCRIPT_VERIFY_NULLDUMMY |
                                                         SCRIPT_VERIFY_DISCOURAGE_UPGRADABLE_NOPS;

// For convenience, standard but not mandatory verify flags.
static const unsigned int STANDARD_NOT_MANDATORY_VERIFY_FLAGS = STANDARD_SCRIPT_VERIFY_FLAGS & ~MANDATORY_SCRIPT_VERIFY_FLAGS;

enum txnouttype {
    TX_NONSTANDARD,
    // 'standard' transaction types:
    TX_PUBKEY,
    TX_PUBKEYHASH,
    TX_SCRIPTHASH,
    TX_MULTISIG,
    TX_NULL_DATA,
};

class CNoDestination
{
public:
    friend bool operator==(const CNoDestination& a, const CNoDestination& b) { return true; }
    friend bool operator<(const CNoDestination& a, const CNoDestination& b) { return true; }
};

/** A txout script template with a specific destination. It is either:
 *  * CNoDestination: no destination set
 *  * CKeyID: TX_PUBKEYHASH destination
 *  * CScriptID: TX_SCRIPTHASH destination
 *  A CTxDestination is the internal data type encoded in a CBitcoinAddress
 */
typedef boost::variant<CNoDestination, CKeyID, CScriptID, CStealthAddress> CTxDestination;

const char* GetTxnOutputType(txnouttype t);

bool Solver(const CScript& scriptPubKey, txnouttype& typeRet, std::vector<std::vector<unsigned char> >& vSolutionsRet);
int ScriptSigArgsExpected(txnouttype t, const std::vector<std::vector<unsigned char> >& vSolutions);
bool IsStandard(const CScript& scriptPubKey, txnouttype& whichType);
bool ExtractDestination(const CScript& scriptPubKey, CTxDestination& addressRet);
bool ExtractDestinations(const CScript& scriptPubKey, txnouttype& typeRet, std::vector<CTxDestination>& addressRet, int& nRequiredRet);

CScript GetScriptForDestination(const CTxDestination& dest);
CScript GetScriptForMultisig(int nRequired, const std::vector<CPubKey>& keys);

#endif // H_BITCOIN_SCRIPT_STANDARD