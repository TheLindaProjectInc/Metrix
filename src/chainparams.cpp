// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/consensus.h>
#include <consensus/merkle.h>
#include <tinyformat.h>
#include <util/convert.h>
#include <util/strencodings.h>
#include <util/system.h>
#include <versionbitsinfo.h>

#include <assert.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

///////////////////////////////////////////// // qtum
#include "arith_uint256.h"
#include <libdevcore/RLP.h>
#include <libdevcore/SHA3.h>
/////////////////////////////////////////////

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const std::string network)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 00 << 488804799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime = nTime;
    genesis.nBits = nBits;
    genesis.nNonce = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    if (network == CBaseChainParams::MAIN) {
        genesis.hashStateRoot = uint256(h256Touint(dev::h256("e320303a3f7937015d93885a5387ba30ab2874ce77752a7a19929a71ea6052de"))); // metrix
    } else {
        genesis.hashStateRoot = uint256(h256Touint(dev::h256("6ea8bd3931247c33bb5adc02ae5dba0719c890c3e222c0250a681c8a384da051"))); // metrix
    }
    genesis.hashUTXORoot = uint256(h256Touint(dev::sha3(dev::rlp(""))));                                                            // metrix
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000057c278d417, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=5f121d, nTime=1600679206, nBits=1d00ffff, nNonce=147545, vtx=1)
 *   CTransaction(hash=5f121d, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 0004bf91221d01044c4e4254432023363236333735202d203030303030303030303030303030303030303061626131393536663264303163623531343162373162343137353035323066393436)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x410462aa66c43fba23f7fb)
 *   vMerkleTree: 5f121d
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward, const std::string network)
{
    const char* pszTimestamp = "BTC #626375 - 0000000000000000000aba1956f2d01cb5141b71b41750520f94614e23b2ea82";
    const CScript genesisOutputScript = CScript() << ParseHex("0462aa66c43fba23f7fb32cebb7a6dfbda5b5aa30c5d0ecf19d64a1be94fe8451dbf631cb0c63e21c31374c3abaaee2892e1c11631b7b56f0e9cc08d6fb80512ec") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward, network);
}

/**
 * Main network
 */
