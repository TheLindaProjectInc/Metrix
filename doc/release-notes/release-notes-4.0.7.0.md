# Metrix 4.0.6.1

## Bugfix update

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

# Core
- Fix for crash on validation when winner selection returns no governor.
- Update checkpoint blocks

# Wallet

The `-salvagewallet` startup option has been removed. A new `salvage` command has been added to the `metrix-wallet` tool which performs the salvage operations that `-salvagewallet` did.