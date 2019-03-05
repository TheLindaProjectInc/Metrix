// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CHAIN_PARAMS_H
#define BITCOIN_CHAIN_PARAMS_H

#include "bignum.h"
#include "uint256.h"
#include "core.h"
#include "protocol.h"

#include <vector>

using namespace std;

typedef unsigned char MessageStartChars[MESSAGE_START_SIZE];


struct CDNSSeedData {
    string name, host;
    CDNSSeedData(const string &strName, const string &strHost) : name(strName), host(strHost) {}
};

/**
 * CChainParams defines various tweakable parameters of a given instance of the
 * Bitcoin system. There are three: the main network on which people trade goods
 * and services, the public test network which gets reset from time to time and
 * a regression test mode which is intended for private networks only. It has
 * minimal difficulty to ensure that blocks can be found instantly.
 */
class CChainParams
{
public:
    enum Network {
        MAIN,
        TESTNET,
        REGTEST,

        MAX_NETWORK_TYPES
    };

    enum Base58Type {
        PUBKEY_ADDRESS,
        SCRIPT_ADDRESS,
        SECRET_KEY,
        EXT_PUBLIC_KEY,
        EXT_SECRET_KEY,

        MAX_BASE58_TYPES
    };

    const uint256& HashGenesisBlock() const { return hashGenesisBlock; }
    const MessageStartChars& MessageStart() const { return pchMessageStart; }
    const vector<unsigned char>& AlertKey() const { return vAlertPubKey; }
    int GetDefaultPort() const { return nDefaultPort; }
    const CBigNum& ProofOfWorkLimit() const { return bnProofOfWorkLimit; }
    int SubsidyHalvingInterval() const { return nSubsidyHalvingInterval; }
	const CBlock& GenesisBlock() const { return genesis; };
	bool RequireRPCPassword() const { return fRequireRPCPassword; }
    /* Default value for -checkmempool argument */
	bool DefaultCheckMemPool() const { return fDefaultCheckMemPool; }
	/* Make standard checks */
	bool RequireStandard() const { return fRequireStandard; }
	/* Make standard checks */
	bool RPCisTestNet() const { return fRPCisTestNet; }
    const string& DataDir() const { return strDataDir; }
	Network NetworkID() const { return networkID; }
    const vector<CDNSSeedData>& DNSSeeds() const { return vSeeds; }
	const std::vector<unsigned char>& Base58Prefix(Base58Type type) const { return base58Prefixes[type]; }
	const vector<CAddress>& FixedSeeds() const { return vFixedSeeds; }
    int RPCPort() const { return nRPCPort; }
    int LastPOWBlock() const { return nLastPOWBlock; }
protected:
    CChainParams() {};

    uint256 hashGenesisBlock;
    MessageStartChars pchMessageStart;
    // Raw pub key bytes for the broadcast alert signing key.
    vector<unsigned char> vAlertPubKey;
    int nDefaultPort;
    int nRPCPort;
    CBigNum bnProofOfWorkLimit;
    int nSubsidyHalvingInterval;
    string strDataDir;
    vector<CDNSSeedData> vSeeds;
    std::vector<unsigned char> base58Prefixes[MAX_BASE58_TYPES];
    int nLastPOWBlock;
	Network networkID;
	CBlock genesis;
	vector<CAddress> vFixedSeeds;
	bool fRequireRPCPassword;
	bool fDefaultCheckMemPool;
	bool fRequireStandard;
	bool fRPCisTestNet;
};

/**
 * Return the currently selected parameters. This won't change after app startup
 * outside of the unit tests.
 */
const CChainParams &Params();

/** Sets the params returned by Params() to those for the given network. */
void SelectParams(CChainParams::Network network);

/**
 * Looks for -regtest or -testnet and then calls SelectParams as appropriate.
 * Returns false if an invalid combination is given.
 */
bool SelectParamsFromCommandLine();

#endif