class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        strNetworkID = "main";
        consensus.nSubsidyHalvingInterval = 0; // metrix unused
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 2016; // segwit activation height + miner confirmation window
        consensus.QIP5Height = 0;
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0;
        consensus.QIP9Height = 5000;
        consensus.minMIP1Height = 264694;    // height of chain path correction due to mismatched AAL tx in next block
        consensus.MIP1Height = 332640;       // Activation block: b20afef871e4d7968b57054d9878d5189258cbd90d278245e718eeebf71fbd96
        consensus.minMIP2Height = 681821;    // height of chain path correction due to goverance contract bug
        consensus.MIP2Height = 685000;       // Activation block: f307d959abb16a49ded03486c2074128de3602cfd46a9b5369060d1d95e8c6e5
        consensus.MIP3StartHeight = 702000;  // Start checking for MIP3 - after last halving (701280)
        consensus.MIP3Height = 766080;       // Activation block: 04ff78f5f3cae51ef9f5c817a2c5c113d3668c7a07cbb9df1e22201ff43ac9e6
        consensus.MIP4StartHeight = 920100;  // Start checking for MIP4
        consensus.MIP4Height = -1;           // Activation block:
        consensus.MIP5StartHeight = 990100;  // Start checking for MIP5
        consensus.MIP5Height = -1;           // Activation block:
        consensus.MIP6StartHeight = 1020100; // Start checking for MIP6
        consensus.MIP6Height = -1;           // Activation block:
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("0000000000001fffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9

        consensus.nPowTargetTimespan = 16 * 60;                                                                // 16 minutes
        consensus.nPowTargetTimespanV2 = 4000;                                                                 // 66 minutes
        consensus.nPowTargetSpacing = 90;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 6384; // BIP9 activate at 95% of 6720
        consensus.nMinerConfirmationWindow = 6720;       // BIP9 confirmation window size
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;                                    // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;                                      // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].bit = 27;                                            // REMOVE Jan 1st 2024!
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartTime = 1662777888;                             // Semptember 10, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;    // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].bit = 28;                                              // REMOVE after Jan 1st 2026!
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartHeight = consensus.MIP4StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartTime = 1682914801;                               // May 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;      // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].bit = 25;                                            // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartHeight = consensus.MIP5StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartTime = 1690858741;                             // Aug 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;    // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].bit = 21;                                         // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartHeight = consensus.MIP6StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartTime = 1695265141;                          // Sept 21, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // No timeout

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000150eb4505eac09858445f"); // Metrix (950000)

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xd290afcbb4d7ab014ba9bd12f20f6a6fc7119e57badbf5a30914c1bc3c0ee342"); // 950000

        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 32-bit integer with any alignment.
         */
        pchMessageStart[0] = 0x9d;
        pchMessageStart[1] = 0xd4;
        pchMessageStart[2] = 0x18;
        pchMessageStart[3] = 0x02;
        nDefaultPort = 33830;
        nPruneAfterHeight = 100000;
        m_assumed_blockchain_size = 8;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1600679206, 147545, 0x1f00ffff, 1, 50 * COIN, CBaseChainParams::MAIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x000057c278d417dfca66d2bcde29d4875b06ea7f5935086bf4f1f968c91c3377"));
        assert(genesis.hashMerkleRoot == uint256S("0x5f121ddfab7e4c75f9cf10d106356e9be38e9567c778390153d7ca8eae03607d"));

        // Note that of those which support the service bits prefix, most only support a subset of
        // possible options.
        // This is fine at runtime as we'll fall back to using them as a oneshot if they don't support the
        // service bits we want, but we should get them updated to support all service bits wanted by any
        // release ASAP to avoid it where possible.
        vSeeds.emplace_back("mainnet.metrixcoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 50);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 85);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 153);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};

        bech32_hrp = "mc";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        m_is_test_chain = false;

        checkpointData = {
            {
                {0, uint256S("0x000057c278d417dfca66d2bcde29d4875b06ea7f5935086bf4f1f968c91c3377")},
                {5000, uint256S("0x000042b868836aac2796bea960bbea18db6cdab2fc465e98fc01d042c52698e1")},
                {100000, uint256S("0x277983da1f28d72092f1af55521381e9d20ce6c5687715a04ebbfeb295570109")},
                {173427, uint256S("0xd44848414844ded450917c081176c934604dd3c7c69d920be8a055762691fe17")},
                {199500, uint256S("0x0efec94703a3e37f4a053611d2745eb5429092c391e54153a4771f0d695ab833")},
                {235885, uint256S("0x8000d7ba64df0848b1b4f3d89737e5573307363a3feac83d664524adb5ce632b")},
                {264694, uint256S("0xa11eae2326f4d3162b4f3e264b38a1cdc0e0cc5c77b6319ba6f2b59bf511f6cd")},
                {264695, uint256S("0x4ebc8cc894f27303995a1b948f8b3c52b6493cd6663c70204a909474cb20ced6")},
                {265123, uint256S("0x0ba351d906584177f27c681592b97f896a80c04b955f1e74e03f9611c5aa97f5")},
                {305037, uint256S("0x84f857128fe6a4d762323a33799e5d3609db4445be9f0481606f384593028cf3")},
                {350000, uint256S("0xd89b4a626819d49c1a938a3d6adbfac2abe1f380e5d2a6ae23ae7d922403a587")},
                {450000, uint256S("0xe7760f9c7b7d20c3acee3e8185626d832da3d14a7b25a2509208ca70f60d43dd")},
                {520000, uint256S("0xfc3f607b5c97bc94e1fb50f3beb473eb06b65ffc21b2efa77cdcb1920482e211")},
                {681822, uint256S("0xf89b90fe4d34509ce5fa2d6a56f4ff80bdc7e0637a49b250ed9eb102a8516479")},
                {683449, uint256S("0x3c11441e82b4d380984ced480c71eec6f26967a3e6283b00ae232553b7fd9224")},
                {685001, uint256S("0xff9ea87783076d9f17526b6fb6e78fbf29dacf74a255fd618e1b281d99687639")},
                {685011, uint256S("0xbd948c547a6c03f2494fed4360b16c42db7df49710edaa82085924fe5587d120")},
                {701281, uint256S("0x0385e7028c76cfff95b8d2b4cebfa49ac90cc5a986ba90ce1cfa65786744ee4f")},
                {706252, uint256S("0x20a682b85de3f3d66edb5850b16803a4a3991cf24fdb6bf97d12367f7876d3b5")},
                {831858, uint256S("0x2d3819e8744490f900e8b2e49b752b56c5f4685de4418209fdb3f88b22d6f270")},
                {950000, uint256S("0xd290afcbb4d7ab014ba9bd12f20f6a6fc7119e57badbf5a30914c1bc3c0ee342")},
            }};

        chainTxData = ChainTxData{
            // Data as of block 2d3819e8744490f900e8b2e49b752b56c5f4685de4418209fdb3f88b22d6f270 (height 831858)
            1676156112, // * UNIX timestamp of last known number of transactions
            2668702,    // * total number of transactions between genesis and that timestamp
            //   (the tx=... number in the SetBestChain debug.log lines)
            0.10018604651162790 // * estimated number of transactions per second after that timestamp
        };

        consensus.nLastPOWBlock = 5000;
        consensus.nMPoSRewardRecipients = 10;
        consensus.nFirstMPoSBlock = 0x7FFFFFFF; // metrix currently disabled

        consensus.nFixUTXOCacheHFHeight = 0;
        consensus.nEnableHeaderSignatureHeight = 0;
        consensus.nCheckpointSpan = COINBASE_MATURITY;
    }
};

