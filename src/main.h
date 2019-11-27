// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_MAIN_H
#define BITCOIN_MAIN_H

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "amount.h"
#include "chain.h"
#include "chainparams.h"
#include "coins.h"
#include "primitives/block.h"
#include "primitives/transaction.h"
#include "net.h"
#include "script/script.h"
#include "script/sigcache.h"
#include "script/standard.h"
#include "scrypt.h"
#include "sync.h"
#include "txmempool.h"
#include "utilmoneystr.h"
#include "undo.h"

#include <algorithm>
#include <exception>
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include <boost/unordered_map.hpp>

#define START_MASTERNODE_PAYMENTS_TESTNET 1429738064
#define START_MASTERNODE_PAYMENTS 1429738064

static const int REWARD_START = 51;
static const int PREMINE_BLOCK = 10;
static const int FAIR_LAUNCH_BLOCK = 50;

//! MBK: Added globals to simplify future potential changes
static const int64_t MASTERNODE_COLLATERAL = (2000000 * COIN);
static const int64_t COIN_YEAR_REWARD_V2 = (99 * CENT);
static const int64_t DARKSEND_FEE = (0.01 * COIN);
static const int64_t DARKSEND_POOL_MAX = (1111.99 * COIN);
static const std::array<int64_t,5> MASTERNODE_COLLATERALS = { 2000000 * COIN, 5000000 * COIN, 25000000 * COIN, 50000000 * COIN, 100000000 * COIN };
//! MBK: Following are block heights to begin V2 swap
static const int POS_REWARD_V2_START_BLOCK = 359930; //! ~03312018 (March 31, 2018)
static const int POW_REWARD_V2_START_BLOCK = 378230; //! ~04052018 (April 5, 2018)
static const int TX_FEE_V2_INCREASE_BLOCK = 378230;  //! ~04052018 (April 5, 2018)
static const int MASTERNODE_V2_START_BLOCK = 378230; //! ~04052018 (April 5, 2018)
static const int DARKSEND_V2_START_BLOCK = 378230;   //! ~04052018 (April 5, 2018)
//! MBK: Following define PoW/PoS reward parameters
static const int POW_REWARD_V1_FULL = 14150;
static const int POW_REWARD_V2_FULL = 13726; //! ~3% reduction from V1 block reward
//! V3 block rewards
static const int V3_START_BLOCK = 580000; //! (April 7, 2018)
static const int64_t COIN_YEAR_REWARD_V3 = (50 * CENT);
static const int64_t MAX_STAKE_VALUE = (100 * 1000000 * COIN); //! POS rewards will be capped to 100 million MRX
//! Start soft-fork v8 THIS WILL BE MODIFIED BEFORE RELEASE
static const int V8_START_BLOCK = 2000000;
/*
    At 15 signatures, 1/2 of the masternode network can be owned by
    one party without comprimising the security of InstantX
    (1000/2150.0)**15 = 1.031e-05
*/
#define INSTANTX_SIGNATURES_REQUIRED 20
#define INSTANTX_SIGNATURES_TOTAL 30

#define MASTERNODE_NOT_PROCESSED 0 //! initial state
#define MASTERNODE_IS_CAPABLE 1
#define MASTERNODE_NOT_CAPABLE 2
#define MASTERNODE_STOPPED 3
#define MASTERNODE_INPUT_TOO_NEW 4
#define MASTERNODE_PORT_NOT_OPEN 6
#define MASTERNODE_PORT_OPEN 7
#define MASTERNODE_SYNC_IN_PROCESS 8
#define MASTERNODE_REMOTELY_ENABLED 9

#define MASTERNODE_MIN_CONFIRMATIONS 15
#define MASTERNODE_MIN_DSEEP_SECONDS (30 * 60)
#define MASTERNODE_MIN_DSEE_SECONDS (5 * 60)
#define MASTERNODE_PING_SECONDS (1 * 60)
#define MASTERNODE_EXPIRATION_SECONDS (65 * 60)
#define MASTERNODE_REMOVAL_SECONDS (70 * 60)


inline void GetMasternodeCollaterals(std::vector<CAmount>& vCollaterals, CBlockIndex* pindex)
{
    //! Allow different Masternode tiers for block.nVersion=8 blocks 
    //! when 75% of the network has upgraded
    if (pindex != NULL && CBlockIndex::IsSuperMajority(8, pindex, Params().EnforceBlockUpgradeMajority()))
    {
        BOOST_FOREACH (CAmount collateral, MASTERNODE_COLLATERALS)
        {
            vCollaterals.push_back(collateral);
        }
    }
    else
    {
        vCollaterals.push_back(MASTERNODE_COLLATERAL);
    }
}

