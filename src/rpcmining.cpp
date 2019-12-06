// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "amount.h"
#include "chainparams.h"
#include "core_io.h"
#include "db.h"
#include "init.h"
#include "kernel.h"
#include "miner.h"
#include "rpcserver.h"
#include "txdb.h"
#include "util.h"

#include <boost/assign/list_of.hpp>

#include "univalue/univalue.h"

using namespace std;
using namespace boost::assign;

UniValue getsubsidy(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getsubsidy [nTarget]\n"
            "Returns proof-of-work subsidy value for the specified value of target.");

    CAmount nReward = GetProofOfWorkReward(0);

    return (CAmount)nReward;
}

UniValue getstakesubsidy(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "getstakesubsidy <hex string>\n"
            "Returns proof-of-stake subsidy value for the specified coinstake.");

    RPCTypeCheck(params, list_of(UniValue::VSTR));

    vector<unsigned char> txData(ParseHex(params[0].get_str()));
    CDataStream ssData(txData, SER_NETWORK, PROTOCOL_VERSION);
    CTransaction tx;
    try {
        ssData >> tx;
    } catch (std::exception& e) {
        throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "TX decode failed");
    }

    uint64_t nCoinAge;
    CCoinsViewCache view(pcoinsTip);
    CValidationState state;
    if (!GetCoinAge(tx, state, view, nCoinAge, chainActive.Height()))
        throw JSONRPCError(RPC_MISC_ERROR, "GetCoinAge failed");

    CAmount nStakeReward = GetProofOfStakeReward(nCoinAge, 0, chainActive.Height());

    return (CAmount)nStakeReward;
}

UniValue getmininginfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getmininginfo\n"
            "Returns an object containing mining-related information.");

    uint64_t nWeight = 0;
    if (pwalletMain)
        nWeight = pwalletMain->GetStakeWeight();

    UniValue obj(UniValue::VOBJ);
    UniValue diff(UniValue::VOBJ);
    UniValue weight(UniValue::VOBJ);
    obj.push_back(Pair("blocks", (int)chainActive.Height()));
    obj.push_back(Pair("currentblocksize", (uint64_t)nLastBlockSize));
    obj.push_back(Pair("currentblocktx", (uint64_t)nLastBlockTx));

    diff.push_back(Pair("proof-of-work", GetDifficulty()));
    diff.push_back(Pair("proof-of-stake", GetDifficulty(GetLastBlockIndex(chainActive.Tip(), true))));
    diff.push_back(Pair("search-interval", (int)nLastCoinStakeSearchInterval));
    obj.push_back(Pair("difficulty", diff));
    obj.push_back(Pair("blockvalue", (uint64_t)GetProofOfWorkReward(0)));

    obj.push_back(Pair("netmhashps", GetPoWMHashPS()));
    obj.push_back(Pair("netstakeweight", GetPoSKernelPS()));
    obj.push_back(Pair("errors", GetWarnings("statusbar")));
    obj.push_back(Pair("pooledtx", (uint64_t)mempool.size()));
    obj.push_back(Pair("chain", Params().NetworkIDString()));

    weight.push_back(Pair("minimum", (uint64_t)nWeight));
    weight.push_back(Pair("maximum", (uint64_t)0));
    weight.push_back(Pair("combined", (uint64_t)nWeight));
    obj.push_back(Pair("stakeweight", weight));

    obj.push_back(Pair("testnet", Params().TestnetToBeDeprecatedFieldRPC()));
    return obj;
}

