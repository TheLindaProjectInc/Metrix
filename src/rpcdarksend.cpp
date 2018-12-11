// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Darkcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
//#include "primitives/transaction.h"
#include "db.h"
#include "init.h"
#include "masternodeman.h"
#include "activemasternode.h"
#include "masternodeconfig.h"
#include "rpcserver.h"
#include <boost/lexical_cast.hpp>
//#include "amount.h"
#include "util.h"
//#include "utilmoneystr.h"

#include <fstream>
using namespace json_spirit;
using namespace std;

void SendMoney(const CTxDestination &address, CAmount nValue, CWalletTx& wtxNew, AvailableCoinsType coin_type)
{
    // Check amount
    if (nValue <= 0)
        throw JSONRPCError(RPC_INVALID_PARAMETER, "Invalid amount");

    if (nValue > pwalletMain->GetBalance())
        throw JSONRPCError(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");

    string strError;
    if (pwalletMain->IsLocked())
    {
        strError = "Error: Wallet locked, unable to create transaction!";
        LogPrintf("SendMoney() : %s", strError);
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }

    // Parse Linda address
    CScript scriptPubKey = GetScriptForDestination(address);

    // Create and send the transaction
    CReserveKey reservekey(pwalletMain);
    int64_t nFeeRequired;
    std::string sNarr;
    if (!pwalletMain->CreateTransaction(scriptPubKey, nValue, sNarr, wtxNew, reservekey, nFeeRequired, NULL))
    {
        if (nValue + nFeeRequired > pwalletMain->GetBalance())
            strError = strprintf("Error: This transaction requires a transaction fee of at least %s because of its amount, complexity, or use of recently received funds!", FormatMoney(nFeeRequired));
        LogPrintf("SendMoney() : %s\n", strError);
        throw JSONRPCError(RPC_WALLET_ERROR, strError);
    }
    if (!pwalletMain->CommitTransaction(wtxNew, reservekey))
        throw JSONRPCError(RPC_WALLET_ERROR, "Error: The transaction was rejected! This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here.");
}

Value darksend(const Array& params, bool fHelp)
{
    if (fHelp || params.size() == 0)
        throw runtime_error(
            "darksend <Lindaaddress> <amount>\n"
            "Lindaaddress, reset, or auto (AutoDenominate)"
            "<amount> is a real and is rounded to the nearest 0.00000001"
            + HelpRequiringPassphrase());

    if (pwalletMain->IsLocked())
        throw JSONRPCError(RPC_WALLET_UNLOCK_NEEDED, "Error: Please enter the wallet passphrase with walletpassphrase first.");

    if(params[0].get_str() == "auto"){
        if(fMasterNode)
            return "DarkSend is not supported from masternodes";

        darkSendPool.DoAutomaticDenominating();
        return "DoAutomaticDenominating";
    }

    if(params[0].get_str() == "reset"){
        darkSendPool.SetNull(true);
        darkSendPool.UnlockCoins();
        return "successfully reset darksend";
    }

    if (params.size() != 2)
        throw runtime_error(
            "darksend <Lindaaddress> <amount>\n"
            "Lindaaddress, denominate, or auto (AutoDenominate)"
            "<amount> is a real and is rounded to the nearest 0.00000001"
            + HelpRequiringPassphrase());

    CBitcoinAddress address(params[0].get_str());
    if (!address.IsValid())
        throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, "Invalid Linda address");

    // Amount
    int64_t nAmount = AmountFromValue(params[1]);

    // Wallet comments
    CWalletTx wtx;
    SendMoney(address.Get(), nAmount, wtx, ONLY_DENOMINATED);
   
    return wtx.GetHash().GetHex();
}


Value getpoolinfo(const Array& params, bool fHelp)
{
    if (fHelp || params.size() != 0)
        throw runtime_error(
            "getpoolinfo\n"
            "Returns an object containing anonymous pool-related information.");

    Object obj;
    obj.push_back(Pair("current_masternode",        mnodeman.GetCurrentMasterNode()->addr.ToString()));
    obj.push_back(Pair("state",        darkSendPool.GetState()));
    obj.push_back(Pair("entries",      darkSendPool.GetEntriesCount()));
    obj.push_back(Pair("entries_accepted",      darkSendPool.GetCountEntriesAccepted()));
    return obj;
}