/**
 * Testnet (v1)
 */
class CTestNetParams : public CChainParams
{
public:
    CTestNetParams()
    {
        strNetworkID = "test";
        consensus.nSubsidyHalvingInterval = 0; // metrix unused
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0;
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;
        consensus.BIP66Height = 0;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 0;
        consensus.MinBIP9WarningHeight = 2016; // segwit activation height + miner confirmation window
        consensus.QIP5Height = 0;
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0;
        consensus.QIP9Height = 5000;
        consensus.minMIP1Height = 264694;    // this has no effect on testnet..
        consensus.MIP1Height = 389088;       // b5a3e290a9f5419b2658d9e0d0b7659d505b8322c320b0a041caf56bfb7bbefc
        consensus.minMIP2Height = 681822;    // height of chain path correction due to goverance contract bug
        consensus.MIP2Height = 685000;       // Activation block: this doesnt effect testnet?
        consensus.MIP3StartHeight = 700000;  // Start checking for MIP3
        consensus.MIP3Height = 725760;       // Activation height for MIP3
        consensus.MIP4StartHeight = 800100;  // Start checking for MIP4
        consensus.MIP4Height = -1;           // Not used
        consensus.MIP5StartHeight = 920100;  // Start checking for MIP5
        consensus.MIP5Height = -1;           // Activation block:
        consensus.MIP6StartHeight = 970700;  // Start checking for MIP6
        consensus.MIP6Height = -1;           // Activation block:
        consensus.powLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("0000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("0000000000001fffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9

        consensus.nPowTargetTimespan = 16 * 60;                                                                // 16 minutes
        consensus.nPowTargetTimespanV2 = 4000;
        consensus.nPowTargetSpacing = 90;
        consensus.fPowAllowMinDifficultyBlocks = false;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 1512; // 75% for testchains
        consensus.nMinerConfirmationWindow = 2016;       // nPowTargetTimespan / nPowTargetSpacing

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1199145601;                                 // January 1, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1230767999;                                   // December 31, 2008
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].bit = 27;                                         // REMOVE Jan 1st 2024!
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartTime = 1662777888;                          // Semptember 10, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartHeight = consensus.MIP4StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartTime = 1682914801;                               // May 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;      // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].bit = 25;                                            // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartHeight = consensus.MIP5StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartTime = 1682914801;                             // Aug 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;    // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].bit = 21;                                         // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartHeight = consensus.MIP6StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartTime = 1682914801;                          // Sept 21, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // No timeout

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00000000000000000000000000000000000000000001d7ca9fb8adfb66714f70"); // metrix 970000

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0xfdd0c420bb7d0f7de4e31c206722f128e133acf0d2b483f7869af17a5b955675"); // metrix 970000

        pchMessageStart[0] = 0xcf;
        pchMessageStart[1] = 0xf3;
        pchMessageStart[2] = 0xc1;
        pchMessageStart[3] = 0xe1;
        nDefaultPort = 33840;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 2;
        m_assumed_chain_state_size = 1;

        genesis = CreateGenesisBlock(1587113529, 54191, 0x1f00ffff, 1, 50 * COIN, CBaseChainParams::TESTNET);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x00007965374994a1a9e0c813c0d935c3ca224dd1cc582d09f11afe134877adf6"));
        assert(genesis.hashMerkleRoot == uint256S("0x5f121ddfab7e4c75f9cf10d106356e9be38e9567c778390153d7ca8eae03607d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top
        vSeeds.emplace_back("testnet1.metrixcoin.com");

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 110);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 187);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "tm";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        m_is_test_chain = true;


        checkpointData = {
            {
                {0, uint256S("0x00007965374994a1a9e0c813c0d935c3ca224dd1cc582d09f11afe134877adf6")},
                {79475, uint256S("0x72ddc370485496c1d2f96d52d00490dbb586060e770dfa21d139e5af7126dfd9")},
                {200000, uint256S("0xcc9a430bee8496b7975cc2e6c70d4f65fdba812f4c4ea0ba7005ccf2426d35a2")},
                {400000, uint256S("0xb06b177e948c1f32f6fa0f892c30b931b61334abbe21e4e5bbde5ded74f28fed")},
                {700000, uint256S("0x6b425fbb117ec454758cc03aaa90f1d73deef90b7bb62b529a99e4f17d371f2c")},
                {970000, uint256S("0xfdd0c420bb7d0f7de4e31c206722f128e133acf0d2b483f7869af17a5b955675")},
            }};

        chainTxData = ChainTxData{
            // Data as of block 72ddc370485496c1d2f96d52d00490dbb586060e770dfa21d139e5af7126dfd9 (height 79475)
            1600679856,
            228567,
            0.01624824080589608};

        consensus.nLastPOWBlock = 5000;
        consensus.nMPoSRewardRecipients = 10;
        consensus.nFirstMPoSBlock = 0x7FFFFFFF; // metrix currently disabled

        consensus.nFixUTXOCacheHFHeight = 0;
        consensus.nEnableHeaderSignatureHeight = 0;
        consensus.nCheckpointSpan = COINBASE_MATURITY;
    }
};