UniValue getstakinginfo(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getstakinginfo\n"
            "Returns an object containing staking-related information.");

    uint64_t nWeight = 0;
    if (pwalletMain)
        nWeight = pwalletMain->GetStakeWeight();

    uint64_t nNetworkWeight = GetPoSKernelPS();
    bool staking = nLastCoinStakeSearchInterval && nWeight && (GetAdjustedTime() - nLastCoinStakeSearchTime < 60);
    uint64_t nExpectedTime = staking ? (Params().TargetSpacing() * nNetworkWeight / nWeight) : 0;

    UniValue obj(UniValue::VOBJ);

    obj.push_back(Pair("enabled", GetBoolArg("-staking", true)));
    obj.push_back(Pair("staking", staking));
    obj.push_back(Pair("errors", GetWarnings("statusbar")));

    obj.push_back(Pair("currentblocksize", (uint64_t)nLastBlockSize));
    obj.push_back(Pair("currentblocktx", (uint64_t)nLastBlockTx));
    obj.push_back(Pair("pooledtx", (uint64_t)mempool.size()));

    obj.push_back(Pair("difficulty", GetDifficulty(GetLastBlockIndex(chainActive.Tip(), true))));
    obj.push_back(Pair("search-interval", (int)nLastCoinStakeSearchInterval));
    obj.push_back(Pair("last-search-time", (int)nLastCoinStakeSearchTime));

    obj.push_back(Pair("weight", (uint64_t)nWeight));
    obj.push_back(Pair("netstakeweight", (uint64_t)nNetworkWeight));

    obj.push_back(Pair("expectedtime", nExpectedTime));

    return obj;
}

UniValue checkkernel(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() < 1 || params.size() > 2)
        throw runtime_error(
            "checkkernel [{\"txid\":txid,\"vout\":n},...] [createblocktemplate=false]\n"
            "Check if one of given inputs is a kernel input at the moment.\n");

    RPCTypeCheck(params, list_of(UniValue::VARR)(UniValue::VBOOL));

    UniValue inputs = params[0].get_array();
    bool fCreateBlockTemplate = params.size() > 1 ? params[1].get_bool() : false;

    if (vNodes.empty())
        throw JSONRPCError(-9, "Metrix is not connected!");

    if (IsInitialBlockDownload())
        throw JSONRPCError(-10, "Metrix is downloading blocks...");

    COutPoint kernel;
    CBlockIndex* pindexPrev = chainActive.Tip();
    unsigned int nBits = GetNextTargetRequired(pindexPrev, true);
    int64_t nTime = GetAdjustedTime();
    nTime &= ~STAKE_TIMESTAMP_MASK;

    for (unsigned int idx = 0; idx < inputs.size(); idx++) {
        const UniValue& input = inputs[idx];
        const UniValue& o = input.get_obj();

        const UniValue& txid_v = find_value(o, "txid");
        if (!txid_v.isStr())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, missing txid key");
        string txid = txid_v.get_str();
        if (!IsHex(txid))
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, expected hex txid");

        const UniValue& vout_v = find_value(o, "vout");
        if (!vout_v.isNum())
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, missing vout key");
        int nOutput = vout_v.get_int();
        if (nOutput < 0)
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid parameter, vout must be positive");

        COutPoint cInput(uint256(txid), nOutput);
        if (CheckKernel(pindexPrev, nBits, nTime, cInput)) {
            kernel = cInput;
            break;
        }
    }

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("found", !kernel.IsNull()));

    if (kernel.IsNull())
        return result;

    UniValue oKernel(UniValue::VOBJ);;
    oKernel.push_back(Pair("txid", kernel.hash.GetHex()));
    oKernel.push_back(Pair("vout", (int64_t)kernel.n));
    oKernel.push_back(Pair("time", nTime));
    result.push_back(Pair("kernel", oKernel));

    if (!fCreateBlockTemplate)
        return result;
    CReserveKey reservekey(pwalletMain);
    CAmount nFees;
    auto_ptr<CBlockTemplate> pblocktemplate(CreateNewBlockWithKey(reservekey, pwalletMain, true));
    CBlock* pblock = &pblocktemplate->block; //! pointer for convenience
    pblock->nTime = pblock->vtx[0].nTime = nTime;

    CDataStream ss(SER_DISK, PROTOCOL_VERSION);
    ss << *pblock;

    result.push_back(Pair("blocktemplate", HexStr(ss.begin(), ss.end())));
    result.push_back(Pair("blocktemplatefees", nFees));

    CPubKey pubkey;
    result.push_back(Pair("blocktemplatesignkey", HexStr(pubkey)));

    return result;
}

