#include <qtum/qtumDGP.h>
#include <chainparams.h>

std::vector<uint32_t> createDataSchedule(const dev::eth::EVMSchedule& schedule)
{
    std::vector<uint32_t> tempData = {schedule.tierStepGas[0], schedule.tierStepGas[1], schedule.tierStepGas[2],
                                      schedule.tierStepGas[3], schedule.tierStepGas[4], schedule.tierStepGas[5],
                                      schedule.tierStepGas[6], schedule.tierStepGas[7], schedule.expGas,
                                      schedule.expByteGas, schedule.sha3Gas, schedule.sha3WordGas,
                                      schedule.sloadGas, schedule.sstoreSetGas, schedule.sstoreResetGas,
                                      schedule.sstoreRefundGas, schedule.jumpdestGas, schedule.logGas,
                                      schedule.logDataGas, schedule.logTopicGas, schedule.createGas,
                                      schedule.callGas, schedule.callStipend, schedule.callValueTransferGas,
                                      schedule.callNewAccountGas, schedule.suicideRefundGas, schedule.memoryGas,
                                      schedule.quadCoeffDiv, schedule.createDataGas, schedule.txGas,
                                      schedule.txCreateGas, schedule.txDataZeroGas, schedule.txDataNonZeroGas,
                                      schedule.copyGas, schedule.extcodesizeGas, schedule.extcodecopyGas,
                                      schedule.balanceGas, schedule.suicideGas, schedule.maxCodeSize};
    return tempData;
}

std::vector<uint32_t> scheduleDataForBlockNumber(unsigned int blockHeight)
{
    dev::eth::EVMSchedule schedule = globalSealEngine->chainParams().scheduleForBlockNumber(blockHeight);
    return createDataSchedule(schedule);
}

void QtumDGP::initDataSchedule(){
    dataSchedule = scheduleDataForBlockNumber(0);
}

void QtumDGP::initContractHook(int blockHeight) {
    DGPaddresses.DGPContract = DGPContract_v1;
    DGPaddresses.GovernanceDGP = GovernanceDGP_v1;
    DGPaddresses.BudgetDGP = BudgetDGP_v1;

    if (blockHeight <= 0) {
        return;
    }

    const Consensus::Params& consensusParams = Params().GetConsensus();

    // Don't trigger until at least MIP2
    if (blockHeight < consensusParams.MIP2Height) {
        return;
    }
    // Don't trigger until at least MIP3 Start
    if (blockHeight < consensusParams.MIP3StartHeight) {
        return;
    }

    const CBlockIndex* pindex = ::ChainActive().Tip()->GetAncestor(blockHeight - 1);
    if (pindex == nullptr) {
        LogPrintf("[WARNING] Checking DGP: pindex null at %u ...\n", blockHeight);
    }
    Consensus::DeploymentPos pos = Consensus::DeploymentPos::DEPLOYMENT_MIP3_DGP_UPGRADE;
    // Get state of MIP3
    ThresholdState state = VersionBitsState(pindex, consensusParams, pos, versionbitscache);
    int64_t since_height = VersionBitsTipStateSinceHeight(consensusParams, pos);

    //LogPrintf("Checking DGP: %u ...\n", blockHeight);
    // If MIP3 state is active, use new contracts
    if (state == ThresholdState::ACTIVE && blockHeight >= since_height) {
        DGPaddresses.DGPContract = DGPContract_v2;
        DGPaddresses.GovernanceDGP = GovernanceDGP_v2;
        DGPaddresses.BudgetDGP = BudgetDGP_v2;
        //LogPrintf("Using new DGP %s at %u\n", DGPaddresses.GovernanceDGP, blockHeight);
    }
}

bool QtumDGP::checkLimitSchedule(const std::vector<uint32_t>& defaultData, const std::vector<uint32_t>& checkData, int blockHeight){
    const Consensus::Params& consensusParams = Params().GetConsensus();

    if(defaultData.size() == 39 && (checkData.size() == 39 || (checkData.size() == 40 && blockHeight >= consensusParams.QIP7Height))) {
        for(size_t i = 0; i < defaultData.size(); i++){
            uint32_t max = defaultData[i] * 1000 > 0 ? defaultData[i] * 1000 : 1 * 1000;
            uint32_t min = defaultData[i] / 100 > 0 ? defaultData[i] / 100 : 1;
            if(checkData[i] > max || checkData[i] < min){
                return false;
            }
        }
        return true;
    }
    return false;
}