/**
 * Regression test
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const ArgsManager& args)
    {
        strNetworkID = "regtest";
        consensus.nSubsidyHalvingInterval = 0; // metrix unused
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 0;             // BIP34 activated on regtest (Used in functional tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 0;             // BIP65 activated on regtest (Used in functional tests)
        consensus.BIP66Height = 0;             // BIP66 activated on regtest (Used in functional tests)
        consensus.CSVHeight = 1;               // CSV activated on regtest (Used in rpc activation tests)
        consensus.SegwitHeight = 0;            // SEGWIT is always activated on regtest unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.QIP5Height = 0;
        consensus.QIP6Height = 0;
        consensus.QIP7Height = 0;
        consensus.QIP9Height = 1000;
        consensus.minMIP1Height = 0;
        consensus.MIP1Height = 0;
        consensus.minMIP2Height = 0;
        consensus.MIP2Height = 0;
        consensus.MIP3StartHeight = 1500;
        consensus.MIP3Height = 1550;
        consensus.MIP4StartHeight = 2000;
        consensus.MIP4Height = -1;
        consensus.MIP5StartHeight = 3000; // Start checking for MIP5
        consensus.MIP5Height = -1;        // Activation block:
        consensus.MIP6StartHeight = 4200; // Start checking for MIP6
        consensus.MIP6Height = -1;        // Activation block:
        consensus.powLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.posLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.QIP9PosLimit = uint256S("7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"); // The new POS-limit activated after QIP9

        consensus.nPowTargetTimespan = 16 * 60;                                                                // 16 minutes (960 = 832 + 128; multiplier is 832)
        consensus.nPowTargetTimespanV2 = 4000;
        consensus.nPowTargetSpacing = 90;
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.fPowNoRetargeting = true;
        consensus.fPoSNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 216; // 75% for testchains
        consensus.nMinerConfirmationWindow = 288;       // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].bit = 27;                                         // REMOVE Jan 1st 2024!
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartHeight = -1;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nStartTime = 1662484496;                          // Semptember 10, 2022
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP3_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].bit = 28;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartHeight = consensus.MIP4StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nStartTime = 1675225201;                               // Feb 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP4_FORK_SPAN].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;      // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].bit = 25;                                            // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartHeight = consensus.MIP5StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nStartTime = 1675225201;                             // Aug 1, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP5_DGP_UPGRADE].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;    // No timeout
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].bit = 21;                                         // REMOVE after Jan 1st 2026?
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartHeight = consensus.MIP6StartHeight;
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nStartTime = 1675225201;                          // Sept 21, 2023
        consensus.vDeployments[Consensus::DEPLOYMENT_MIP6_REM_LEGACY_DGP].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT; // No timeout

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        // By default assume that the signatures in ancestors of this block are valid.
        consensus.defaultAssumeValid = uint256S("0x00");

        pchMessageStart[0] = 0xce;
        pchMessageStart[1] = 0xf2;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0xef;
        nDefaultPort = 33850;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        UpdateActivationParametersFromArgs(args);

        genesis = CreateGenesisBlock(1587113529, 1, 0x207fffff, 1, 50 * COIN, CBaseChainParams::REGTEST);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x7d3cee5e4d85664505bc079810796a18f56f5aa4941b617d144150a25c24fe51"));
        assert(genesis.hashMerkleRoot == uint256S("0x5f121ddfab7e4c75f9cf10d106356e9be38e9567c778390153d7ca8eae03607d"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();      //!< Regtest mode doesn't have any DNS seeds.

        fDefaultConsistencyChecks = true;
        fRequireStandard = true;
        m_is_test_chain = true;

        checkpointData = {
            {}};

        chainTxData = ChainTxData{
            0,
            0,
            0};
        consensus.nLastPOWBlock = 0x7fffffff;
        consensus.nMPoSRewardRecipients = 10;
        consensus.nFirstMPoSBlock = 0x7FFFFFFF; // metrix currently disabled

        consensus.nFixUTXOCacheHFHeight = 0;
        consensus.nEnableHeaderSignatureHeight = 0;
        consensus.nCheckpointSpan = COINBASE_MATURITY;

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 112);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 187);
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "mcrt";
    }

    /**
     * Allows modifying the Version Bits regtest parameters.
     */
    void UpdateVersionBitsParameters(Consensus::DeploymentPos d, int64_t nStartTime, int64_t nTimeout)
    {
        consensus.vDeployments[d].nStartTime = nStartTime;
        consensus.vDeployments[d].nTimeout = nTimeout;
    }
    void UpdateActivationParametersFromArgs(const ArgsManager& args);
};

