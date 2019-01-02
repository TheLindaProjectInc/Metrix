# Linda 3.3.0.0

This is a new major version release, bringing both new features and bug fixes.

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/linda/issues

## How to Upgrade
Shut down Linda, wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Lindad file.

If you are upgrading from 3.1.0.1 or older, the first time you run after the upgrade a re-indexing process will be started that will take anywhere from 30 minutes to several hours, depending on the speed of your machine.

## Downgrading warnings
The 'chainstate' for this release is not always compatible with previous releases, so if you run 3.3 and then decide to switch back to a 3.2 release you might get a blockchain validation error when starting the old release (due to 'pruned outputs' being omitted from the index of unspent transaction outputs).

Running the old release with the -reindex option will rebuild the chainstate data structures and correct the problem.

Also, the first time you run a 3.2 release on a 3.3 wallet it will rescan the blockchain for missing spent coins, which will take a long time (tens of minutes on a typical machine).

## About this Release

### What's New

#### New RPC commands

#### New startup commands
- whitebind: Bind to given address and whitelist peers connecting to it
- whitelist: Whitelist peers connecting from the given netmask or ip. Can be specified multiple times.
- stopafterblockimport: Stop running after importing blocks from disk

### Bug Fixes