dev::eth::EVMSchedule QtumDGP::getGasSchedule(int blockHeight){
    clear();
    dataSchedule = scheduleDataForBlockNumber(blockHeight);
    dev::eth::EVMSchedule schedule = globalSealEngine->chainParams().scheduleForBlockNumber(blockHeight);
    if(initStorages(getDGPContract(), blockHeight, ParseHex("26fadbe2"))){
        schedule = createEVMSchedule(schedule, blockHeight);
    }
    return schedule;
}

uint64_t QtumDGP::getUint64FromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data){
    uint64_t value = 0;
    if(initStorages(contract, blockHeight, data)){
        if(!dgpevm){
            parseStorageOneUint64(value);
        } else {
            parseDataOneUint64(value);
        }
    }
    return value;
}

uint32_t QtumDGP::getBlockSize(unsigned int blockHeight){
    clear();
    uint32_t result = DEFAULT_BLOCK_SIZE_DGP;
    uint32_t blockSize = getUint64FromDGP(blockHeight, getDGPContract(), ParseHex("92ac3c62"));
    if(blockSize <= MAX_BLOCK_SIZE_DGP && blockSize >= MIN_BLOCK_SIZE_DGP){
        result = blockSize;
    }
    return result;
}

uint64_t QtumDGP::getMinGasPrice(unsigned int blockHeight){
    clear();
    uint64_t result = DEFAULT_MIN_GAS_PRICE_DGP;
    uint64_t minGasPrice = getUint64FromDGP(blockHeight, getDGPContract(), ParseHex("3fb58819"));
    if(minGasPrice <= MAX_MIN_GAS_PRICE_DGP && minGasPrice >= MIN_MIN_GAS_PRICE_DGP){
        result = minGasPrice;
    }
    return result;
}

uint64_t QtumDGP::getBlockGasLimit(unsigned int blockHeight){
    clear();
    uint64_t result = DEFAULT_BLOCK_GAS_LIMIT_DGP;
    uint64_t blockGasLimit = getUint64FromDGP(blockHeight, getDGPContract(), ParseHex("2cc8377d"));
    if(blockGasLimit <= MAX_BLOCK_GAS_LIMIT_DGP && blockGasLimit >= MIN_BLOCK_GAS_LIMIT_DGP){
        result = blockGasLimit;
    }
    return result;
}

DGPFeeRates QtumDGP::getFeeRates(unsigned int blockHeight){
    clear();
    DGPFeeRates dgpFeeRates;
    dgpFeeRates.minRelayTxFee = DEFAULT_MIN_RELAY_TX_FEE_DGP;
    dgpFeeRates.incrementalRelayFee = DEFAULT_INCREMENTAL_RELAY_FEE_DGP;
    dgpFeeRates.dustRelayFee = DEFAULT_DUST_RELAY_TX_FEE_DGP;
    /*
    std::vector<uint64_t> feeRates = getUint64VectorFromDGP(blockHeight, getDGPContract(), ParseHex("d6f6ac1d"));
    if (feeRates.size() == 3){
        if(feeRates[0] <= MAX_MIN_RELAY_TX_FEE_DGP && feeRates[0] >= MIN_MIN_RELAY_TX_FEE_DGP){
            dgpFeeRates.minRelayTxFee = feeRates[0];
        }
        if(feeRates[1] <= MAX_INCREMENTAL_RELAY_FEE_DGP && feeRates[1] >= MIN_INCREMENTAL_RELAY_FEE_DGP){
            dgpFeeRates.incrementalRelayFee = feeRates[1];
        }
        if(feeRates[2] <= MAX_DUST_RELAY_TX_FEE_DGP && feeRates[2] >= MIN_DUST_RELAY_TX_FEE_DGP){
            dgpFeeRates.dustRelayFee = feeRates[2];
        }
    } */
    return dgpFeeRates;
}

uint64_t QtumDGP::getGovernanceCollateral(unsigned int blockHeight){
    clear();
    uint64_t result = DEFAULT_GOVERNANCE_COLLATERAL;
    uint64_t collateral = getUint64FromDGP(blockHeight, getDGPContract(), ParseHex("3efafcd9"));
    if(collateral > 0){
        result = collateral;
    }
    return result;
}