inline int64_t IsValidMasternodeCollateral(CAmount collateral, CBlockIndex* pindex)
{ 
    std::vector<CAmount> vCollaterals;
    GetMasternodeCollaterals(vCollaterals, pindex);
    return std::find(std::begin(vCollaterals), std::end(vCollaterals), collateral) != std::end(vCollaterals);  
}


/** "reject" message codes **/
static const unsigned char REJECT_MALFORMED = 0x01;
static const unsigned char REJECT_INVALID = 0x10;
static const unsigned char REJECT_OBSOLETE = 0x11;
static const unsigned char REJECT_DUPLICATE = 0x12;
static const unsigned char REJECT_NONSTANDARD = 0x40;
static const unsigned char REJECT_DUST = 0x41;
static const unsigned char REJECT_INSUFFICIENTFEE = 0x42;
static const unsigned char REJECT_CHECKPOINT = 0x43;

class CBlock;
class CBlockIndex;
class CBlockTreeDB;
class CCoins;
class CInv;
class CKeyItem;
class CNode;
class CReserveKey;
class CScriptCheck;
class CWallet;
class CValidationInterface;
class CValidationState;

struct CBlockTemplate;
struct CNodeStateStats;

/** The maximum size for transactions we're willing to relay/mine **/
static const unsigned int MAX_STANDARD_TX_SIZE = 100000;
/** Default for -blockmaxsize  and -blockminsize, which control the range of sizes the mining code will create **/
static const unsigned int DEFAULT_BLOCK_MAX_SIZE = 1000000;
static const unsigned int DEFAULT_BLOCK_MIN_SIZE = 0;
/** Default for -blockprioritysize, maximum space for zero/low-fee transactions **/
static const unsigned int DEFAULT_BLOCK_PRIORITY_SIZE = 30000;
/** Default for accepting alerts from the P2P network. */
static const bool DEFAULT_ALERTS = true;
/** The maximum allowed number of signature check operations in a block (network rule) */
static const unsigned int MAX_BLOCK_SIGOPS = MAX_BLOCK_SIZE / 50;
/** Maxiumum number of signature check operations in an IsStandard() P2SH script */
static const unsigned int MAX_P2SH_SIGOPS = 15;
/** The maximum number of sigops we're willing to relay/mine in a single tx */
static const unsigned int MAX_TX_SIGOPS = MAX_BLOCK_SIGOPS / 5;
/** Default for -maxorphantx, maximum number of orphan transactions kept in memory */
static const unsigned int DEFAULT_MAX_ORPHAN_TRANSACTIONS = 100;
/** Default for -maxorphanblocks, maximum number of orphan blocks kept in memory */
static const unsigned int DEFAULT_MAX_ORPHAN_BLOCKS = 750;
/** The maximum size of a blk?????.dat file (since 0.8) */
static const unsigned int MAX_BLOCKFILE_SIZE = 0x8000000; //! 128 MiB
/** The pre-allocation chunk size for blk?????.dat files (since 0.8) */
static const unsigned int BLOCKFILE_CHUNK_SIZE = 0x1000000; //! 16 MiB
/** The pre-allocation chunk size for rev?????.dat files (since 0.8) */
static const unsigned int UNDOFILE_CHUNK_SIZE = 0x100000; //! 1 MiB
/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
static const int COINBASE_MATURITY = 500;
/** Maximum number of script-checking threads allowed */
static const int MAX_SCRIPTCHECK_THREADS = 16;
/** -par default (number of script-checking threads, 0 = auto) */
static const int DEFAULT_SCRIPTCHECK_THREADS = 0;
/** Number of blocks that can be requested at any given time from a single peer. */
static const int MAX_BLOCKS_IN_TRANSIT_PER_PEER = 16;
/** Timeout in seconds during which a peer must stall block download progress before being disconnected. */
static const unsigned int BLOCK_STALLING_TIMEOUT = 2;
/** Number of headers sent in one getheaders result. We rely on the assumption that if a peer sends
 *  less than this number, we reached their tip. Changing this value is a protocol upgrade. */