// NOTE: Unlike wallet RPC (which use BTC values), mining RPCs follow GBT (BIP 22) in using satoshi amounts
UniValue prioritisetransaction(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 3)
        throw runtime_error(
            "prioritisetransaction <txid> <priority delta> <fee delta>\n"
            "Accepts the transaction into mined blocks at a higher (or lower) priority\n"
            "\nArguments:\n"
            "1. \"txid\"       (string, required) The transaction id.\n"
            "2. priority delta (numeric, required) The priority to add or subtract.\n"
            "                  The transaction selection algorithm considers the tx as it would have a higher priority.\n"
            "                  (priority of a transaction is calculated: coinage * value_in_satoshis / txsize) \n"
            "3. fee delta      (numeric, required) The fee value (in satoshis) to add (or subtract, if negative).\n"
            "                  The fee is not actually paid, only the algorithm for selecting transactions into a block\n"
            "                  considers the transaction as it would have paid a higher (or lower) fee.\n"
            "\nResult\n"
            "true              (boolean) Returns true\n"
            "\nExamples:\n"
            + HelpExampleCli("prioritisetransaction", "\"txid\" 0.0 10000")
            + HelpExampleRpc("prioritisetransaction", "\"txid\", 0.0, 10000")
        );

    uint256 hash = ParseHashStr(params[0].get_str(), "txid");
    CAmount nAmount = params[2].get_int64();

    mempool.PrioritiseTransaction(hash, params[0].get_str(), params[1].get_real(), nAmount);
    return true;
}