Value masternode(const Array& params, bool fHelp)
{
    string strCommand;
    if (params.size() >= 1)
        strCommand = params[0].get_str();

    if (fHelp  ||
        (strCommand != "init" && strCommand != "isInit" && strCommand != "start" && strCommand != "start-alias" && strCommand != "start-many" && strCommand != "stop" && strCommand != "stop-alias" && strCommand != "stop-many" && strCommand != "kill" && strCommand != "list" && strCommand != "list-conf" && strCommand != "count"  && strCommand != "enforce"
            && strCommand != "debug" && strCommand != "current" && strCommand != "winners" && strCommand != "genkey" && strCommand != "connect" && strCommand != "outputs"
             && strCommand != "addremote" && strCommand != "removeremote" && strCommand != "status" && strCommand != "status-all"))
        throw runtime_error(
            "masternode <init|isInit|start|start-alias|start-many|stop|stop-alias|stop-many|kill|list|list-conf|count|debug|current|winners|genkey|enforce|outputs|addremote|removeremote|status|status-all> [passphrase]\n");

    if (strCommand == "stop")
    {
        if(!fMasterNode) return "you must set masternode=1 in the configuration";

        if(pwalletMain->IsLocked()) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 2){
                strWalletPass = params[1].get_str().c_str();
            } else {
                throw runtime_error(
                    "Your wallet is locked, passphrase is required\n");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                return "incorrect passphrase";
            }
        }

        std::string errorMessage;
        if(!activeMasternode.StopMasterNode(errorMessage)) {
        	return "stop failed: " + errorMessage;
        }
        pwalletMain->Lock();

        if(activeMasternode.status == MASTERNODE_STOPPED) return "successfully stopped masternode";
        if(activeMasternode.status == MASTERNODE_NOT_CAPABLE) return "not capable masternode";

        return "unknown";
    }

    if (strCommand == "stop-alias")
    {
	    if (params.size() < 2){
			throw runtime_error(
			"command needs at least 2 parameters\n");
	    }

	    std::string alias = params[1].get_str().c_str();

    	if(pwalletMain->IsLocked()) {
    		SecureString strWalletPass;
    	    strWalletPass.reserve(100);

			if (params.size() == 3){
				strWalletPass = params[2].get_str().c_str();
			} else {
				throw runtime_error(
				"Your wallet is locked, passphrase is required\n");
			}

			if(!pwalletMain->Unlock(strWalletPass)){
				return "incorrect passphrase";
			}
        }

    	bool found = false;

		Object statusObj;
		statusObj.push_back(Pair("alias", alias));

    	BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
    		if(mne.getAlias() == alias) {
    			found = true;
    			std::string errorMessage;
    			bool result = activeMasternode.StopMasterNode(mne.getIp(), mne.getPrivKey(), errorMessage);

				statusObj.push_back(Pair("result", result ? "successful" : "failed"));
    			if(!result) {
   					statusObj.push_back(Pair("errorMessage", errorMessage));
   				}
    			break;
    		}
    	}

    	if(!found) {
    		statusObj.push_back(Pair("result", "failed"));
    		statusObj.push_back(Pair("errorMessage", "could not find alias in config. Verify with list-conf."));
    	}

    	pwalletMain->Lock();
    	return statusObj;
    }

    if (strCommand == "stop-many")
    {
    	if(pwalletMain->IsLocked()) {
			SecureString strWalletPass;
			strWalletPass.reserve(100);

			if (params.size() == 2){
				strWalletPass = params[1].get_str().c_str();
			} else {
				throw runtime_error(
				"Your wallet is locked, passphrase is required\n");
			}

			if(!pwalletMain->Unlock(strWalletPass)){
				return "incorrect passphrase";
			}
		}

		int total = 0;
		int successful = 0;
		int fail = 0;


		Object resultsObj;

		BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
			total++;

			std::string errorMessage;
			bool result = activeMasternode.StopMasterNode(mne.getIp(), mne.getPrivKey(), errorMessage);

			Object statusObj;
			statusObj.push_back(Pair("alias", mne.getAlias()));
			statusObj.push_back(Pair("result", result ? "successful" : "failed"));

			if(result) {
				successful++;
			} else {
				fail++;
				statusObj.push_back(Pair("errorMessage", errorMessage));
			}

			resultsObj.push_back(Pair("status", statusObj));
		}
		pwalletMain->Lock();

		Object returnObj;
		returnObj.push_back(Pair("overall", "Successfully stopped " + boost::lexical_cast<std::string>(successful) + " masternodes, failed to stop " +
				boost::lexical_cast<std::string>(fail) + ", total " + boost::lexical_cast<std::string>(total)));
		returnObj.push_back(Pair("detail", resultsObj));

		return returnObj;

    }

    if (strCommand == "list")
    {
        Array newParams(params.size() - 1);
        std::copy(params.begin() + 1, params.end(), newParams.begin());
        return masternodelist(newParams, fHelp);
    }

         if (strCommand == "count")
    {
        if (params.size() > 2){
            throw runtime_error(
                "too many parameters\n");
        }
		 
 if (params.size() == 2) return mnodeman.CountEnabled();
        return mnodeman.size();	 
    }

    if (strCommand == "start")
    {
        if(!fMasterNode) return "you must set masternode=1 in the configuration";

        if(pwalletMain->IsLocked(true)) {
            SecureString strWalletPass;
            strWalletPass.reserve(100);

            if (params.size() == 2){
                strWalletPass = params[1].get_str().c_str();
            } else {
                throw runtime_error(
                    "Your wallet is locked, passphrase is required\n");
            }

            if(!pwalletMain->Unlock(strWalletPass)){
                return "incorrect passphrase";
            }
        }

        if(activeMasternode.status != MASTERNODE_REMOTELY_ENABLED && activeMasternode.status != MASTERNODE_IS_CAPABLE){
            activeMasternode.status = MASTERNODE_NOT_PROCESSED; // TODO: consider better way
            std::string errorMessage;
            activeMasternode.ManageStatus();
            pwalletMain->Lock();
        }

        if(activeMasternode.status == MASTERNODE_REMOTELY_ENABLED) return "masternode started remotely";
        if(activeMasternode.status == MASTERNODE_INPUT_TOO_NEW) return "masternode input must have at least 15 confirmations";
        if(activeMasternode.status == MASTERNODE_STOPPED) return "masternode is stopped";
        if(activeMasternode.status == MASTERNODE_IS_CAPABLE) return "successfully started masternode";
        if(activeMasternode.status == MASTERNODE_NOT_CAPABLE) return "not capable masternode(cmd=start): " + activeMasternode.notCapableReason;
        if(activeMasternode.status == MASTERNODE_SYNC_IN_PROCESS) return "sync in process. Must wait until client is synced to start.";

        return "unknown";
    }

    if (strCommand == "start-alias")
    {
	    if (params.size() < 2){
			throw runtime_error(
			"command needs at least 2 parameters\n");
	    }

	    std::string alias = params[1].get_str().c_str();

    	if(pwalletMain->IsLocked()) {
    		SecureString strWalletPass;
    	    strWalletPass.reserve(100);

			if (params.size() == 3){
				strWalletPass = params[2].get_str().c_str();
			} else {
				throw runtime_error(
				"Your wallet is locked, passphrase is required\n");
			}

			if(!pwalletMain->Unlock(strWalletPass)){
				return "incorrect passphrase";
			}
        }

    	bool found = false;

		Object statusObj;
		statusObj.push_back(Pair("alias", alias));

    	BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
    		if(mne.getAlias() == alias) {
    			found = true;
    			std::string errorMessage;
    			bool result = activeMasternode.Register(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage);

    			statusObj.push_back(Pair("result", result ? "successful" : "failed"));
    			if(!result) {
					statusObj.push_back(Pair("errorMessage", errorMessage));
				}
    			break;
    		}
    	}

    	if(!found) {
    		statusObj.push_back(Pair("result", "failed"));
    		statusObj.push_back(Pair("errorMessage", "could not find alias in config. Verify with list-conf."));
    	}

    	pwalletMain->Lock();
    	return statusObj;

    }

    if (strCommand == "start-many")
    {
    	if(pwalletMain->IsLocked()) {
			SecureString strWalletPass;
			strWalletPass.reserve(100);

			if (params.size() == 2){
				strWalletPass = params[1].get_str().c_str();
			} else {
				throw runtime_error(
				"Your wallet is locked, passphrase is required\n");
			}

			if(!pwalletMain->Unlock(strWalletPass)){
				return "incorrect passphrase";
			}
		}

		std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
		mnEntries = masternodeConfig.getEntries();

		int total = 0;
		int successful = 0;
		int fail = 0;

		Object resultsObj;

		BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
			total++;

			std::string errorMessage;
			bool result = activeMasternode.Register(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), errorMessage);

			Object statusObj;
			statusObj.push_back(Pair("alias", mne.getAlias()));
			statusObj.push_back(Pair("result", result ? "successful" : "failed"));

			if(result) {
				successful++;
			} else {
				fail++;
				statusObj.push_back(Pair("errorMessage", errorMessage));
			}

			resultsObj.push_back(Pair("status", statusObj));
		}
		pwalletMain->Lock();

		Object returnObj;
		returnObj.push_back(Pair("overall", "Successfully started " + boost::lexical_cast<std::string>(successful) + " masternodes, failed to start " +
				boost::lexical_cast<std::string>(fail) + ", total " + boost::lexical_cast<std::string>(total)));
		returnObj.push_back(Pair("detail", resultsObj));

		return returnObj;
    }

    if (strCommand == "debug")
    {
        if(activeMasternode.status == MASTERNODE_REMOTELY_ENABLED) return "masternode started remotely";
        if(activeMasternode.status == MASTERNODE_INPUT_TOO_NEW) return "masternode input must have at least 15 confirmations";
        if(activeMasternode.status == MASTERNODE_IS_CAPABLE) return "successfully started masternode";
        if(activeMasternode.status == MASTERNODE_STOPPED) return "masternode is stopped";
        if(activeMasternode.status == MASTERNODE_NOT_CAPABLE) return "not capable masternode(cmd=debug): " + activeMasternode.notCapableReason;
        if(activeMasternode.status == MASTERNODE_SYNC_IN_PROCESS) return "sync in process. Must wait until client is synced to start.";

        CTxIn vin = CTxIn();
        CPubKey pubkey = CScript();
        CKey key;
        bool found = activeMasternode.GetMasterNodeVin(vin, pubkey, key);
        if(!found){
            return "Missing masternode input, please look at the documentation for instructions on masternode creation";
        } else {
            return "No problems were found";
        }
    }

    if (strCommand == "addremote")
    {
         if (params.size() < 6){
			throw runtime_error(
			"missing args <account> <ip:port> <key> <hash> <index>\n");
	    }

	    std::string alias = params[1].get_str().c_str();
        std::string ip = params[2].get_str().c_str();
        std::string privKey = params[3].get_str().c_str();
        std::string txHash = params[4].get_str().c_str();
        std::string outputIndex = params[5].get_str().c_str();

        masternodeConfig.create(alias, ip, privKey, txHash, outputIndex);

        return "Masternode created";
    }

    if (strCommand == "removeremote")
    {
         if (params.size() < 2) { 
			throw runtime_error(
			"missing args <account>\n");
	    }

	    std::string alias = params[1].get_str().c_str();

        bool res = masternodeConfig.remove(alias);
        if (!res)
            return "Masternode not found";

        return "Masternode removed";
    }

    if (strCommand == "current")
    {
        CMasternode* winner = mnodeman.GetCurrentMasterNode(1);
        if(winner) {
            return winner->addr.ToString().c_str();
        }

        return "unknown";
    }

    if (strCommand == "genkey")
    {
        CKey secret;
        secret.MakeNewKey(false);

        return CBitcoinSecret(secret).ToString();
    }

    if (strCommand == "winners")
    {
        Object obj;

        for(int nHeight = pindexBest->nHeight-10; nHeight < pindexBest->nHeight+20; nHeight++)
        {
            CScript payee;
            if(masternodePayments.GetBlockPayee(nHeight, payee)){
                CTxDestination address1;
                ExtractDestination(payee, address1);
                CBitcoinAddress address2(address1);
                obj.push_back(Pair(boost::lexical_cast<std::string>(nHeight),       address2.ToString().c_str()));
            } else {
                obj.push_back(Pair(boost::lexical_cast<std::string>(nHeight),       ""));
            }
        }

        return obj;
    }

    if(strCommand == "enforce")
    {
        return (uint64_t)enforceMasternodePaymentsTime;
    }

    if(strCommand == "connect")
    {
        std::string strAddress = "";
        if (params.size() == 2){
            strAddress = params[1].get_str().c_str();
        } else {
            throw runtime_error(
                "Masternode address required\n");
        }

        CService addr = CService(strAddress);

        if(ConnectNode((CAddress)addr, NULL, true)){
            return "successfully connected";
        } else {
            return "error connecting";
        }
    }

    if(strCommand == "list-conf")
    {
    	std::vector<CMasternodeConfig::CMasternodeEntry> mnEntries;
    	mnEntries = masternodeConfig.getEntries();

        std::vector<Value> resultArr;

        BOOST_FOREACH(CMasternodeConfig::CMasternodeEntry mne, masternodeConfig.getEntries()) {
    		Object mnObj;
    		mnObj.push_back(Pair("alias", mne.getAlias()));
    		mnObj.push_back(Pair("address", mne.getIp()));
    		mnObj.push_back(Pair("privateKey", mne.getPrivKey()));
    		mnObj.push_back(Pair("txHash", mne.getTxHash()));
    		mnObj.push_back(Pair("outputIndex", mne.getOutputIndex()));
    		resultArr.push_back(mnObj);
    	}

    	return resultArr;
    }

    if (strCommand == "outputs")
    {
        // Find possible candidates
        vector<COutput> possibleCoins = activeMasternode.SelectCoinsMasternode();

        Object obj;
        BOOST_FOREACH(COutput& out, possibleCoins) {
            obj.push_back(Pair(out.tx->GetHash().ToString().c_str(), boost::lexical_cast<std::string>(out.i)));
        }

        return obj;

    }

    if (strCommand == "status" || strCommand == "status-all")
    {
        // get masternode status
        std::vector<Value> resultArr;      
        std::vector<pair<unsigned int, CTxIn> > vecMasternodeScores = GetMasternodeScores(pindexBest->nHeight, MIN_INSTANTX_PROTO_VERSION);

        BOOST_FOREACH(CMasterNode mn, vecMasternodes) {
            if (strCommand == "status-all" || mn.addr.ToString() == strMasterNodeAddr) { 
                Object mnObj;

                // get masternode address
                CScript pubkey;
                pubkey = GetScriptForDestination(mn.pubkey.GetID());
                CTxDestination address1;
                ExtractDestination(pubkey, address1);
                CBitcoinAddress address2(address1);

                mnObj.push_back(Pair("minProtoVersion", mn.minProtoVersion));
                mnObj.push_back(Pair("address", mn.addr.ToString().c_str()));
                mnObj.push_back(Pair("pubkey", address2.ToString().c_str()));
                mnObj.push_back(Pair("vin", mn.vin.ToString()));
                mnObj.push_back(Pair("lastTimeSeen", mn.lastTimeSeen));            
                mnObj.push_back(Pair("activeseconds",  mn.lastTimeSeen - mn.now));   
                mnObj.push_back(Pair("rank", GetMasternodeRank(mn.vin, vecMasternodeScores)));
                mnObj.push_back(Pair("lastDseep", mn.lastDseep));
                mnObj.push_back(Pair("enabled", mn.enabled));
                mnObj.push_back(Pair("allowFreeTx", mn.allowFreeTx));
                mnObj.push_back(Pair("protocolVersion", mn.protocolVersion));
                mnObj.push_back(Pair("nLastDsq", mn.nLastDsq));

                // check if me to include activeMasternode.status
                if (mn.addr.ToString() == strMasterNodeAddr)
                    mnObj.push_back(Pair("status", activeMasternode.status));                

                resultArr.push_back(mnObj);
            }
    	}
        
    	return resultArr;
    }

    if (strCommand == "init")
    {
        if (params.size() == 3){
            strMasterNodePrivKey = params[1].get_str().c_str();
            strMasterNodeAddr = params[2].get_str().c_str();            
        } else {
            throw runtime_error(
                "missing args <MasterNodePrivKey> <MasterNodeAddr>\n");
        }

        
        CService addrTest = CService(strMasterNodeAddr);
        if (!addrTest.IsValid()) {
            throw runtime_error(
                "Invalid -masternodeaddr address: " + strMasterNodeAddr + "\n");
        }

        std::string errorMessage;

        CKey key;
        CPubKey pubkey;

        if(!darkSendSigner.SetKey(strMasterNodePrivKey, errorMessage, key, pubkey))
        {
            throw runtime_error(
                "Invalid masternodeprivkey\n");
        }

        activeMasternode.pubKeyMasternode = pubkey;

        fMasterNode = true;
        LogPrintf("IS DARKSEND MASTER NODE\n");

        return true;
    }

    if (strCommand == "kill") return fMasterNode = false;

    if (strCommand == "isInit") {
        // check flag and variables are set
        if (!fMasterNode || strMasterNodeAddr == "" || strMasterNodePrivKey == "")
            return false;
        
        // check valid address
        CService addrTest = CService(strMasterNodeAddr);
        if (!addrTest.IsValid())
            return false;

        std::string errorMessage;

        CKey key;
        CPubKey pubkey;

        if(!darkSendSigner.SetKey(strMasterNodePrivKey, errorMessage, key, pubkey))
            return false;

        return true;
    }

    return Value::null;
}

