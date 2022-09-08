# Metrix 4.0.10.0

## Bugfix update

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

# Core

- Softfork - In response to a prior issue caused by a bad Governor address that cannot be unenrolled from the contract. 
  - This will update the DGP Contracts used by the network after a block version consensus passes.
- Use newly deployed, updated DGP Contracts across the network after fork.
- Increment version 4.0.10.0
- New protocol version 70023