UniValue getblocktemplate(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() > 1)
        throw runtime_error(
            "getblocktemplate [params]\n"
            "Returns data needed to construct a block to work on:\n"
            "  \"version\" : block version\n"
            "  \"previousblockhash\" : hash of current highest block\n"
            "  \"transactions\" : contents of non-coinbase transactions that should be included in the next block\n"
            "  \"coinbaseaux\" : data that should be included in coinbase\n"
            "  \"coinbasevalue\" : maximum allowable input to coinbase transaction, including the generation award and transaction fees\n"
            "  \"target\" : hash target\n"
            "  \"mintime\" : minimum timestamp appropriate for next block\n"
            "  \"curtime\" : current timestamp\n"
            "  \"mutable\" : list of ways the block template may be changed\n"
            "  \"noncerange\" : range of valid nonces\n"
            "  \"sigoplimit\" : limit of sigops in blocks\n"
            "  \"sizelimit\" : limit of block size\n"
            "  \"bits\" : compressed target of next block\n"
            "  \"height\" : height of the next block\n"
            "See https://en.bitcoin.it/wiki/BIP_0022 for full specification.");

    std::string strMode = "template";
    UniValue lpval = NullUniValue;
    if (params.size() > 0) {
        const UniValue& oparam = params[0].get_obj();
        const UniValue& modeval = find_value(oparam, "mode");
        if (modeval.isStr())
            strMode = modeval.get_str();
        else if (modeval.isNull())
        {
            /* Do nothing */
        }
        else
            throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid mode");
        lpval = find_value(oparam, "longpollid");
    }

    if (strMode != "template")
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid mode");

    if (vNodes.empty())
        throw JSONRPCError(RPC_CLIENT_NOT_CONNECTED, "Metrix is not connected!");

    //! if (IsInitialBlockDownload())
    //!     throw JSONRPCError(RPC_CLIENT_IN_INITIAL_DOWNLOAD, "Metrix is downloading blocks...");

    if (chainActive.Height() >= Params().LastPOWBlock())
        throw JSONRPCError(RPC_MISC_ERROR, "No more PoW blocks");

    static unsigned int nTransactionsUpdatedLast;
    if (!lpval.isNull())
    {
        //! Wait to respond until either the best block changes, OR a minute has passed and there are more transactions
        uint256 hashWatchedChain;
        boost::system_time checktxtime;
        unsigned int nTransactionsUpdatedLastLP;

        if (lpval.isStr())
        {
            //! Format: <hashBestChain><nTransactionsUpdatedLast>
            std::string lpstr = lpval.get_str();
            hashWatchedChain.SetHex(lpstr.substr(0, 64));
            nTransactionsUpdatedLastLP = atoi64(lpstr.substr(64));
        } else {
            //! NOTE: Spec does not specify behaviour for non-string longpollid, but this makes testing easier
            hashWatchedChain = chainActive.Tip()->GetBlockHash();
            nTransactionsUpdatedLastLP = nTransactionsUpdatedLast;
        }

        //! Release the wallet and main lock while waiting
#ifdef ENABLE_WALLET
        if (pwalletMain)
            LEAVE_CRITICAL_SECTION(pwalletMain->cs_wallet);
#endif
        LEAVE_CRITICAL_SECTION(cs_main);
        {
            checktxtime = boost::get_system_time() + boost::posix_time::minutes(1);

            boost::unique_lock<boost::mutex> lock(csBestBlock);
            while (chainActive.Tip()->GetBlockHash() == hashWatchedChain && IsRPCRunning()) {
                if (!cvBlockChange.timed_wait(lock, checktxtime)) {
                    //! Timeout: Check transactions for update
                    if (mempool.GetTransactionsUpdated() != nTransactionsUpdatedLastLP)
                        break;
                    checktxtime += boost::posix_time::seconds(10);
                }
            }
        }
        ENTER_CRITICAL_SECTION(cs_main);
#ifdef ENABLE_WALLET
        if (pwalletMain)
            ENTER_CRITICAL_SECTION(pwalletMain->cs_wallet);
#endif

        if (!IsRPCRunning())
            throw JSONRPCError(RPC_CLIENT_NOT_CONNECTED, "Shutting down");
        //! TODO: Maybe recheck connections/IBD and (if something wrong) send an expires-immediately template to stop miners?
    }

    //! Update block
    static CBlockIndex* pindexPrev;
    static int64_t nStart;
    static CBlockTemplate* pblocktemplate;
    if (pindexPrev != chainActive.Tip() ||
        (mempool.GetTransactionsUpdated() != nTransactionsUpdatedLast && GetTime() - nStart > 5)) {
        //! Clear pindexPrev so future calls make a new block, despite any failures from here on
        pindexPrev = NULL;

        //! Store the chainActive.Tip() used before CreateNewBlock, to avoid races
        nTransactionsUpdatedLast = mempool.GetTransactionsUpdated();
        CBlockIndex* pindexPrevNew = chainActive.Tip();
        nStart = GetTime();

        //! Create new block
        if (pblocktemplate) {
            delete pblocktemplate;
            pblocktemplate = NULL;
        }
        CScript scriptDummy = CScript() << OP_TRUE;
        pblocktemplate = CreateNewBlock(scriptDummy, pwalletMain, false);
        if (!pblocktemplate)
            throw JSONRPCError(RPC_OUT_OF_MEMORY, "Out of memory");

        //! Need to update only after we know CreateNewBlock succeeded
        pindexPrev = pindexPrevNew;
    }
    CBlock* pblock = &pblocktemplate->block; //! pointer for convenience

    //! Update nTime
    UpdateTime(*pblock, pindexPrev);
    pblock->nNonce = 0;

    UniValue transactions(UniValue::VARR);
    map<uint256, int64_t> setTxIndex;
    int i = 0;

    BOOST_FOREACH (CTransaction& tx, pblock->vtx) {
        uint256 txHash = tx.GetHash();
        setTxIndex[txHash] = i++;

        if (tx.IsCoinBase() || tx.IsCoinStake())
            continue;

        UniValue entry(UniValue::VOBJ);

        entry.push_back(Pair("data", EncodeHexTx(tx)));

        entry.push_back(Pair("hash", txHash.GetHex()));

        UniValue deps(UniValue::VARR);
        BOOST_FOREACH (const CTxIn& in, tx.vin) {
            if (setTxIndex.count(in.prevout.hash))
                deps.push_back(setTxIndex[in.prevout.hash]);
        }
        entry.push_back(Pair("depends", deps));
        int index_in_template = i - 1;
        entry.push_back(Pair("fee", pblocktemplate->vTxFees[index_in_template]));
        entry.push_back(Pair("sigops", pblocktemplate->vTxSigOps[index_in_template]));
        transactions.push_back(entry);
    }

    UniValue aux(UniValue::VOBJ);
    aux.push_back(Pair("flags", HexStr(COINBASE_FLAGS.begin(), COINBASE_FLAGS.end())));

    uint256 hashTarget = uint256().SetCompact(pblock->nBits);

    static UniValue aMutable(UniValue::VARR);
    if (aMutable.empty()) {
        aMutable.push_back("time");
        aMutable.push_back("transactions");
        aMutable.push_back("prevblock");
    }

    UniValue result(UniValue::VOBJ);
    result.push_back(Pair("version", pblock->nVersion));
    result.push_back(Pair("previousblockhash", pblock->hashPrevBlock.GetHex()));
    result.push_back(Pair("transactions", transactions));
    result.push_back(Pair("coinbaseaux", aux));
    result.push_back(Pair("coinbasevalue", (int64_t)pblock->vtx[0].vout[0].nValue));
    result.push_back(Pair("longpollid", chainActive.Tip()->GetBlockHash().GetHex() + i64tostr(nTransactionsUpdatedLast)));
    result.push_back(Pair("target", hashTarget.GetHex()));
    result.push_back(Pair("mintime", (int64_t)pindexPrev->GetPastTimeLimit() + 1));
    result.push_back(Pair("mutable", aMutable));
    result.push_back(Pair("noncerange", "00000000ffffffff"));
    result.push_back(Pair("sigoplimit", (int64_t)MAX_BLOCK_SIGOPS));
    result.push_back(Pair("sizelimit", (int64_t)MAX_BLOCK_SIZE));
    result.push_back(Pair("curtime", pblock->GetBlockTime()));
    result.push_back(Pair("bits", strprintf("%08x", pblock->nBits)));
    result.push_back(Pair("height", (int64_t)(pindexPrev->nHeight + 1)));

    return result;
}

