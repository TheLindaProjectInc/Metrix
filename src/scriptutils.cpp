// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "scriptutils.h"

#include "crypto/ripemd160.h"
#include "crypto/sha1.h"
#include "crypto/sha256.h"
#include "core.h"
#include "key.h"
#include "keystore.h"
#include "random.h"
#include "sync.h"
#include "util.h"

#include <openssl/sha.h>

#include "eccryptoverify.h"

#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

using namespace std;
using namespace boost;

namespace {

inline bool set_success(ScriptError* ret)
{
    if (ret)
        *ret = SCRIPT_ERR_OK;
    return true;
}

inline bool set_error(ScriptError* ret, const ScriptError serror)
{
    if (ret)
        *ret = serror;
    return false;
}

} // anon namespace


const char* ScriptErrorString(const ScriptError serror)
{
    switch (serror)
    {
        case SCRIPT_ERR_OK:
            return "No error";
        case SCRIPT_ERR_EVAL_FALSE:
            return "Script evaluated without error but finished with a false/empty top stack element";
        case SCRIPT_ERR_VERIFY:
            return "Script failed an OP_VERIFY operation";
        case SCRIPT_ERR_EQUALVERIFY:
            return "Script failed an OP_EQUALVERIFY operation";
        case SCRIPT_ERR_CHECKMULTISIGVERIFY:
            return "Script failed an OP_CHECKMULTISIGVERIFY operation";
        case SCRIPT_ERR_CHECKSIGVERIFY:
            return "Script failed an OP_CHECKSIGVERIFY operation";
        case SCRIPT_ERR_NUMEQUALVERIFY:
            return "Script failed an OP_NUMEQUALVERIFY operation";
        case SCRIPT_ERR_SCRIPT_SIZE:
            return "Script is too big";
        case SCRIPT_ERR_PUSH_SIZE:
            return "Push value size limit exceeded";
        case SCRIPT_ERR_OP_COUNT:
            return "Operation limit exceeded";
        case SCRIPT_ERR_STACK_SIZE:
            return "Stack size limit exceeded";
        case SCRIPT_ERR_SIG_COUNT:
            return "Signature count negative or greater than pubkey count";
        case SCRIPT_ERR_PUBKEY_COUNT:
            return "Pubkey count negative or limit exceeded";
        case SCRIPT_ERR_BAD_OPCODE:
            return "Opcode missing or not understood";
        case SCRIPT_ERR_DISABLED_OPCODE:
            return "Attempted to use a disabled opcode";
        case SCRIPT_ERR_INVALID_STACK_OPERATION:
            return "Operation not valid with the current stack size";
        case SCRIPT_ERR_INVALID_ALTSTACK_OPERATION:
            return "Operation not valid with the current altstack size";
        case SCRIPT_ERR_OP_RETURN:
            return "OP_RETURN was encountered";
        case SCRIPT_ERR_UNBALANCED_CONDITIONAL:
            return "Invalid OP_IF construction";
        case SCRIPT_ERR_SIG_HASHTYPE:
            return "Signature hash type missing or not understood";
        case SCRIPT_ERR_SIG_DER:
            return "Non-canonical DER signature";
        case SCRIPT_ERR_MINIMALDATA:
            return "Data push larger than necessary";
        case SCRIPT_ERR_SIG_PUSHONLY:
            return "Only non-push operators allowed in signatures";
        case SCRIPT_ERR_SIG_HIGH_S:
            return "Non-canonical signature: S value is unnecessarily high";
        case SCRIPT_ERR_SIG_NULLDUMMY:
            return "Dummy CHECKMULTISIG argument must be zero";
        case SCRIPT_ERR_DISCOURAGE_UPGRADABLE_NOPS:
            return "NOPx reserved for soft-fork upgrades";
        case SCRIPT_ERR_PUBKEYTYPE:
            return "Public key is neither compressed or uncompressed";
        case SCRIPT_ERR_UNKNOWN_ERROR:
        case SCRIPT_ERR_ERROR_COUNT:
        default: break;
    }
    return "unknown error";
}

bool CheckSig(vector<unsigned char> vchSig, const vector<unsigned char> &vchPubKey, const CScript &scriptCode, const CTransaction& txTo, unsigned int nIn, int nHashType, int flags);

static const size_t nDefaultMaxNumSize = 4;