Value masternodelist(const Array& params, bool fHelp)
{
    std::string strMode = "active";
    std::string strFilter = "";
     if (params.size() >= 1) strMode = params[0].get_str();
    if (params.size() == 2) strFilter = params[1].get_str();
     if (fHelp ||
            (strMode != "active" && strMode != "vin" && strMode != "pubkey" && strMode != "lastseen"
             && strMode != "activeseconds" && strMode != "rank" && strMode != "protocol" && strMode != "full"))
    {
        throw runtime_error(
                "masternodelist ( \"mode\" \"filter\" )\n"
                "Get a list of masternodes in different modes\n"
                "\nArguments:\n"
                "1. \"mode\"      (string, optional, defauls = active) The mode to run list in\n"
                "2. \"filter\"    (string, optional) Filter results, can be applied in few modes only\n"
                "Available modes:\n"
                "  active         - Print '1' if active and '0' otherwise (can be filtered, exact match)\n"
                "  activeseconds  - Print number of seconds masternode recognized by the network as enabled\n"
                "  full           - Print info in format 'active | protocol | pubkey | vin | lastseen | activeseconds' (can be filtered, partial match)\n"
                "  lastseen       - Print timestamp of when a masternode was last seen on the network\n"
                "  protocol       - Print protocol of a masternode (can be filtered, exact match)\n"
                "  pubkey         - Print public key associated with a masternode (can be filtered, partial match)\n"
                "  rank           - Print rank of a masternode based on current block\n"
                "  vin            - Print vin associated with a masternode (can be filtered, partial match)\n"
                );
    }
    Object obj;
    std::vector<CMasternode> vMasternodes = mnodeman.GetFullMasternodeVector();
    BOOST_FOREACH(CMasternode& mn, vMasternodes) {
           
         std::string strAddr = mn.addr.ToString().c_str();
        if(strMode == "active"){
            if(strFilter !="" && stoi(strFilter) != mn.IsEnabled()) continue;
            obj.push_back(Pair(strAddr,       (int)mn.IsEnabled()));
        } else if (strMode == "vin") {
            if(strFilter !="" && mn.vin.prevout.hash.ToString().find(strFilter) == string::npos) continue;
            obj.push_back(Pair(strAddr,       mn.vin.prevout.hash.ToString().c_str()));
        } else if (strMode == "pubkey") {
            CScript pubkey;
            pubkey.SetDestination(mn.pubkey.GetID());
            CTxDestination address1;
            ExtractDestination(pubkey, address1);
            CBitcoinAddress address2(address1);
		
             if(strFilter !="" && address2.ToString().find(strFilter) == string::npos) continue;
            obj.push_back(Pair(strAddr,       address2.ToString().c_str()));
        } else if (strMode == "protocol") {
            if(strFilter !="" && stoi(strFilter) != mn.protocolVersion) continue;
            obj.push_back(Pair(strAddr,       (int64_t)mn.protocolVersion));
        } else if (strMode == "lastseen") {
            obj.push_back(Pair(strAddr,       (int64_t)mn.lastTimeSeen));
        } else if (strMode == "activeseconds") {
            obj.push_back(Pair(strAddr,       (int64_t)(mn.lastTimeSeen - mn.now)));
        } else if (strMode == "rank") {
            obj.push_back(Pair(strAddr,       (int)(mnodeman.GetMasternodeRank(mn.vin, chainActive.Tip()->nHeight))));
        } else if (strMode == "full") {
            CScript pubkey;
            pubkey.SetDestination(mn.pubkey.GetID());
            CTxDestination address1;
            ExtractDestination(pubkey, address1);
            CBitcoinAddress address2(address1);
		
            std::ostringstream stringStream;
            stringStream << (mn.IsEnabled() ? "1" : "0") << " | " <<
                           mn.protocolVersion << " | " <<
                           address2.ToString() << " | " <<
                           mn.vin.prevout.hash.ToString() << " | " <<
                           mn.lastTimeSeen << " | " <<
                           (mn.lastTimeSeen - mn.now);
            std::string output = stringStream.str();
            stringStream << " " << strAddr;
            if(strFilter !="" && stringStream.str().find(strFilter) == string::npos) continue;
            obj.push_back(Pair(strAddr, output));
        }
    }
    return obj;

}
