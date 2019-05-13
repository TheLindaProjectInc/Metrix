// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef H_BITCOIN_SCRIPTUTILS
#define H_BITCOIN_SCRIPTUTILS

#include "key.h"
#include "stealth.h"
#include "script/interpreter.h"
#include "script/script.h"
#include "script/standard.h"

#include <string>
#include <vector>

#include <stdint.h>

typedef std::vector<unsigned char> valtype;

class CKeyStore;
class CTransaction;
struct CMutableTransaction;

class BaseSignatureChecker;

template <typename T>
std::vector<unsigned char> ToByteVector(const T& in)
{
    return std::vector<unsigned char>(in.begin(), in.end());
}

typedef enum ScriptError_t
{
    SCRIPT_ERR_OK = 0,
    SCRIPT_ERR_UNKNOWN_ERROR,
    SCRIPT_ERR_EVAL_FALSE,
    SCRIPT_ERR_OP_RETURN,

    /* Max sizes */
    SCRIPT_ERR_SCRIPT_SIZE,
    SCRIPT_ERR_PUSH_SIZE,
    SCRIPT_ERR_OP_COUNT,
    SCRIPT_ERR_STACK_SIZE,
    SCRIPT_ERR_SIG_COUNT,
    SCRIPT_ERR_PUBKEY_COUNT,

    /* Failed verify operations */
    SCRIPT_ERR_VERIFY,
    SCRIPT_ERR_EQUALVERIFY,
    SCRIPT_ERR_CHECKMULTISIGVERIFY,
    SCRIPT_ERR_CHECKSIGVERIFY,
    SCRIPT_ERR_NUMEQUALVERIFY,

    /* Logical/Format/Canonical errors */
    SCRIPT_ERR_BAD_OPCODE,
    SCRIPT_ERR_DISABLED_OPCODE,
    SCRIPT_ERR_INVALID_STACK_OPERATION,
    SCRIPT_ERR_INVALID_ALTSTACK_OPERATION,
    SCRIPT_ERR_UNBALANCED_CONDITIONAL,

    /* BIP62 */
    SCRIPT_ERR_SIG_HASHTYPE,
    SCRIPT_ERR_SIG_DER,
    SCRIPT_ERR_MINIMALDATA,
    SCRIPT_ERR_SIG_PUSHONLY,
    SCRIPT_ERR_SIG_HIGH_S,
    SCRIPT_ERR_SIG_NULLDUMMY,
    SCRIPT_ERR_PUBKEYTYPE,

    /* softfork safeness */
    SCRIPT_ERR_DISCOURAGE_UPGRADABLE_NOPS,

    SCRIPT_ERR_ERROR_COUNT
} ScriptError;

#define SCRIPT_ERR_LAST SCRIPT_ERR_ERROR_COUNT

const char* ScriptErrorString(const ScriptError error);




/** IsMine() return codes */
enum isminetype
{
    ISMINE_NO = 0,
    ISMINE_WATCH_ONLY = 1,
    ISMINE_SPENDABLE = 2,
	ISMINE_ALL = ISMINE_WATCH_ONLY | ISMINE_SPENDABLE
};

/** used for bitflags of isminetype */
typedef uint8_t isminefilter;

bool IsDERSignature(const valtype &vchSig, bool haveHashType = true);
isminetype IsMine(const CKeyStore& keystore, const CScript& scriptPubKey);
isminetype IsMine(const CKeyStore& keystore, const CTxDestination& dest);
void ExtractAffectedKeys(const CKeyStore &keystore, const CScript& scriptPubKey, std::vector<CKeyID> &vKeys);
bool SignSignature(const CKeyStore& keystore, const CScript& fromPubKey, CMutableTransaction& txTo, unsigned int nIn, int nHashType=SIGHASH_ALL);
bool SignSignature(const CKeyStore& keystore, const CTransaction& txFrom, CMutableTransaction& txTo, unsigned int nIn, int nHashType=SIGHASH_ALL);
bool VerifySignature(const CTransaction& txFrom, const CTransaction& txTo, unsigned int nIn, unsigned int flags, int nHashType);

// Given two sets of signatures for scriptPubKey, possibly with OP_0 placeholders,
// combine them intelligently and return the result.
CScript CombineSignatures(CScript scriptPubKey, const CTransaction& txTo, unsigned int nIn, const CScript& scriptSig1, const CScript& scriptSig2);

CScript GetScriptForDestination(const CTxDestination& dest);
CScript GetScriptForMultisig(int nRequired, const std::vector<CPubKey>& keys);

bool Solver(const CKeyStore& keystore, const CScript& scriptPubKey, uint256 hash, int nHashType,
                  CScript& scriptSigRet, txnouttype& whichTypeRet);
//uint256 SignatureHash(const CScript &scriptCode, const CTransaction& txTo, unsigned int nIn, int nHashType);


class BaseSignatureChecker
{
public:
    virtual bool CheckSig(const std::vector<unsigned char>& scriptSig, const std::vector<unsigned char>& vchPubKey, const CScript& scriptCode) const
    {
        return false;
    }

    virtual ~BaseSignatureChecker() {}
};

class SignatureChecker : public BaseSignatureChecker
{
private:
    const CTransaction& txTo;
    unsigned int nIn;

protected:
    virtual bool VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& vchPubKey, const uint256& sighash) const;

public:
    SignatureChecker(const CTransaction& txToIn, unsigned int nInIn) : txTo(txToIn), nIn(nInIn) {}
    bool CheckSig(const std::vector<unsigned char>& scriptSig, const std::vector<unsigned char>& vchPubKey, const CScript& scriptCode) const;
};

#endif // H_BITCOIN_SCRIPTUTILS