static bool IsCompressedOrUncompressedPubKey(const valtype &vchPubKey) {
    if (vchPubKey.size() < 33)
        return error("Non-canonical public key: too short");
    if (vchPubKey[0] == 0x04) {
        if (vchPubKey.size() != 65)
            return error("Non-canonical public key: invalid length for uncompressed key");
    } else if (vchPubKey[0] == 0x02 || vchPubKey[0] == 0x03) {
        if (vchPubKey.size() != 33)
            return error("Non-canonical public key: invalid length for compressed key");
    } else {
        return error("Non-canonical public key: neither compressed nor uncompressed");
    }
    return true;
}

bool IsDERSignature(const valtype &vchSig, bool haveHashType) {
    // See https://bitcointalk.org/index.php?topic=8392.msg127623#msg127623
    // A canonical signature exists of: <30> <total len> <02> <len R> <R> <02> <len S> <S> <hashtype>
    // Where R and S are not negative (their first byte has its highest bit not set), and not
    // excessively padded (do not start with a 0 byte, unless an otherwise negative number follows,
    // in which case a single 0 byte is necessary and even required).
    if (vchSig.size() < 9)
        return error("Non-canonical signature: too short");
    if (vchSig.size() > 73)
        return error("Non-canonical signature: too long");
    if (vchSig[0] != 0x30)
        return error("Non-canonical signature: wrong type");
    if (vchSig[1] != vchSig.size() - (haveHashType ? 3 : 2))
        return error("Non-canonical signature: wrong length marker");
    unsigned int nLenR = vchSig[3];
    if (5 + nLenR >= vchSig.size())
        return error("Non-canonical signature: S length misplaced");
    unsigned int nLenS = vchSig[5+nLenR];
    if ((unsigned long)(nLenR + nLenS + (haveHashType ? 7 : 6)) != vchSig.size())
        return error("Non-canonical signature: R+S length mismatch");

    const unsigned char *R = &vchSig[4];
    if (R[-2] != 0x02)
        return error("Non-canonical signature: R value type mismatch");
    if (nLenR == 0)
        return error("Non-canonical signature: R length is zero");
    if (R[0] & 0x80)
        return error("Non-canonical signature: R value negative");
    if (nLenR > 1 && (R[0] == 0x00) && !(R[1] & 0x80))
        return error("Non-canonical signature: R value excessively padded");

    const unsigned char *S = &vchSig[6+nLenR];
    if (S[-2] != 0x02)
        return error("Non-canonical signature: S value type mismatch");
    if (nLenS == 0)
        return error("Non-canonical signature: S length is zero");
    if (S[0] & 0x80)
        return error("Non-canonical signature: S value negative");
    if (nLenS > 1 && (S[0] == 0x00) && !(S[1] & 0x80))
        return error("Non-canonical signature: S value excessively padded");

    return true;
}

bool static IsLowDERSignature(const valtype &vchSig) {
    if (!IsDERSignature(vchSig)) {
        return false;
    }
    unsigned int nLenR = vchSig[3];
    unsigned int nLenS = vchSig[5+nLenR];
    const unsigned char *S = &vchSig[6+nLenR];
    // If the S value is above the order of the curve divided by two, its
    // complement modulo the order could have been used instead, which is
    // one byte shorter when encoded correctly.
    if (!CKey::CheckSignatureElement(S, nLenS, true))
        return error("Non-canonical signature: S value is unnecessarily high");

    return true;
}

bool static IsDefinedHashtypeSignature(const valtype &vchSig) {
    if (vchSig.size() == 0) {
        return false;
    }
    unsigned char nHashType = vchSig[vchSig.size() - 1] & (~(SIGHASH_ANYONECANPAY));
    if (nHashType < SIGHASH_ALL || nHashType > SIGHASH_SINGLE)
        return error("Non-canonical signature: unknown hashtype byte");

    return true;
}

bool static CheckSignatureEncoding(const valtype &vchSig) {
    if (!IsLowDERSignature(vchSig)) {
        return false;
    } else if (!IsDefinedHashtypeSignature(vchSig)) {
        return false;
    }
    return true;
}

bool static CheckPubKeyEncoding(const valtype &vchSig) {
    if (!IsCompressedOrUncompressedPubKey(vchSig)) {
        return false;
    }
    return true;
}


