# Metrix 4.0.9.0

## Bugfix update

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

# Core

- Hardfork - In response to an issue caused by a bad Governor address that cannot be unenrolled from the contract. Fork occurs at 685000, there is also sync fixes to ensure sync from genesis after the bad gov got introduced at block 681822.
- Increment version 4.0.9.0
- New protocol version 70022