static const unsigned int MAX_HEADERS_RESULTS = 2000;
/** Number of headers that can be waiting for their blocks. In POS it's not possible to confidently
 *  verify a header is valid as the coinstake requires the complete transaction history.
 *  This will help prevent exhaustion attacks from bad actors spamming bad headers */
static const unsigned int MAX_HEADERS_PENDING = 10 * MAX_HEADERS_RESULTS;
/** Size of the "block download window": how far ahead of our current height do we fetch?
 *  Larger windows tolerate larger download speed differences between peer, but increase the potential
 *  degree of disordering of blocks on disk (which make reindexing and in the future perhaps pruning
 *  harder). We'll probably want to make this a per-peer adaptive value at some point. */
static const unsigned int BLOCK_DOWNLOAD_WINDOW = 1024;
/** Time to wait (in seconds) between writing blockchain state to disk. */
static const unsigned int DATABASE_WRITE_INTERVAL = 3600;

/** Average delay between local address broadcasts in seconds. */
static const unsigned int AVG_LOCAL_ADDRESS_BROADCAST_INTERVAL = 24 * 24 * 60;
/** Average delay between peer address broadcasts in seconds. */
static const unsigned int AVG_ADDRESS_BROADCAST_INTERVAL = 30;
/** Average delay between trickled inventory broadcasts in seconds.
 *  Blocks, whitelisted receivers, and a random 25% of transactions bypass this. */
static const unsigned int AVG_INVENTORY_BROADCAST_INTERVAL = 5;
/** Maximum length of reject messages. */
static const unsigned int MAX_REJECT_MESSAGE_LENGTH = 111;

static const int64_t COIN_YEAR_REWARD = 99 * CENT; //! 99% per year
static const int POS_START_BLOCK = 25;
static const int DIFF_FORK_BLOCK = 100;

inline int64_t FutureDrift(int64_t nTime) { return nTime + 15; }

static const int64_t STAKE_TIMESPAN_SWITCH_TIME = 1428537599;

struct BlockHasher {
    size_t operator()(const uint256& hash) const { return hash.GetLow64(); }
};

extern CScript COINBASE_FLAGS;
extern CCriticalSection cs_main;
extern CTxMemPool mempool;
typedef boost::unordered_map<uint256, CBlockIndex*, BlockHasher> BlockMap;
extern BlockMap mapBlockIndex;
extern std::set<std::pair<COutPoint, unsigned int> > setStakeSeen;
extern unsigned int nStakeMinAge;
extern unsigned int nStakeMaxAge;
extern unsigned int nNodeLifespan;
extern int nCoinbaseMaturity;
extern uint64_t nLastBlockTx;
extern uint64_t nLastBlockSize;
extern int64_t nLastCoinStakeSearchInterval;
extern int64_t nLastCoinStakeSearchTime;
extern const std::string strMessageMagic;
extern int64_t nTimeBestReceived;
extern CWaitableCriticalSection csBestBlock;
extern CConditionVariable cvBlockChange;
extern bool fImporting;
extern bool fReindex;
extern bool fIsBareMultisigStd;
extern bool fCheckBlockIndex;
extern int nScriptCheckThreads;
extern unsigned int nCoinCacheSize;
extern CFeeRate minRelayTxFee;
extern bool fAlerts;
struct COrphanBlock;
extern std::map<uint256, COrphanBlock*> mapOrphanBlocks;

/** Best header we've seen so far (used for getheaders queries' starting points). */
extern CBlockIndex *pindexBestHeader;

//! Settings
extern unsigned int nDerivationMethodIndex;

extern bool fMinimizeCoinAge;

//! Minimum disk space required - used in CheckDiskSpace()
static const uint64_t nMinDiskSpace = 52428800;

/** Register a wallet to receive updates from core */
void RegisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister a wallet from core */
void UnregisterValidationInterface(CValidationInterface* pwalletIn);
/** Unregister all wallets from core */
void UnregisterAllValidationInterfaces();
/** Push an updated transaction to all registered wallets */
void SyncWithWallets(const CTransaction& tx, const CBlock* pblock = NULL);
/** Ask wallets to resend their transactions */
void ResendWalletTransactions(bool fForce = false);

/** Register with a network node to receive its signals */
void RegisterNodeSignals(CNodeSignals& nodeSignals);
/** Unregister a network node */
void UnregisterNodeSignals(CNodeSignals& nodeSignals);

void PushGetBlocks(CNode* pnode, CBlockIndex* pindexBegin, uint256 hashEnd);

