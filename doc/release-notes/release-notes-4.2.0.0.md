# Metrix Core v4.2.0.0

## Major Update

This is a major protocol upgrade in response to an issue discovered within the (v2) DGP contracts.
This upgrade will enable usage of newly created DGP contracts after consensus is reached.

Failure to upgrade after activation of MIP6 will result in disconnection from the greater Metrix network.

## Bug Reporting

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

## What's Changed
- Changed target timespan for BIP9 signaling from 2016 to 6720 (MIP4)
- Added start block height for BIP9 signaling
- Softfork using BIP9 consensus vote
- Bugfix to ensure governance winner validation occurs in contract after the DPGv3 migration.
- Disable legacy validation method once MIP6 activates, this will reduce CPU used by the daemon after the activation block.
- Increment version 4.2.0.0
- New protocol version 70026
- Implementation of [MIP4](https://github.com/TheLindaProjectInc/MIPs/blob/main/mip-4.md)
- Implementation of [MIP5](https://github.com/TheLindaProjectInc/MIPs/blob/main/mip-5.md)
- Implementation of [MIP6](https://github.com/TheLindaProjectInc/MIPs/blob/main/mip-6.md)