uint64_t QtumDGP::getBudgetFee(unsigned int blockHeight){
    clear();
    uint64_t result = DEFAULT_BUDGET_FEE;
    uint64_t budgetFee = getUint64FromDGP(blockHeight, getDGPContract(), ParseHex("0db3970d"));
    if(budgetFee > 0){
        result = budgetFee;
    }
    return result;
}

dev::Address QtumDGP::getGovernanceWinner(unsigned int blockHeight){
    clear();
    uint64_t defaultGasLimit = DEFAULT_BLOCK_GAS_LIMIT_DGP;
    bool startGovMaturity = false;
    bool winnerEligibileFix = false;

    if (gArgs.GetChainName() == CBaseChainParams::MAIN) {
        if (::ChainActive().Tip()->nHeight > 110000 && ::ChainActive().Tip()->nHeight < 137001) {
            defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_WINNER_OP_SEND;
        }
        if (::ChainActive().Tip()->nHeight < 110001) {
            defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_OP_SEND;
        }

        // 48hr maturity fix enforcement
        if (::ChainActive().Tip()->nHeight > 170000 && ::ChainActive().Tip()->nHeight < 199999) {
           startGovMaturity = true;
        }

        // Fix for choosing winner directly from list of eligible rather then getting contract to do it
        if (::ChainActive().Tip()->nHeight > 200000) {
           winnerEligibileFix = true;
        }
    }

    if (gArgs.GetChainName() == CBaseChainParams::TESTNET) {
        if (::ChainActive().Tip()->nHeight > 187000 && ::ChainActive().Tip()->nHeight < 200001) {
            defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_WINNER_OP_SEND;
        }
        if (::ChainActive().Tip()->nHeight < 187001) {
            defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_OP_SEND;
        }

        // 48hr maturity fix enforcement
        if (::ChainActive().Tip()->nHeight > 245000 && ::ChainActive().Tip()->nHeight < 254999) {
           startGovMaturity = true;
        }

        // Fix for choosing winner directly from list of eligible rather then getting contract to do it
        if (::ChainActive().Tip()->nHeight > 255000) {
           winnerEligibileFix = true;
        }
    }

    if (gArgs.GetChainName() == CBaseChainParams::REGTEST) {
        winnerEligibileFix = true;
    }

    dev::Address value = getAddressFromDGP(blockHeight, getGovernanceDGP(), ParseHex("aabe2fe3"), defaultGasLimit);

    if (startGovMaturity) {
        if (value != dev::Address(0x0)) {
            std::vector<uint64_t> v = getUint64VectorFromDGP(blockHeight, getGovernanceDGP(), ParseHex("e3eece26000000000000000000000000" + HexStr(value.asBytes())));
            if (::ChainActive().Tip()->nHeight < v[0] + 1920) {
                //Take the registration block and add 48hrs worth of blocks
                LogPrintf("Governor immature - Address: %s | Registration Block: %u\n", HexStr(value.asBytes()), v[0] + 1920);
                value = dev::Address(0x0);
            }
        }
    }

    if (winnerEligibileFix) {
        const Consensus::Params& consensusParams = Params().GetConsensus();
        int64_t dgpCollateral = getGovernanceCollateral(blockHeight);
        int64_t height = ::ChainActive().Tip()->nHeight;
        // Must be 48 hrs old to get first rewards
        uint64_t minRewardMaturity = 1920;
        // How often rewarded
        uint64_t rewardBlockInterval = 1920;
        // must have received ping in the last 30 days(28800 blocks)
        uint64_t pingInterval = 28800;
        // Valid governors are at least 15 blocks old.
        uint64_t minMaturity = 15;

        // Metrix fix to skip bad Governor in contract
        if (isBannedGov(HexStr(value.asBytes())) && height >= consensusParams.MIP2Height && height < consensusParams.MIP3Height) {
            return dev::Address(0x0);
        }

        if (value != dev::Address(0x0)) {
                // check if contract winner selection is valid and meets the criteria
                std::vector<uint64_t> v = getUint64VectorFromDGP(blockHeight, getGovernanceDGP(), ParseHex("e3eece26000000000000000000000000" + HexStr(value.asBytes())));
                uint64_t govBlockHeight = v[0];
                uint64_t govlastPing = v[1];
                uint64_t govCollateral = v[2];
                uint64_t govLastReward = v[3];
                if (height >= govBlockHeight + minRewardMaturity && 
                    height >= govLastReward + rewardBlockInterval && 
                    dgpCollateral == govCollateral && 
                    height >= govBlockHeight + minMaturity && 
                    height <= govlastPing + pingInterval) {
                        LogPrintf("Governor valid - Address: %s | Registration block: %i | Last Rewarded: %i\n", HexStr(value.asBytes()), v[0], v[3]);
                        return value;
                } else {
                    // if winner selection doesn't pass criteria checks then manually try and find an eligible one.
                    // Get the list of currently active governors
                    std::vector<dev::Address> governorAddresses = getAddressVectorFromDGP(blockHeight, getGovernanceDGP(), ParseHex("883703c2"));
                    for(std::vector<uint64_t>::size_type i = 0; i != governorAddresses.size(); i++) {
                        dev::Address value = dev::Address(governorAddresses[i]);
                        // Metrix fix to skip bad Governor in contract
                        if (isBannedGov(HexStr(value.asBytes())) && height >= consensusParams.MIP2Height && height < consensusParams.MIP3Height) {
                            continue;
                        }
                        std::vector<uint64_t> v = getUint64VectorFromDGP(blockHeight, getGovernanceDGP(), ParseHex("e3eece26000000000000000000000000" + HexStr(value.asBytes())));
                        uint64_t govBlockHeight = v[0];
                        uint64_t govlastPing = v[1];
                        uint64_t govCollateral = v[2];
                        uint64_t govLastReward = v[3];
                        if (height >= govBlockHeight + minRewardMaturity && 
                            height >= govLastReward + rewardBlockInterval && 
                            dgpCollateral == govCollateral && 
                            height >= govBlockHeight + minMaturity && 
                            height <= govlastPing + pingInterval) {
                                LogPrintf("Governor valid - Address: %s | Registration block: %i | Last Rewarded: %i\n", HexStr(value.asBytes()), v[0], v[3]);
                                return value;
                            }
                    }
                }
        }
        // If we have looped through and found no one eligible then pay no one
        value = dev::Address(0x0);
    }

    return value;
}

