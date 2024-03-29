#ifndef QTUMDGP_H
#define QTUMDGP_H

#include <qtum/qtumstate.h>
#include <primitives/block.h>
#include <validation.h>
#include <util/strencodings.h>

// DGP Version 3 [MIP5] Contract Addresses - MAINNET
static const dev::Address DGPContract_v3 = dev::Address("0xfc199dd5d86f60e2c15da35b9c1c1c70e5890c03");
static const dev::Address GovernanceDGP_v3 = dev::Address("0x73e6c0383dceed1583eb6a4b2aa9253020cb2b18");
static const dev::Address BudgetDGP_v3 = dev::Address("0x495e4ce99ab534cf8d3faa53c817b632681717fb");
// DGP Version 3 [MIP5] Contract Addresses - TESTNET
static const dev::Address tDGPContract_v3 = dev::Address("0xc88ceb07c05ac05666ae982c257c52468aad4f93");
static const dev::Address tGovernanceDGP_v3 = dev::Address("0x3cc15a2bae287cabb1ef6f26a86fa6f1895708eb");
static const dev::Address tBudgetDGP_v3 = dev::Address("0xf2d61e4cd5fc215071e5e60350261ec4f41ee2b5");
// DGP Version 3 [MIP5] Contract Addresses - REGTEST
static const dev::Address rDGPContract_v3 = dev::Address("0x0000000000000000000000000000000000000088");
static const dev::Address rGovernanceDGP_v3 = dev::Address("0x0000000000000000000000000000000000000089");
static const dev::Address rBudgetDGP_v3 = dev::Address("0x0000000000000000000000000000000000000090");

// DGP Version 2 [MIP3] Contract Addresses - MAINNET
static const dev::Address DGPContract_v2 = dev::Address("0xa523bfd08ca0365ca0f93de522c8d53590447a52");
static const dev::Address GovernanceDGP_v2 = dev::Address("0x13a5933a1b786e8016178656145e36eccd0221f6");
static const dev::Address BudgetDGP_v2 = dev::Address("0x28238c7d116aa2ca3739c4c93038fd5a06a77303");
// DGP Version 2 [MIP3] Contract Addresses - TESTNET
static const dev::Address tDGPContract_v2 = dev::Address("0x710efbb1026237f0cbf32ca9c5f040f86b66ae0c");
static const dev::Address tGovernanceDGP_v2 = dev::Address("0x4e3d35f867c895c612d38dc28a30e815c6ccd717");
static const dev::Address tBudgetDGP_v2 = dev::Address("0xb196971bc9ed1a193fa69310986d3de5d14c59c4");
// DGP Version 2 [MIP3] Contract Addresses - REGTEST
static const dev::Address rDGPContract_v2 = dev::Address("0x0000000000000000000000000000000000000088");
static const dev::Address rGovernanceDGP_v2 = dev::Address("0x0000000000000000000000000000000000000089");
static const dev::Address rBudgetDGP_v2 = dev::Address("0x0000000000000000000000000000000000000090");

// DGP Version 1 [original] Contract Addresses
static const dev::Address DGPContract_v1 = dev::Address("0x0000000000000000000000000000000000000088");
static const dev::Address GovernanceDGP_v1 = dev::Address("0x0000000000000000000000000000000000000089");
static const dev::Address BudgetDGP_v1 = dev::Address("0x0000000000000000000000000000000000000090");

static const uint32_t MIN_BLOCK_SIZE_DGP = 500000;
static const uint32_t MAX_BLOCK_SIZE_DGP = 32000000;
static const uint32_t DEFAULT_BLOCK_SIZE_DGP = 2000000;

static const uint64_t MIN_MIN_GAS_PRICE_DGP = 1;
static const uint64_t MAX_MIN_GAS_PRICE_DGP = 10000000;
static const uint64_t DEFAULT_MIN_GAS_PRICE_DGP = 5000;

static const uint64_t MIN_BLOCK_GAS_LIMIT_DGP = 1000000;
static const uint64_t MAX_BLOCK_GAS_LIMIT_DGP = 1000000000;
static const uint64_t DEFAULT_BLOCK_GAS_LIMIT_DGP = 40000000;
static const uint64_t DEFAULT_GAS_LIMIT_DGP_OP_SEND = 500000;
static const uint64_t DEFAULT_GAS_LIMIT_DGP_WINNER_OP_SEND = 500000;