bool static IsLowDERSignature(const valtype &vchSig, ScriptError* serror) {
    if (!IsDERSignature(vchSig)) {
        return set_error(serror, SCRIPT_ERR_SIG_DER);
    }
    unsigned int nLenR = vchSig[3];
    unsigned int nLenS = vchSig[5+nLenR];
    const unsigned char *S = &vchSig[6+nLenR];
    // If the S value is above the order of the curve divided by two, its
    // complement modulo the order could have been used instead, which is
    // one byte shorter when encoded correctly.
    if (!eccrypto::CheckSignatureElement(S, nLenS, true))
        return set_error(serror, SCRIPT_ERR_SIG_HIGH_S);

    return true;
}

bool static CheckSignatureEncoding(const valtype &vchSig, unsigned int flags, ScriptError* serror) {
    if ((flags & (SCRIPT_VERIFY_DERSIG | SCRIPT_VERIFY_LOW_S | SCRIPT_VERIFY_STRICTENC)) != 0 && !IsDERSignature(vchSig)) {
        return set_error(serror, SCRIPT_ERR_SIG_DER);
    } else if ((flags & SCRIPT_VERIFY_LOW_S) != 0 && !IsLowDERSignature(vchSig, serror)) {
        // serror is set
        return false;
    } else if ((flags & SCRIPT_VERIFY_STRICTENC) != 0 && !IsDefinedHashtypeSignature(vchSig)) {
        return set_error(serror, SCRIPT_ERR_SIG_HASHTYPE);
    }
    return true;
}

bool static CheckPubKeyEncoding(const valtype &vchSig, unsigned int flags, ScriptError* serror) {
    if ((flags & SCRIPT_VERIFY_STRICTENC) != 0 && !IsCompressedOrUncompressedPubKey(vchSig)) {
        return set_error(serror, SCRIPT_ERR_PUBKEYTYPE);
    }
    return true;
}

bool static CheckMinimalPush(const valtype& data, opcodetype opcode) {
    if (data.size() == 0) {
        // Could have used OP_0.
        return opcode == OP_0;
    } else if (data.size() == 1 && data[0] >= 1 && data[0] <= 16) {
        // Could have used OP_1 .. OP_16.
        return opcode == OP_1 + (data[0] - 1);
    } else if (data.size() == 1 && data[0] == 0x81) {
        // Could have used OP_1NEGATE.
        return opcode == OP_1NEGATE;
    } else if (data.size() <= 75) {
        // Could have used a direct push (opcode indicating number of bytes pushed + those bytes).
        return opcode == data.size();
    } else if (data.size() <= 255) {
        // Could have used OP_PUSHDATA.
        return opcode == OP_PUSHDATA1;
    } else if (data.size() <= 65535) {
        // Could have used OP_PUSHDATA2.
        return opcode == OP_PUSHDATA2;
    }
    return true;
}





bool SignSignature(const CKeyStore &keystore, const CScript& fromPubKey, CMutableTransaction& txTo, unsigned int nIn, int nHashType)
{
    assert(nIn < txTo.vin.size());
    CTxIn& txin = txTo.vin[nIn];

    // Leave out the signature from the hash, since a signature can't sign itself.
    // The checksig op will also drop the signatures from its hash.
    uint256 hash = SignatureHash(fromPubKey, txTo, nIn, nHashType);

    txnouttype whichType;
    if (!Solver(keystore, fromPubKey, hash, nHashType, txin.scriptSig, whichType))
        return false;

    if (whichType == TX_SCRIPTHASH)
    {
        // Solver returns the subscript that need to be evaluated;
        // the final scriptSig is the signatures from that
        // and then the serialized subscript:
        CScript subscript = txin.scriptSig;

        // Recompute txn hash using subscript in place of scriptPubKey:
        uint256 hash2 = SignatureHash(subscript, txTo, nIn, nHashType);

        txnouttype subType;
        bool fSolved =
            Solver(keystore, subscript, hash2, nHashType, txin.scriptSig, subType) && subType != TX_SCRIPTHASH;
        // Append serialized subscript whether or not it is completely signed:
        txin.scriptSig << static_cast<valtype>(subscript);
        if (!fSolved) return false;
    }

    // Test solution
    return VerifyScript(txin.scriptSig, fromPubKey, txTo, nIn, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_STRICTENC, 0);
}

bool SignSignature(const CKeyStore &keystore, const CTransaction& txFrom, CMutableTransaction& txTo, unsigned int nIn, int nHashType)
{
    assert(nIn < txTo.vin.size());
    CTxIn& txin = txTo.vin[nIn];
    assert(txin.prevout.n < txFrom.vout.size());
    const CTxOut& txout = txFrom.vout[txin.prevout.n];

    return SignSignature(keystore, txout.scriptPubKey, txTo, nIn, nHashType);
}