bool QtumDGP::isBannedGov(std::string addressStr) {
    // Metrix hacky fix to skip bad Governor in contract
    // TODO: remove after MIP3 !
    const std::string badGov = "a66268b3c8a9501e492f81abdd81655ee41e35d2";
    if (addressStr == badGov) {
        return true;
    }
    return false;
}

dev::Address QtumDGP::getDGPContract() {
    return DGPaddresses.DGPContract;
}

dev::Address QtumDGP::getGovernanceDGP() {
    return DGPaddresses.GovernanceDGP;
}

dev::Address QtumDGP::getBudgetDGP() {
    return DGPaddresses.BudgetDGP;
}

bool QtumDGP::initStorages(const dev::Address& addr, unsigned int blockHeight, std::vector<unsigned char> data, uint64_t defaultGasLimit){
    initStorageDGP(addr);
    // metrix DGP contract address does not change so no need to check for it every time
    if(blockHeight > 0){
        if(!dgpevm){
            initStorageTemplate(addr);
        } else {
            initDataTemplate(addr, data, defaultGasLimit);
        }
        return true;
    }
    return false;
}

void QtumDGP::initStorageDGP(const dev::Address& addr){
    storageDGP = state->storage(addr);
}

void QtumDGP::initStorageTemplate(const dev::Address& addr){
    storageTemplate = state->storage(addr);
}

void QtumDGP::initDataTemplate(const dev::Address& addr, std::vector<unsigned char>& data, uint64_t defaultGasLimit){
    // metrix send default gas limit to prevent recursive call when getting gas limit
    dataTemplate = CallContract(addr, data, dev::Address(), 0, defaultGasLimit)[0].execRes.output;
}

void QtumDGP::createParamsInstance(){
    dev::h256 paramsInstanceHash = sha3(dev::h256("0000000000000000000000000000000000000000000000000000000000000000"));
    if(storageDGP.count(paramsInstanceHash)){
        dev::u256 paramsInstanceSize = storageDGP.find(paramsInstanceHash)->second.second;
        for(size_t i = 0; i < size_t(paramsInstanceSize); i++){
            std::pair<unsigned int, dev::Address> params;
            params.first = dev::toUint64(storageDGP.find(sha3(paramsInstanceHash))->second.second);
            ++paramsInstanceHash;
            params.second = dev::right160(dev::h256(storageDGP.find(sha3(paramsInstanceHash))->second.second));
            ++paramsInstanceHash;
            paramsInstance.push_back(params);
        }
    }
}