static const uint64_t MIN_MIN_RELAY_TX_FEE_DGP = 100000;
static const uint64_t MAX_MIN_RELAY_TX_FEE_DGP = 100000000000;
static const uint64_t DEFAULT_MIN_RELAY_TX_FEE_DGP = 1000000000;
static const uint64_t MIN_INCREMENTAL_RELAY_FEE_DGP = 100000;
static const uint64_t MAX_INCREMENTAL_RELAY_FEE_DGP = 100000000000;
static const uint64_t DEFAULT_INCREMENTAL_RELAY_FEE_DGP = 1000000000;
static const uint64_t MIN_DUST_RELAY_TX_FEE_DGP = 100000;
static const uint64_t MAX_DUST_RELAY_TX_FEE_DGP = 300000000000;
static const uint64_t DEFAULT_DUST_RELAY_TX_FEE_DGP = 3000000000;

static const uint64_t DEFAULT_GOVERNANCE_COLLATERAL = 750000000000000;

static const uint64_t DEFAULT_BUDGET_FEE = 60000000000000;

struct metrixDGPaddr
{
    dev::Address DGPContract;
    dev::Address GovernanceDGP;
    dev::Address BudgetDGP;
};

struct DGPFeeRates
{
    uint64_t minRelayTxFee;
    uint64_t incrementalRelayFee;
    uint64_t dustRelayFee;
};

class QtumDGP {
    
public:

    QtumDGP(QtumState* _state, int blockHeight, bool _dgpevm = true) : dgpevm(_dgpevm), state(_state) {
        initContractHook(blockHeight);
        initDataSchedule(); 
    }

    dev::eth::EVMSchedule getGasSchedule(int blockHeight);

    uint32_t getBlockSize(unsigned int blockHeight);

    uint64_t getMinGasPrice(unsigned int blockHeight);

    uint64_t getBlockGasLimit(unsigned int blockHeight);

    DGPFeeRates getFeeRates(unsigned int blockHeight);

    uint64_t getGovernanceCollateral(unsigned int blockHeight);

    uint64_t getBudgetFee(unsigned int blockHeight);

    dev::Address getGovernanceWinner(unsigned int blockHeight);

    dev::Address getDGPContract();

    dev::Address getGovernanceDGP();

    dev::Address getBudgetDGP();

    uint32_t getContractVersion();

private:

    bool initStorages(const dev::Address& addr, unsigned int blockHeight, std::vector<unsigned char> data = std::vector<unsigned char>(), uint64_t defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_OP_SEND);

    void initStorageDGP(const dev::Address& addr);

    void initStorageTemplate(const dev::Address& addr);

    void initDataTemplate(const dev::Address& addr, std::vector<unsigned char>& data, uint64_t defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_OP_SEND);

    void initContractHook(int blockHeight);

    void initDataSchedule();

    bool checkLimitSchedule(const std::vector<uint32_t>& defaultData, const std::vector<uint32_t>& checkData, int blockHeight);

    void createParamsInstance();

    dev::Address getAddressForBlock(unsigned int blockHeight);

    uint64_t getUint64FromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data);

    void parseStorageScheduleContract(std::vector<uint32_t>& uint32Values);
    
    void parseDataScheduleContract(std::vector<uint32_t>& uint32Values);

    void parseStorageOneUint64(uint64_t& blockSize);

    void parseDataOneUint64(uint64_t& value);
    
    void parseStorageOneAddress(dev::Address& value);
    
    void parseDataOneAddress(dev::Address& value);

    void parseDataVectorAddress(std::vector<dev::Address>& addressValues);

    void parseStorageUint64Vector(std::vector<uint64_t>& uint64Values);

    void parseDataUint64Vector(std::vector<uint64_t>& uint64Values);

    std::vector<uint64_t> getUint64VectorFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data);

    std::vector<dev::Address> getAddressVectorFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data);

    dev::Address getAddressFromDGP(unsigned int blockHeight, const dev::Address& contract, std::vector<unsigned char> data, uint64_t defaultGasLimit = DEFAULT_GAS_LIMIT_DGP_OP_SEND);

    dev::eth::EVMSchedule createEVMSchedule(const dev::eth::EVMSchedule& schedule, int blockHeight);

    bool isBannedGov(std::string addr);

    void clear();


    bool dgpevm;

    uint32_t contractVersion;

    const QtumState* state;

    dev::Address templateContract;

    std::map<dev::h256, std::pair<dev::u256, dev::u256>> storageDGP;

    std::map<dev::h256, std::pair<dev::u256, dev::u256>> storageTemplate;

    std::vector<unsigned char> dataTemplate;

    std::vector<std::pair<unsigned int, dev::Address>> paramsInstance;

    std::vector<uint32_t> dataSchedule;

    metrixDGPaddr DGPaddresses;
};
#endif