bool Sign1(const CKeyID& address, const CKeyStore& keystore, uint256 hash, int nHashType, CScript& scriptSigRet)
{
    CKey key;
    if (!keystore.GetKey(address, key))
        return false;

    vector<unsigned char> vchSig;
    if (!key.Sign(hash, vchSig))
        return false;
    vchSig.push_back((unsigned char)nHashType);
    scriptSigRet << vchSig;

    return true;
}

bool SignN(const vector<valtype>& multisigdata, const CKeyStore& keystore, uint256 hash, int nHashType, CScript& scriptSigRet)
{
    int nSigned = 0;
    int nRequired = multisigdata.front()[0];
    for (unsigned int i = 1; i < multisigdata.size()-1 && nSigned < nRequired; i++)
    {
        const valtype& pubkey = multisigdata[i];
        CKeyID keyID = CPubKey(pubkey).GetID();
        if (Sign1(keyID, keystore, hash, nHashType, scriptSigRet))
            ++nSigned;
    }
    return nSigned==nRequired;
}

//
// Sign scriptPubKey with private keys stored in keystore, given transaction hash and hash type.
// Signatures are returned in scriptSigRet (or returns false if scriptPubKey can't be signed),
// unless whichTypeRet is TX_SCRIPTHASH, in which case scriptSigRet is the redemption script.
// Returns false if scriptPubKey could not be completely satisfied.
//
bool Solver(const CKeyStore& keystore, const CScript& scriptPubKey, uint256 hash, int nHashType,
                  CScript& scriptSigRet, txnouttype& whichTypeRet)
{
    scriptSigRet.clear();

    vector<valtype> vSolutions;
    if (!Solver(scriptPubKey, whichTypeRet, vSolutions))
        return false;

    CKeyID keyID;
    switch (whichTypeRet)
    {
    case TX_NONSTANDARD:
    case TX_NULL_DATA:
        return false;
    case TX_PUBKEY:
        keyID = CPubKey(vSolutions[0]).GetID();
        return Sign1(keyID, keystore, hash, nHashType, scriptSigRet);
    case TX_PUBKEYHASH:
        keyID = CKeyID(uint160(vSolutions[0]));
        if (!Sign1(keyID, keystore, hash, nHashType, scriptSigRet))
            return false;
        else
        {
            CPubKey vch;
            keystore.GetPubKey(keyID, vch);
            scriptSigRet << vch;
        }
        return true;
    case TX_SCRIPTHASH:
        return keystore.GetCScript(uint160(vSolutions[0]), scriptSigRet);

    case TX_MULTISIG:
        scriptSigRet << OP_0; // workaround CHECKMULTISIG bug
        return (SignN(vSolutions, keystore, hash, nHashType, scriptSigRet));
    }
    return false;
}

unsigned int HaveKeys(const vector<valtype>& pubkeys, const CKeyStore& keystore)
{
    unsigned int nResult = 0;
    BOOST_FOREACH(const valtype& pubkey, pubkeys)
    {
        CKeyID keyID = CPubKey(pubkey).GetID();
        if (keystore.HaveKey(keyID))
            ++nResult;
    }
    return nResult;
}

isminetype IsMine(const CKeyStore &keystore, const CTxDestination& dest)
{
    CScript script;
    script.SetDestination(dest);
    return IsMine(keystore, script);
}