dev::Address QtumDGP::getAddressForBlock(unsigned int blockHeight){
    for(auto i = paramsInstance.rbegin(); i != paramsInstance.rend(); i++){
        if(i->first <= blockHeight)
            return i->second;
    }
    return dev::Address();
}

static inline bool sortPairs(const std::pair<dev::u256, dev::u256>& a, const std::pair<dev::u256, dev::u256>& b){
    return a.first < b.first;
}

void QtumDGP::parseStorageScheduleContract(std::vector<uint32_t>& uint32Values){
    std::vector<std::pair<dev::u256, dev::u256>> data;
    for(size_t i = 0; i < 5; i++){
        dev::h256 gasScheduleHash = sha3(dev::h256(dev::u256(i)));
        if(storageTemplate.count(gasScheduleHash)){
            dev::u256 key = storageTemplate.find(gasScheduleHash)->second.first;
            dev::u256 value = storageTemplate.find(gasScheduleHash)->second.second;
            data.push_back(std::make_pair(key, value));
        }
    }

    std::sort(data.begin(), data.end(), sortPairs);

    for(std::pair<dev::u256, dev::u256> d : data){
        dev::u256 value = d.second;
        for(size_t i = 0; i < 4; i++){
            uint64_t uint64Value = dev::toUint64(value);
            value = value >> 64;

            uint32Values.push_back(uint32_t(uint64Value));
            uint64Value = uint64Value >> 32;
            uint32Values.push_back(uint32_t(uint64Value));
        }
    }
}

void QtumDGP::parseDataScheduleContract(std::vector<uint32_t>& uint32Values){
    size_t size = dataTemplate.size() / 32;
    for(size_t i = 0; i < size; i++){
        std::vector<unsigned char> value = std::vector<unsigned char>(dataTemplate.begin() + (i * 32), dataTemplate.begin() + ((i+1) * 32));
        dev::h256 valueTemp(value);
        uint32Values.push_back(dev::toUint64(dev::u256(valueTemp)));
    }
}

void QtumDGP::parseStorageOneUint64(uint64_t& value){
    dev::h256 blockSizeHash = sha3(dev::h256(dev::u256(0)));
    if(storageTemplate.count(blockSizeHash)){
        value = dev::toUint64(storageTemplate.find(blockSizeHash)->second.second);
    }
}

void QtumDGP::parseDataOneUint64(uint64_t& value){
    if(dataTemplate.size() == 32){
        value = dev::toUint64(dev::u256(dev::h256(dataTemplate)));
    }
}

void QtumDGP::parseStorageOneAddress(dev::Address& value){
    dev::h256 blockSizeHash = sha3(dev::h256(dev::u256(0)));
    if(storageTemplate.count(blockSizeHash)){
        value = dev::Address(dev::h160((dev::u160)storageTemplate.find(blockSizeHash)->second.second));
    }
}

void QtumDGP::parseDataOneAddress(dev::Address& value){
    if(dataTemplate.size() == 32){
        std::vector<unsigned char> addr = std::vector<unsigned char>(dataTemplate.begin() + 12, dataTemplate.begin() + 32);
        value = dev::Address(dev::toHex(addr));
    }
}

void QtumDGP::parseDataVectorAddress(std::vector<dev::Address>& addressValues){
    size_t size = dataTemplate.size() / 32;
    for(size_t i = 0; i < size; i++){
        std::vector<unsigned char> addr = std::vector<unsigned char>(dataTemplate.begin() + (i * 32) + 12, dataTemplate.begin() + ((i+1) * 32));
        addressValues.push_back(dev::Address(dev::toHex(addr)));
    }
}

std::vector<dev::Address> QtumDGP::getAddressVectorFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data){
    std::vector<dev::Address> addressValues;
    if(initStorages(contract, blockHeight, data)){
        if(!dgpevm){
            //parseStorageOneAddress(value);
        } else {
            parseDataVectorAddress(addressValues);
        }
    }
    return addressValues;
}

dev::Address QtumDGP::getAddressFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data, uint64_t defaultGasLimit){
    dev::Address value = dev::Address(0);
    if(initStorages(contract, blockHeight, data, defaultGasLimit)){
        if(!dgpevm){
            parseStorageOneAddress(value);
        } else {
            parseDataOneAddress(value);
        }
    }
    return value;
}

