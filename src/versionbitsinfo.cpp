// Copyright (c) 2016-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <versionbitsinfo.h>

#include <consensus/params.h>

const struct VBDeploymentInfo VersionBitsDeploymentInfo[Consensus::MAX_VERSION_BITS_DEPLOYMENTS] = {
    {
        /*.name =*/ "testdummy",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "mip3",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "mip4",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "mip5",
        /*.gbt_force =*/ true,
    },
    {
        /*.name =*/ "mip6",
        /*.gbt_force =*/ true,
    },
};