isminetype IsMine(const CKeyStore &keystore, const CScript& scriptPubKey)
{
    vector<valtype> vSolutions;
    txnouttype whichType;
     if (!Solver(scriptPubKey, whichType, vSolutions)) {
        if (keystore.HaveWatchOnly(scriptPubKey))
            return ISMINE_WATCH_ONLY;
        return ISMINE_NO;
    }

    CKeyID keyID;
    switch (whichType)
    {
    case TX_NONSTANDARD:
    case TX_NULL_DATA:
        break;
    case TX_PUBKEY:
        keyID = CPubKey(vSolutions[0]).GetID();
        if (keystore.HaveKey(keyID))
            return ISMINE_SPENDABLE;
        break;
    case TX_PUBKEYHASH:
        keyID = CKeyID(uint160(vSolutions[0]));
        if (keystore.HaveKey(keyID))
            return ISMINE_SPENDABLE;
        break;
    case TX_SCRIPTHASH:
    {
        CScriptID scriptID = CScriptID(uint160(vSolutions[0]));
        CScript subscript;
        if (keystore.GetCScript(scriptID, subscript)) {
            isminetype ret = IsMine(keystore, subscript);
            if (ret == ISMINE_SPENDABLE)
                return ret;
        }
        break;
    }
    case TX_MULTISIG:
    {
        // Only consider transactions "mine" if we own ALL the
        // keys involved. multi-signature transactions that are
        // partially owned (somebody else has a key that can spend
        // them) enable spend-out-from-under-you attacks, especially
        // in shared-wallet situations.
        vector<valtype> keys(vSolutions.begin()+1, vSolutions.begin()+vSolutions.size()-1);
        if (HaveKeys(keys, keystore) == keys.size())
            return ISMINE_SPENDABLE;
        break;
    }
    }
    
    if (keystore.HaveWatchOnly(scriptPubKey))
        return ISMINE_WATCH_ONLY;
    return ISMINE_NO;
}



class CAffectedKeysVisitor : public boost::static_visitor<void> {
private:
    const CKeyStore &keystore;
    std::vector<CKeyID> &vKeys;

public:
    CAffectedKeysVisitor(const CKeyStore &keystoreIn, std::vector<CKeyID> &vKeysIn) : keystore(keystoreIn), vKeys(vKeysIn) {}

    void Process(const CScript &script) {
        txnouttype type;
        std::vector<CTxDestination> vDest;
        int nRequired;
        if (ExtractDestinations(script, type, vDest, nRequired)) {
            BOOST_FOREACH(const CTxDestination &dest, vDest)
                boost::apply_visitor(*this, dest);
        }
    }

    void operator()(const CKeyID &keyId) {
        if (keystore.HaveKey(keyId))
            vKeys.push_back(keyId);
    }

    void operator()(const CScriptID &scriptId) {
        CScript script;
        if (keystore.GetCScript(scriptId, script))
            Process(script);
    }

    void operator()(const CStealthAddress &stxAddr) {
        CScript script;
        
    }

    void operator()(const CNoDestination &none) {}
};


void ExtractAffectedKeys(const CKeyStore &keystore, const CScript& scriptPubKey, std::vector<CKeyID> &vKeys) {
    CAffectedKeysVisitor(keystore, vKeys).Process(scriptPubKey);
}

bool SignatureChecker::VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& pubkey, const uint256& sighash) const
{
    return pubkey.Verify(sighash, vchSig);
}

bool SignatureChecker::CheckSig(const vector<unsigned char>& vchSigIn, const vector<unsigned char>& vchPubKey, const CScript& scriptCode) const
{
    CPubKey pubkey(vchPubKey);
    if (!pubkey.IsValid())
        return false;

    // Hash type is one byte tacked on to the end of the signature
    vector<unsigned char> vchSig(vchSigIn);
    if (vchSig.empty())
        return false;
    int nHashType = vchSig.back();
    vchSig.pop_back();

    uint256 sighash = SignatureHash(scriptCode, txTo, nIn, nHashType);

    if (!VerifySignature(vchSig, pubkey, sighash))
        return false;

    return true;
}

static CScript PushAll(const vector<valtype>& values)
{
    CScript result;
    BOOST_FOREACH(const valtype& v, values)
        result << v;
    return result;
}

static CScript CombineMultisig(CScript scriptPubKey, const CMutableTransaction& txTo, unsigned int nIn,
                               const vector<valtype>& vSolutions,
                               vector<valtype>& sigs1, vector<valtype>& sigs2)
{
    // Combine all the signatures we've got:
    set<valtype> allsigs;
    BOOST_FOREACH(const valtype& v, sigs1)
    {
        if (!v.empty())
            allsigs.insert(v);
    }
    BOOST_FOREACH(const valtype& v, sigs2)
    {
        if (!v.empty())
            allsigs.insert(v);
    }

    // Build a map of pubkey -> signature by matching sigs to pubkeys:
    assert(vSolutions.size() > 1);
    unsigned int nSigsRequired = vSolutions.front()[0];
    unsigned int nPubKeys = vSolutions.size()-2;
    map<valtype, valtype> sigs;
    BOOST_FOREACH(const valtype& sig, allsigs)
    {
        for (unsigned int i = 0; i < nPubKeys; i++)
        {
            const valtype& pubkey = vSolutions[i+1];
            if (sigs.count(pubkey))
                continue; // Already got a sig for this pubkey

            if (CheckSig(sig, pubkey, scriptPubKey, txTo, nIn, 0, 0))
            {
                sigs[pubkey] = sig;
                break;
            }
        }
    }
    // Now build a merged CScript:
    unsigned int nSigsHave = 0;
    CScript result; result << OP_0; // pop-one-too-many workaround
    for (unsigned int i = 0; i < nPubKeys && nSigsHave < nSigsRequired; i++)
    {
        if (sigs.count(vSolutions[i+1]))
        {
            result << sigs[vSolutions[i+1]];
            ++nSigsHave;
        }
    }
    // Fill any missing with OP_0:
    for (unsigned int i = nSigsHave; i < nSigsRequired; i++)
        result << OP_0;

    return result;
}

