// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <limits>
#include <map>
#include <string>
#include <uint256.h>

namespace Consensus {

enum DeploymentPos {
    DEPLOYMENT_TESTDUMMY,
    DEPLOYMENT_MIP3_DGP_UPGRADE,
    DEPLOYMENT_MIP4_FORK_SPAN,
    DEPLOYMENT_MIP5_DGP_UPGRADE,
    DEPLOYMENT_MIP6_REM_LEGACY_DGP,
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;
    /** Start block height for version bits miner confirmation.*/
    int64_t nStartHeight;

    /** Constant for nTimeout very far in the future. */
    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();

    /** Special value for nStartTime indicating that the deployment is always active.
     *  This is useful for testing, as it means tests don't need to deal with the activation
     *  process (which takes at least 3 BIP9 intervals). Only tests that specifically test the
     *  behaviour during activation cannot use this. */
    static constexpr int64_t ALWAYS_ACTIVE = -1;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /* Block hash that is excepted from BIP16 enforcement */
    uint256 BIP16Exception;
    /** Block height and hash at which BIP34 becomes active */
    int BIP34Height;
    uint256 BIP34Hash;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /** Block height at which CSV (BIP68, BIP112 and BIP113) becomes active */
    int CSVHeight;
    /** Block height at which Segwit (BIP141, BIP143 and BIP147) becomes active.
     * Note that segwit v0 script rules are enforced on all blocks except the
     * BIP 16 exception blocks. */
    int SegwitHeight;
    /** Don't warn about unknown BIP 9 activations below this height.
     * This prevents us from warning about the CSV and segwit activations. */
    int MinBIP9WarningHeight;
    /** Block height at which QIP5 becomes active */
    int QIP5Height;
    /** Block height at which QIP6 becomes active */
    int QIP6Height;
    /** Block height at which QIP7 becomes active */
    int QIP7Height;
    /** Block height at which QIP9 becomes active */
    int QIP9Height;
    /** Block height at which MIP1 becomes needed-
     *  At this block the chain forked due to mismatched AAL tx in next block.
     *  Override check extends 7 blocks beyond minimum height for correction. */
    int minMIP1Height;
    /** Block height at which MIP1 becomes active */
    int MIP1Height;
    /** Block height at which MIP2 becomes needed-
     *  At this block the chain procudes invalid blocks due to goverance bug. */
    int minMIP2Height;
    /** Block height at which MIP2 becomes active */
    int MIP2Height;
    /** Block height at which MIP3 becomes available */
    int MIP3StartHeight;
    /** Block height at which MIP3 becomes active */
    int MIP3Height;
    /** Block height at which MIP4 becomes available */
    int MIP4StartHeight;
    /** Block height at which MIP4 becomes active */
    int MIP4Height;
    /** Block height at which MIP5 becomes available */
    int MIP5StartHeight;
    /** Block height at which MIP5 becomes active */
    int MIP5Height;
    /** Block height at which MIP6 becomes available */
    int MIP6StartHeight;
    /** Block height at which MIP6 becomes active */
    int MIP6Height;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    uint256 powLimit;
    uint256 posLimit;
    uint256 QIP9PosLimit;
    bool fPowAllowMinDifficultyBlocks;
    bool fPowNoRetargeting;
    bool fPoSNoRetargeting;
    int64_t nPowTargetSpacing;
    int64_t nPowTargetTimespan;
    int64_t nPowTargetTimespanV2;
    int64_t nPowTargetTimespanV3;
    int64_t DifficultyAdjustmentInterval(int height) const
    {
        int64_t targetSpacing = nPowTargetTimespan;
        if (height >= QIP9Height) {
            targetSpacing = nPowTargetTimespanV2;
        }
        return targetSpacing / nPowTargetSpacing;
    }
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;
    int nLastPOWBlock;
    int nFirstMPoSBlock;
    int nMPoSRewardRecipients;
    int nFixUTXOCacheHFHeight;
    int nEnableHeaderSignatureHeight;
    /** Block sync-checkpoint span*/
    int nCheckpointSpan;
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