void QtumDGP::parseStorageUint64Vector(std::vector<uint64_t>& uint64Values){
    std::vector<std::pair<dev::u256, dev::u256>> data;
    for(size_t i = 0; i < 3; i++){
        dev::h256 gasScheduleHash = sha3(dev::h256(dev::u256(i)));
        if(storageTemplate.count(gasScheduleHash)){
            dev::u256 key = storageTemplate.find(gasScheduleHash)->second.first;
            dev::u256 value = storageTemplate.find(gasScheduleHash)->second.second;
            data.push_back(std::make_pair(key, value));
        }
    }

    std::sort(data.begin(), data.end(), sortPairs);

    for(std::pair<dev::u256, dev::u256> d : data){
        dev::u256 value = d.second;
        uint64Values.push_back(dev::toUint64(value));
    }
}

void QtumDGP::parseDataUint64Vector(std::vector<uint64_t>& uint64Values){
    size_t size = dataTemplate.size() / 32;
    for(size_t i = 0; i < size; i++){
        std::vector<unsigned char> value = std::vector<unsigned char>(dataTemplate.begin() + (i * 32), dataTemplate.begin() + ((i+1) * 32));
        dev::h256 valueTemp(value);
        uint64Values.push_back(dev::toUint64(dev::u256(valueTemp)));
    }
}

std::vector<uint64_t> QtumDGP::getUint64VectorFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data){
    std::vector<uint64_t> uint64Values;
    if(initStorages(contract, blockHeight, data)){
        if(!dgpevm){
            parseStorageUint64Vector(uint64Values);
        } else {
            parseDataUint64Vector(uint64Values);
        }
    }
    return uint64Values;
}

dev::eth::EVMSchedule QtumDGP::createEVMSchedule(const dev::eth::EVMSchedule &_schedule, int blockHeight){
    dev::eth::EVMSchedule schedule = _schedule;
    std::vector<uint32_t> uint32Values;

    if(!dgpevm){
        parseStorageScheduleContract(uint32Values);
    } else {
        parseDataScheduleContract(uint32Values);
    }

    if(!checkLimitSchedule(dataSchedule, uint32Values, blockHeight))
        return schedule;

    if(uint32Values.size() >= 39){
        schedule.tierStepGas = {{uint32Values[0], uint32Values[1], uint32Values[2], uint32Values[3],
                                uint32Values[4], uint32Values[5], uint32Values[6], uint32Values[7]}};
        schedule.expGas = uint32Values[8];
        schedule.expByteGas = uint32Values[9];
        schedule.sha3Gas = uint32Values[10];
        schedule.sha3WordGas = uint32Values[11];
        schedule.sloadGas = uint32Values[12];
        schedule.sstoreSetGas = uint32Values[13];
        schedule.sstoreResetGas = uint32Values[14];
        schedule.sstoreRefundGas = uint32Values[15];
        schedule.jumpdestGas = uint32Values[16];
        schedule.logGas = uint32Values[17];
        schedule.logDataGas = uint32Values[18];
        schedule.logTopicGas = uint32Values[19];
        schedule.createGas = uint32Values[20];
        schedule.callGas = uint32Values[21];
        schedule.callStipend = uint32Values[22];
        schedule.callValueTransferGas = uint32Values[23];
        schedule.callNewAccountGas = uint32Values[24];
        schedule.suicideRefundGas = uint32Values[25];
        schedule.memoryGas = uint32Values[26];
        schedule.quadCoeffDiv = uint32Values[27];
        schedule.createDataGas = uint32Values[28];
        schedule.txGas = uint32Values[29];
        schedule.txCreateGas = uint32Values[30];
        schedule.txDataZeroGas = uint32Values[31];
        schedule.txDataNonZeroGas = uint32Values[32];
        schedule.copyGas = uint32Values[33];
        schedule.extcodesizeGas = uint32Values[34];
        schedule.extcodecopyGas = uint32Values[35];
        schedule.balanceGas = uint32Values[36];
        schedule.suicideGas = uint32Values[37];
        schedule.maxCodeSize = uint32Values[38];
    }
    return schedule;
}

void QtumDGP::clear(){
    templateContract = dev::Address();
    storageDGP.clear();
    storageTemplate.clear();
    paramsInstance.clear();
}