/** Process an incoming block. This only returns after the best known valid
    block is made active. Note that it does not, however, guarantee that the
    specific block passed to it has been checked for validity!
    @param[out]  state   This may be set to an Error state if any error occurred processing it, including during validation/connection/etc of otherwise unrelated blocks during reorganisation; or it may be set to an Invalid state iff pblock is itself invalid (but this is not guaranteed even when the block is checked). If you want to *possibly* get feedback on whether pblock is valid, you must also install a CValidationInterface - this will have its BlockChecked method called whenever *any* block completes validation.
    @param[in]   pfrom   The node which we are receiving the block from; it is added to mapBlockSource and may be penalised if the block is invalid.
    @param[in]   pblock  The block we want to process.
    @param[out]  dbp     If pblock is stored to disk (or already there), this will be set to its location.
    @return True if state.IsValid()
*/
bool ProcessNewBlock(CValidationState& state, CNode* pfrom, CBlock* pblock, CDiskBlockPos *dbp = NULL);
/** Check whether enough disk space is available for an incoming block */
bool CheckDiskSpace(uint64_t nAdditionalBytes = 0);
/** Open a block file (blk?????.dat) */
FILE* OpenBlockFile(const CDiskBlockPos& pos, bool fReadOnly = false);
/** Open an undo file (rev?????.dat) */
FILE* OpenUndoFile(const CDiskBlockPos& pos, bool fReadOnly = false);
/** Translation to a filesystem path */
boost::filesystem::path GetBlockPosFilename(const CDiskBlockPos& pos, const char* prefix);
/** Import blocks from an external file */
bool LoadExternalBlockFile(FILE* fileIn, CDiskBlockPos* dbp = NULL);
/** Initialize a new block tree database + block data on disk */
bool InitBlockIndex();
/** Load the block tree and coins database from disk */
bool LoadBlockIndex();
/** Unload database information */
void UnloadBlockIndex();
/** Process protocol messages received from a given node */
bool ProcessMessages(CNode* pfrom);
/** Send queued protocol messages to be sent to a give node */
bool SendMessages(CNode* pto);
/** Run an instance of the script checking thread */
void ThreadScriptCheck();
/** Stop the script checking threads */
void ThreadScriptCheckQuit();

bool CheckProofOfWork(uint256 hash, unsigned int nBits);
unsigned int GetNextTargetRequired(const CBlockIndex* pindexLast, bool fProofOfStake);
CAmount GetProofOfWorkReward(const CAmount& nFees);
CAmount GetProofOfStakeReward(int64_t nCoinAge, const CAmount& nFees, unsigned int nHeight);

unsigned int ComputeMinWork(unsigned int nBase, int64_t nTime);
unsigned int ComputeMinStake(unsigned int nBase, int64_t nTime, unsigned int nBlockTime);
bool IsInitialBlockDownload();
std::string GetWarnings(std::string strFor);
/** Retrieve a transaction (from memory pool, or from disk, if possible) */
bool GetTransaction(const uint256& hash, CTransaction& tx, uint256& hashBlock, bool fAllowSlow);
uint256 WantedByOrphan(const COrphanBlock* pblockOrphan);
const CBlockIndex* GetLastBlockIndex(const CBlockIndex* pindex, bool fProofOfStake);
void ThreadStakeMiner(CWallet* pwallet, bool fProofOfStake);
/** Find the best known block, and make it the tip of the block chain */
bool ActivateBestChain(CValidationState& state, CBlock* pblock = NULL);
/** Create a new block index entry for a given block hash */
CBlockIndex* InsertBlockIndex(uint256 hash);

/** (try to) add transaction to memory pool **/
bool AcceptToMemoryPool(CTxMemPool& pool, CValidationState& state, const CTransaction& tx, bool fLimitFree, bool* pfMissingInputsbool, bool fRejectInsaneFee = false);

bool AcceptToMemoryPool(CTxMemPool& pool, CValidationState& state, const CTransaction& tx, bool fLimitFree, bool* pfMissingInputs, bool fRejectInsaneFee, std::string& errorMessage);

bool AcceptableInputs(CTxMemPool& pool, CValidationState& state, const CTransaction& txo, bool fLimitFree, bool* pfMissingInputs);