void CRegTestParams::UpdateActivationParametersFromArgs(const ArgsManager& args)
{
    if (gArgs.IsArgSet("-segwitheight")) {
        int64_t height = gArgs.GetArg("-segwitheight", consensus.SegwitHeight);
        if (height < -1 || height >= std::numeric_limits<int>::max()) {
            throw std::runtime_error(strprintf("Activation height %ld for segwit is out of valid range. Use -1 to disable segwit.", height));
        } else if (height == -1) {
            LogPrintf("Segwit disabled for testing\n");
            height = std::numeric_limits<int>::max();
        }
        consensus.SegwitHeight = static_cast<int>(height);
    }

    if (!args.IsArgSet("-vbparams")) return;

    for (const std::string& strDeployment : args.GetArgs("-vbparams")) {
        std::vector<std::string> vDeploymentParams;
        boost::split(vDeploymentParams, strDeployment, boost::is_any_of(":"));
        if (vDeploymentParams.size() != 3) {
            throw std::runtime_error("Version bits parameters malformed, expecting deployment:start:end");
        }
        int64_t nStartTime, nTimeout;
        if (!ParseInt64(vDeploymentParams[1], &nStartTime)) {
            throw std::runtime_error(strprintf("Invalid nStartTime (%s)", vDeploymentParams[1]));
        }
        if (!ParseInt64(vDeploymentParams[2], &nTimeout)) {
            throw std::runtime_error(strprintf("Invalid nTimeout (%s)", vDeploymentParams[2]));
        }
        bool found = false;
        for (int j = 0; j < (int)Consensus::MAX_VERSION_BITS_DEPLOYMENTS; ++j) {
            if (vDeploymentParams[0] == VersionBitsDeploymentInfo[j].name) {
                UpdateVersionBitsParameters(Consensus::DeploymentPos(j), nStartTime, nTimeout);
                found = true;
                LogPrintf("Setting version bits activation parameters for %s to start=%ld, timeout=%ld\n", vDeploymentParams[0], nStartTime, nTimeout);
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(strprintf("Invalid deployment (%s)", vDeploymentParams[0]));
        }
    }
}

/**
 * Regression network parameters overwrites for unit testing
 */
class CUnitTestParams : public CRegTestParams
{
public:
    explicit CUnitTestParams(const ArgsManager& args)
        : CRegTestParams(args)
    {
        // Activate the the BIPs for regtest as in Bitcoin
        consensus.BIP16Exception = uint256();
        consensus.BIP34Height = 100000000; // BIP34 has not activated on regtest (far in the future so block v1 are not rejected in tests)
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1351;      // BIP65 activated on regtest (Used in rpc activation tests)
        consensus.BIP66Height = 1251;      // BIP66 activated on regtest (Used in rpc activation tests)
        consensus.QIP6Height = 1000;
        consensus.QIP7Height = 0;          // QIP7 activated on regtest

        consensus.nSubsidyHalvingInterval = 0;
        consensus.nRuleChangeActivationThreshold = 558; // 75% for testchains
        consensus.nMinerConfirmationWindow = 744;       // Faster than normal for regtest (744 instead of 2016)
        consensus.nCheckpointSpan = 1000;               // Increase the check point span for the reorganization tests from 500 to 1000
    }
};

static std::unique_ptr<const CChainParams> globalChainParams;

const CChainParams& Params()
{
    assert(globalChainParams);
    return *globalChainParams;
}

std::unique_ptr<const CChainParams> CreateChainParams(const std::string& chain)
{
    if (chain == CBaseChainParams::MAIN)
        return std::unique_ptr<CChainParams>(new CMainParams());
    else if (chain == CBaseChainParams::TESTNET)
        return std::unique_ptr<CChainParams>(new CTestNetParams());
    else if (chain == CBaseChainParams::REGTEST)
        return std::unique_ptr<CChainParams>(new CRegTestParams(gArgs));
    else if (chain == CBaseChainParams::UNITTEST)
        return std::unique_ptr<CChainParams>(new CUnitTestParams(gArgs));
    throw std::runtime_error(strprintf("%s: Unknown chain %s.", __func__, chain));
}

void SelectParams(const std::string& network)
{
    SelectBaseParams(network);
    globalChainParams = CreateChainParams(network);
}

std::string CChainParams::EVMGenesisInfo(dev::eth::Network network) const
{
    std::string genesisInfo = dev::eth::genesisInfo(network);
    ReplaceInt(consensus.QIP7Height, "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(consensus.QIP6Height, "QIP6_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

std::string CChainParams::EVMGenesisInfo(dev::eth::Network network, int nHeight) const
{
    std::string genesisInfo = dev::eth::genesisInfo(network);
    ReplaceInt(nHeight, "QIP7_STARTING_BLOCK", genesisInfo);
    ReplaceInt(nHeight, "QIP6_STARTING_BLOCK", genesisInfo);
    return genesisInfo;
}

void CChainParams::UpdateOpSenderBlockHeight(int nHeight)
{
    consensus.QIP5Height = nHeight;
}

void UpdateOpSenderBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateOpSenderBlockHeight(nHeight);
}

void CChainParams::UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    consensus.QIP6Height = nHeight;
}

void UpdateBtcEcrecoverBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateBtcEcrecoverBlockHeight(nHeight);
}

void CChainParams::UpdateConstantinopleBlockHeight(int nHeight)
{
    consensus.QIP7Height = nHeight;
}

void UpdateConstantinopleBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateConstantinopleBlockHeight(nHeight);
}

void CChainParams::UpdateDifficultyChangeBlockHeight(int nHeight)
{
    consensus.nSubsidyHalvingInterval = 0; // metrix unused
    consensus.posLimit = uint256S("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    consensus.QIP9Height = nHeight;
    consensus.fPowAllowMinDifficultyBlocks = false;
    consensus.fPowNoRetargeting = true;
    consensus.fPoSNoRetargeting = false;
    consensus.nLastPOWBlock = 5000;
    consensus.nMPoSRewardRecipients = 10;
    consensus.nFirstMPoSBlock = 0x7FFFFFFF; // metrix currently disabled
}

void UpdateDifficultyChangeBlockHeight(int nHeight)
{
    const_cast<CChainParams*>(globalChainParams.get())->UpdateDifficultyChangeBlockHeight(nHeight);
}
