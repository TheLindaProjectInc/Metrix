# Metrix 4.1.0.0

## Major update

This is a major protocol upgrade in response to an issue encountered within the origin DGP contracts.
This upgrade will enable usage of newly created DGP contracts after consensus is reached.

Failure to upgrade after activation of MIP3 will result in disconnection from the greater Metrix network.

## Bug Reporting

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

# Core

- Softfork using BIP9 consensus vote
  - In response to a prior issue caused by a bad Governor address that cannot be unenrolled from the contract. 
  - Update the DGP Contracts used by the network after a block version consensus vote passes.
- Use newly deployed, updated DGP Contracts across the network after fork.
- Increment version 4.1.0.0
- New protocol version 70023
- Improve sync speed during blocks which require legacy validation lookup
- Corrected subsidy for > 12 year reduction
- Add DGP contracts to `getdgpinfo` command
- Add Optional usage of blockheight in `getdgpinfo` command
- Corrected references to Metrix RPC port