int GetInputAge(CTxIn& vin);
/** Abort with a message */
bool AbortNode(const std::string& msg, const std::string& userMessage="");
/** Get statistics from node state */
bool GetNodeStateStats(NodeId nodeid, CNodeStateStats& stats);
/** Increase a node's misbehavior score. */
void Misbehaving(NodeId nodeid, int howmuch);
/** Flush all state, indexes and buffers to disk. */
void FlushStateToDisk();
bool IsBlockMasternodePaymentValid(CBlockIndex *pindex, CAmount masternodePayment);
int64_t GetMasternodePayment(int nHeight, int64_t blockValue, CAmount masternodeCollateral);

struct CNodeStateStats {
    int nMisbehavior;
    int nSyncHeight;
    int nCommonHeight;
    std::vector<int> vHeightInFlight;
};

struct CDiskTxPos : public CDiskBlockPos {
    unsigned int nTxOffset; //! after header

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(*(CDiskBlockPos*)this);
        READWRITE(VARINT(nTxOffset));
    }

    CDiskTxPos(const CDiskBlockPos& blockIn, unsigned int nTxOffsetIn) : CDiskBlockPos(blockIn.nFile, blockIn.nPos), nTxOffset(nTxOffsetIn)
    {
    }

    CDiskTxPos()
    {
        SetNull();
    }

    void SetNull()
    {
        CDiskBlockPos::SetNull();
        nTxOffset = 0;
    }
};

CAmount GetMinRelayFee(const CTransaction& tx, unsigned int nBytes);

/**
 * Check whether all inputs of this transaction are valid (no double spends, scripts & sigs, amounts)
 * This does not modify the UTXO set. If pvChecks is not NULL, script checks are pushed onto it
 * instead of being performed inline.
 */
bool CheckInputs(const CTransaction& tx, CValidationState &state, const CCoinsViewCache &view, bool fScriptChecks,
                 unsigned int flags, bool cacheStore, std::vector<CScriptCheck> *pvChecks = NULL);

//! Apply the effects of this transaction on the UTXO set represented by view
void UpdateCoins(const CTransaction& tx, CValidationState& state, CCoinsViewCache& view, CTxUndo& txundo, int nHeight);

//! Context-independent validity checks
bool CheckTransaction(const CTransaction& tx, CValidationState& state);

//! ppcoin: get transaction coin age
bool GetCoinAge(const CTransaction& tx, CValidationState& state, CCoinsViewCache& view, uint64_t& nCoinAge, unsigned int nHeight);


/** Undo information for a CBlock */
class CBlockUndo
{
public:
    std::vector<CTxUndo> vtxundo; //! for all but the coinbase

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(vtxundo);
    }

    bool WriteToDisk(CDiskBlockPos& pos, const uint256& hashBlock);
    bool ReadFromDisk(const CDiskBlockPos& pos, const uint256& hashBlock);
};


/** Closure representing one script verification
 *  Note that this stores references to the spending transaction */
class CScriptCheck
{
private:
    CScript scriptPubKey;
    const CTransaction* ptxTo;
    unsigned int nIn;
    unsigned int nFlags;
    bool cacheStore;
    ScriptError error;

public:
    CScriptCheck(): ptxTo(0), nIn(0), nFlags(0), cacheStore(false), error(SCRIPT_ERR_UNKNOWN_ERROR) {}
    CScriptCheck(const CCoins& txFromIn, const CTransaction& txToIn, unsigned int nInIn, unsigned int nFlagsIn, bool cacheIn) :
        scriptPubKey(txFromIn.vout[txToIn.vin[nInIn].prevout.n].scriptPubKey),
        ptxTo(&txToIn), nIn(nInIn), nFlags(nFlagsIn), cacheStore(cacheIn), error(SCRIPT_ERR_UNKNOWN_ERROR) { }

    bool operator()();

    void swap(CScriptCheck& check)
    {
        scriptPubKey.swap(check.scriptPubKey);
        std::swap(ptxTo, check.ptxTo);
        std::swap(nIn, check.nIn);
        std::swap(nFlags, check.nFlags);
        std::swap(cacheStore, check.cacheStore);
        std::swap(error, check.error);

    }

    ScriptError GetScriptError() const { return error; }
};

/** Check for standard transaction types
    @param[in] mapInputs	Map of previous transactions that have outputs we're spending
    @return True if all inputs (scriptSigs) use only standard transaction forms
    @see CTransaction::FetchInputs
*/
bool AreInputsStandard(const CTransaction& tx, const CCoinsViewCache& mapInputs);

