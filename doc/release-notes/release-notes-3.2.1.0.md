# Linda 3.2.1.0

This is a patch release fix a bug causing transactions not to relay to nodes.

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/linda/issues

## How to Upgrade
If you are running an older version (older than v3.2), shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Linda-Qt or Lindad file.

The first time you run after the upgrade a re-indexing process will be started that will take anywhere from 30 minutes to several hours, depending on the speed of your machine.

## About this Release

### What's New
- Replace trickle nodes with per-node/message Poisson delays

### Bug Fixes
- Fix an issue where nodes where not asking for transaction data.