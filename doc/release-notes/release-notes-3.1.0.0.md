# Linda 3.1.0.0

#### This is a mandatory upgrade. Please upgrade by block 555000 to avoid any interruptions

## How to Upgrade
If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Linda-Qt or Lindad file.

## About this Release

### Critical Fix
- fixed issue where wallet was stuck on syncing block 0 when syncing a new blockchain
- Beginning from block 555000, consecutive blocks created through PoW will be rejected by the network. This is to address a potential security concern if an attacker has a vast amount of hash power at their disposal.

### Sync Improvements
- track peer block downloads to auto remove stalled peers
- disconnect peer when reached ban threshold
- disallows access to nodes running wallets less than 2.0.0.0
- nodes running wallets less than 3.0.0.0 will be disconnected from the network starting after block 585200
- dump orphan blocks from peer when disconnected (prevents filling orphan block list with unusable blocks
- add maxorphantx startup flag to cap orphan transactions held in memory for running on lightweight machines

### General Improvements
- masternode should ignore if wallet is unlocked for staking only
- locked masternode coins will be ignored when staking
- masternode start will check all possible coin inputs instead of just the first

### General RPC improvements
- updates listreceivedbyaddress RPC command to return only your addresses when running with minconf=0 and includeempty=true instead of returning all addresses including send addresses from your address book
- fix GetAccountAddress not committing the new account to the walletdb causing duplicates to be made when running the command again
- setaccount no longer can create new addresses. This was a confusing feature and gave no way to change account names without creating new addresses
- setaccount returns error if trying to change the account of an address that doesn't exist in your wallet
- listreceived now returns (change) addresses
- added RPC commands for masternodes
- masternode status dumps your masternode status. includes your active time and status to determine if your masternode is running correctly
-masternode status-all dumps the above info for all masternodes on the network (only your masternode will include status field)
- masternode init ip:port allows you to initialise your masternode without editing the linda.conf or masternode.conf files and or restarting the program. Running masternode start will then start your masternode
- masternode addremote ip:port - adds the info to your masternode.conf file (no need to restart Linda-qt or Lindad) running start-many or start-alias will start the appropriate remote masternodes
- masternode removeremote - removes the related account from your masternode.conf file (no need to restart Linda-qt or Lindad) you will need to run the masternode stop to stop these masternodes
- masternode isInit - returns boolean if masternode is enabled and capable of running
- masternode kill - disables masternode. can be re-enable by running init command
- Masternode tab performance improvement.

### Thanks to @Tumba24
- fixed the masternodes UI tab uses a lot of resources when it refreshes.
- added sorting to the masternodes UI tab