/** Count ECDSA signature operations the old-fashioned (pre-0.6) way
    @return number of sigops this transaction's outputs will produce when spent
    @see CTransaction::FetchInputs
*/
unsigned int GetLegacySigOpCount(const CTransaction& tx);

/** Count ECDSA signature operations in pay-to-script-hash inputs.

    @param[in] mapInputs	Map of previous transactions that have outputs we're spending
    @return maximum number of sigops required to validate this transaction's inputs
    @see CTransaction::FetchInputs
 */
unsigned int GetP2SHSigOpCount(const CTransaction& tx, const CCoinsViewCache& inputs);

/** Check for standard transaction types
    @return True if all outputs (scriptPubKeys) use only standard transaction forms
*/
bool IsStandardTx(const CTransaction& tx, std::string& reason);

bool IsFinalTx(const CTransaction& tx, int nBlockHeight = 0, int64_t nBlockTime = 0);

void UpdateTime(CBlockHeader& block, const CBlockIndex* pindexPrev);

/** Functions for disk access for blocks */
bool WriteBlockToDisk(CBlock& block, CDiskBlockPos& pos);
bool ReadBlockFromDisk(CBlock& block, const CDiskBlockPos& pos);
bool ReadBlockFromDisk(CBlock& block, const CBlockIndex* pindex);

/** Functions for validating blocks and updating the block tree */
/** Undo the effects of this block (with given index) on the UTXO set represented by coins.
 *  In case pfClean is provided, operation will try to be tolerant about errors, and *pfClean
 *  will be true if no problems were found. Otherwise, the return value will be false in case
 *  of problems. Note that in any case, coins may be modified. */
bool DisconnectBlock(CBlock& block, CValidationState& state, CBlockIndex* pindex, CCoinsViewCache& coins, bool* pfClean = NULL);

//! Apply the effects of this block (with given index) on the UTXO set represented by coins
bool ConnectBlock(CBlock& block, CValidationState& state, CBlockIndex* pindex, CCoinsViewCache& coins, bool fJustCheck = false);

//! Context-independent validity checks
bool CheckBlockHeader(const CBlockHeader& block, CValidationState& state, bool fCheckPOW = true);
bool CheckBlock(const CBlock& block, CValidationState& state, bool fCheckPOW = true, bool fCheckMerkleRoot = true, bool fCheckSig = true);


bool UpdateHashProof(CBlock& block, CValidationState& state, CBlockIndex* pindex);

//! Store block on disk
//! if dbp is provided, the file is known to already reside on disk
bool AcceptBlock(CBlock& block, CValidationState& state, CBlockIndex** pindex, CDiskBlockPos* dbp = NULL);
bool AcceptBlockHeader(const CBlockHeader& block, CValidationState& state, CBlockIndex** ppindex= NULL);

//! Metrix: attempt to generate suitable proof-of-stake
bool SignBlock(CBlock& block, CWallet& keystore, int64_t nFees);

//!UTXO: The public key that signs must match the public key associated with the first utxo of the coinstake tx.
bool CheckBlockSignature(const CBlock& block);

class CBlockFileInfo
{
public:
    unsigned int nBlocks;      //! number of blocks stored in file
    unsigned int nSize;        //! number of used bytes of block file
    unsigned int nUndoSize;    //! number of used bytes in the undo file
    unsigned int nHeightFirst; //! lowest height of block in file
    unsigned int nHeightLast;  //! highest height of block in file
    uint64_t nTimeFirst;       //! earliest time of block in file
    uint64_t nTimeLast;        //! latest time of block in file

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion)
    {
        READWRITE(VARINT(nBlocks));
        READWRITE(VARINT(nSize));
        READWRITE(VARINT(nUndoSize));
        READWRITE(VARINT(nHeightFirst));
        READWRITE(VARINT(nHeightLast));
        READWRITE(VARINT(nTimeFirst));
        READWRITE(VARINT(nTimeLast));
    }

    void SetNull()
    {
        nBlocks = 0;
        nSize = 0;
        nUndoSize = 0;
        nHeightFirst = 0;
        nHeightLast = 0;
        nTimeFirst = 0;
        nTimeLast = 0;
    }

    CBlockFileInfo()
    {
        SetNull();
    }

    std::string ToString() const;

    //! update statistics (does not update nSize)
    void AddBlock(unsigned int nHeightIn, uint64_t nTimeIn)
    {
        if (nBlocks == 0 || nHeightFirst > nHeightIn)
            nHeightFirst = nHeightIn;
        if (nBlocks == 0 || nTimeFirst > nTimeIn)
            nTimeFirst = nTimeIn;
        nBlocks++;
        if (nHeightIn > nHeightLast)
            nHeightLast = nHeightIn;
        if (nTimeIn > nTimeLast)
            nTimeLast = nTimeIn;
    }
};