UniValue estimatefee(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "estimatefee nblocks\n"
            "\nEstimates the approximate fee per kilobyte\n"
            "needed for a transaction to begin confirmation\n"
            "within nblocks blocks.\n"
            "\nArguments:\n"
            "1. nblocks     (numeric)\n"
            "\nResult:\n"
            "n :    (numeric) estimated fee-per-kilobyte\n"
            "\n"
            "-1.0 is returned if not enough transactions and\n"
            "blocks have been observed to make an estimate.\n"
            "\nExample:\n" +
            HelpExampleCli("estimatefee", "6"));

    RPCTypeCheck(params, boost::assign::list_of(UniValue::VNUM));

    int nBlocks = params[0].get_int();
    if (nBlocks < 1)
        nBlocks = 1;

    CFeeRate feeRate = mempool.estimateFee(nBlocks);
    if (feeRate == CFeeRate(0))
        return -1.0;

    return ValueFromAmount(feeRate.GetFeePerK());
}

UniValue estimatepriority(const UniValue& params, bool fHelp)
{
    if (fHelp || params.size() != 1)
        throw runtime_error(
            "estimatepriority nblocks\n"
            "\nEstimates the approximate priority\n"
            "a zero-fee transaction needs to begin confirmation\n"
            "within nblocks blocks.\n"
            "\nArguments:\n"
            "1. nblocks     (numeric)\n"
            "\nResult:\n"
            "n :    (numeric) estimated priority\n"
            "\n"
            "-1.0 is returned if not enough transactions and\n"
            "blocks have been observed to make an estimate.\n"
            "\nExample:\n" +
            HelpExampleCli("estimatepriority", "6"));

    RPCTypeCheck(params, boost::assign::list_of(UniValue::VNUM));

    int nBlocks = params[0].get_int();
    if (nBlocks < 1)
        nBlocks = 1;

    return mempool.estimatePriority(nBlocks);
}