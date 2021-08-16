# Metrix 4.0.7.0

## Mandatory update

Once the MIP1 protocol version update is active, protocol versions prior to 70018 will be disconnected from the network.

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

# Core
- Fix chain sync from genesis
- Increment version 4.0.7.0
- New protocol version 70018
- Ban old peers after soft fork
- Fix for mismatched block data at 264695 that caused sync from genesis to fail.
- Add checkpoints to guide nodes.
- Minor code tidying.
- Update block version to 0x30000000