/** Capture information about block/transaction validation */
class CValidationState
{
private:
    enum mode_state {
        MODE_VALID,   //!! everything ok
        MODE_INVALID, //!! network rule violation (DoS value may be set)
        MODE_ERROR,   //!! run-time error
    } mode;
    int nDoS;
    std::string strRejectReason;
    unsigned char chRejectCode;
    bool corruptionPossible;

public:
    CValidationState() : mode(MODE_VALID), nDoS(0), chRejectCode(0), corruptionPossible(false) {}
    bool DoS(int level, bool ret = false, unsigned char chRejectCodeIn = 0, std::string strRejectReasonIn = "", bool corruptionIn = false)
    {
        chRejectCode = chRejectCodeIn;
        strRejectReason = strRejectReasonIn;
        corruptionPossible = corruptionIn;
        if (mode == MODE_ERROR)
            return ret;
        nDoS += level;
        mode = MODE_INVALID;
        return ret;
    }
    bool Invalid(bool ret = false,
                 unsigned char _chRejectCode = 0,
                 std::string _strRejectReason = "")
    {
        return DoS(0, ret, _chRejectCode, _strRejectReason);
    }
    bool Error(std::string strRejectReasonIn = "")
    {
        if (mode == MODE_VALID)
            strRejectReason = strRejectReasonIn;
        mode = MODE_ERROR;
        return false;
    }
    bool Abort(const std::string& msg)
    {
        AbortNode(msg);
        return Error(msg);
    }
    bool IsValid() const
    {
        return mode == MODE_VALID;
    }
    bool IsInvalid() const
    {
        return mode == MODE_INVALID;
    }
    bool IsError() const
    {
        return mode == MODE_ERROR;
    }
    bool IsInvalid(int& nDoSOut) const
    {
        if (IsInvalid()) {
            nDoSOut = nDoS;
            return true;
        }
        return false;
    }
    bool CorruptionPossible() const
    {
        return corruptionPossible;
    }
    unsigned char GetRejectCode() const { return chRejectCode; }
    std::string GetRejectReason() const { return strRejectReason; }
};

/** RAII wrapper for VerifyDB: Verify consistency of the block and coin databases */
class CVerifyDB
{
public:
    CVerifyDB();
    ~CVerifyDB();
    bool VerifyDB(CCoinsView* coinsview, int nCheckLevel, int nCheckDepth);
};

class CValidationInterface
{
protected:
    virtual void SyncTransaction(const CTransaction& tx, const CBlock* pblock) {};
    virtual void EraseFromWallet(const uint256& hash) {};
    virtual void SetBestChain(const CBlockLocator& locator) {};
    virtual void UpdatedTransaction(const uint256& hash) {};
    virtual void Inventory(const uint256& hash) {};
    virtual void ResendWalletTransactions(bool fForce) {};
    virtual void BlockChecked(const CBlock&, const CValidationState&) {};
    friend void ::RegisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterValidationInterface(CValidationInterface*);
    friend void ::UnregisterAllValidationInterfaces();
};

/** Global variable that points to the active CCoinsView (protected by cs_main) */
extern CCoinsViewCache* pcoinsTip;
/** Global variable that points to the active block tree (protected by cs_main) */
extern CBlockTreeDB* pblocktree;
struct CBlockTemplate {
    CBlock block;
    std::vector<CAmount> vTxFees;
    std::vector<int64_t> vTxSigOps;
};

/** Mark a block as invalid. */
bool InvalidateBlock(CValidationState& state, CBlockIndex *pindex);

/** Remove invalidity status from a block and its descendants. */
bool ReconsiderBlock(CValidationState& state, CBlockIndex *pindex);

/** Find the last common block between the parameter chain and a locator. */
CBlockIndex* FindForkInGlobalIndex(const CChain& chain, const CBlockLocator& locator);

/** The currently-connected chain of blocks. */
extern CChain chainActive;

#endif // BITCOIN_MAIN_H