static CScript CombineSignatures(CScript scriptPubKey, const CTransaction& txTo, unsigned int nIn,
                                 const txnouttype txType, const vector<valtype>& vSolutions,
                                 vector<valtype>& sigs1, vector<valtype>& sigs2)
{
    switch (txType)
    {
    case TX_NONSTANDARD:
    case TX_NULL_DATA:
        // Don't know anything about this, assume bigger one is correct:
        if (sigs1.size() >= sigs2.size())
            return PushAll(sigs1);
        return PushAll(sigs2);
    case TX_PUBKEY:
    case TX_PUBKEYHASH:
        // Signatures are bigger than placeholders or empty scripts:
        if (sigs1.empty() || sigs1[0].empty())
            return PushAll(sigs2);
        return PushAll(sigs1);
    case TX_SCRIPTHASH:
        if (sigs1.empty() || sigs1.back().empty())
            return PushAll(sigs2);
        else if (sigs2.empty() || sigs2.back().empty())
            return PushAll(sigs1);
        else
        {
            // Recur to combine:
            valtype spk = sigs1.back();
            CScript pubKey2(spk.begin(), spk.end());

            txnouttype txType2;
            vector<vector<unsigned char> > vSolutions2;
            Solver(pubKey2, txType2, vSolutions2);
            sigs1.pop_back();
            sigs2.pop_back();
            CScript result = CombineSignatures(pubKey2, txTo, nIn, txType2, vSolutions2, sigs1, sigs2);
            result << spk;
            return result;
        }
    case TX_MULTISIG:
        return CombineMultisig(scriptPubKey, txTo, nIn, vSolutions, sigs1, sigs2);
    }

    return CScript();
}

CScript CombineSignatures(CScript scriptPubKey, const CTransaction& txTo, unsigned int nIn,
                          const CScript& scriptSig1, const CScript& scriptSig2)
{
    txnouttype txType;
    vector<vector<unsigned char> > vSolutions;
    Solver(scriptPubKey, txType, vSolutions);

    vector<valtype> stack1;
    EvalScript(stack1, scriptSig1, CTransaction(), 0, SCRIPT_VERIFY_NONE, 0);
    vector<valtype> stack2;
    EvalScript(stack2, scriptSig2, CTransaction(), 0, SCRIPT_VERIFY_NONE, 0);

    return CombineSignatures(scriptPubKey, txTo, nIn, txType, vSolutions, stack1, stack2);
}

CScript GetScriptForDestination(const CTxDestination& dest)
{
    CScript script;

    boost::apply_visitor(CScriptVisitor(&script), dest);
    return script;
}

CScript GetScriptForMultisig(int nRequired, const std::vector<CPubKey>& keys)
{
    CScript script;

    script << CScript::EncodeOP_N(nRequired);
    BOOST_FOREACH(const CPubKey& key, keys)
        script << ToByteVector(key);
    script << CScript::EncodeOP_N(keys.size()) << OP_CHECKMULTISIG;
    return script;
}

bool CScript::IsNormalPaymentScript() const
{
    if(this->size() != 25) return false;

    std::string str;
    opcodetype opcode;
    const_iterator pc = begin();
    int i = 0;
    while (pc < end())
    {
        GetOp(pc, opcode);

        if(     i == 0 && opcode != OP_DUP) return false;
        else if(i == 1 && opcode != OP_HASH160) return false;
        else if(i == 3 && opcode != OP_EQUALVERIFY) return false;
        else if(i == 4 && opcode != OP_CHECKSIG) return false;
        else if(i == 5) return false;

        i++;
    }

    return true;
}
