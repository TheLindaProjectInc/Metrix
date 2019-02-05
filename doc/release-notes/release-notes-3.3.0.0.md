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

## Linda-cli
We are moving away from the Lindad executable functioning both as a server and as a RPC client. The RPC client functionality ("tell the running Lindacoin daemon to do THIS") was split into a separate executable, 'Linda-cli'. The RPC client code will eventually be removed from Lindad, but will be kept for backwards compatibility for a release or two.

## RPC
`walletpassphrase` 
The behavior of the `walletpassphrase` RPC when the wallet is already unlocked has changed between 3.2 and 3.3.

The 3.2 behavior of `walletpassphrase` is to fail when the wallet is already unlocked:
```
> walletpassphrase 1000
walletunlocktime = now + 1000
> walletpassphrase 10
Error: Wallet is already unlocked (old unlock time stays)
```
The new behavior of `walletpassphrase` is to set a new unlock time overriding the old one:
```
> walletpassphrase 1000
walletunlocktime = now + 1000
> walletpassphrase 10
walletunlocktime = now + 10 (overriding the old unlock time)
```
## Transaction malleability-related fixes
This release contains a few fixes for transaction ID (TXID) malleability issues:

- `-nospendzeroconfchange` command-line option, to avoid spending zero-confirmation change
- `IsStandard()` transaction rules tightened to prevent relaying and mining of mutated transactions
- Additional information in listtransactions/gettransaction output to report wallet transactions that conflict with each other because they spend the same outputs.
- Bug fixes to the getbalance/listaccounts RPC commands, which would report incorrect balances for double-spent (or mutated) transactions.

## Transaction Fees
This release drops the default fee required to relay transactions across the network and for miners to consider the transaction in their blocks to 0.001Linda per kilobyte.

Note that getting a transaction relayed across the network does NOT guarantee that the transaction will be accepted by a miner; by default, miners fill their blocks with 50 kilobytes of high-priority transactions, and then with 700 kilobytes of the highest-fee-per-kilobyte transactions.

The minimum relay/mining fee-per-kilobyte may be changed with the minrelaytxfee option. Note that previous releases incorrectly used the mintxfee setting to determine which low-priority transactions should be considered for inclusion in blocks.

The wallet code still uses a default fee for low-priority transactions of 0.01Linda per kilobyte. During periods of heavy transaction volume, even this fee may not be enough to get transactions confirmed quickly; the mintxfee option may be used to override the default.

## About this Release

### What's New
#### Block-chain handling and storage:
- Check for correct genesis (prevent cases where a datadir from the wrong network is accidentally loaded)
- Log aborted block database rebuilds
- Fix non-standard disconnected transactions causing mempool orphans

#### Wallet
- Store key creation time. Calculate whole-wallet birthday.
- Optimize rescan to skip blocks prior to birthday
- Let user select wallet file with -wallet=foo.dat
- Improve wallet load time
- Don’t count txins for priority to encourage sweeping
- Don’t create empty transactions when reading a corrupted wallet
- Fix rescan to start from beginning after importprivkey
- Only create signatures with low S values
- Make GetAvailableCredit run GetHash() only once per transaction (performance improvement)
- Fix `importwallet` nTimeFirstKey (trigger necessary rescans)
- CWallet init fix
- Check redeemScript size does not exceed 520 byte limit
- Ignore (and warn about) too-long redeemScripts while loading wallet

#### Protocol and network:
- Drop the fee required to relay a transaction to 0.001Linda per kilobyte
- Send tx relay flag with version
- New reject P2P message (BIP 0061, see https://gist.github.com/gavinandresen/7079034 for draft)
- Relay OP_RETURN data TxOut as standard transaction type
- Remove CENT-output free transaction rule when relaying
- Send multiple inv messages if mempool.size > MAX_INV_SZ
- Do not treat fFromMe transaction differently when broadcasting
Process received messages one at a time without sleeping between messages
- Improve logging of failed connections
- Bump protocol version to 70002
- Add some additional logging to give extra network insight
- Prevent socket leak in ThreadSocketHandler and correct some proxy related socket leaks
- Use pnode->nLastRecv as sync score (was the wrong way around)

#### Validation:
- Log reason for non-standard transaction rejection
- Prune provably-unspendable outputs, and adapt consistency check for it.
- Detect any sufficiently long fork and add a warning
- Call the -alertnotify script when we see a long or invalid fork
- Fix multi-block reorg transaction resurrection
- Reject non-canonically-encoded serialization sizes
- Reject dust amounts during validation
- Accept nLockTime transactions that finalize in the next block
- Add a way to limit deserialized string lengths and use it
- Increase IsStandard() scriptSig length
- Remove a useless millisleep in socket handler
- Stricter memory limits on CNode
- Better orphan transaction handling

#### RPC commands
- New notion of 'conflicted' transactions, reported as confirmations: -1
- `listreceivedbyaddress` now provides tx ids
- Add raw transaction hex to gettransaction output
- Updated help and tests for `getreceivedby(account|address)`
- In `getblock`, accept 2nd verbose parameter, similar to `getrawtransaction`, but defaulting to 1 for backward compatibility
- Add `verifychain`, to verify chain database at runtime
- Add `dumpwallet` and `importwallet` RPCs
- `keypoolrefill` gains optional size parameter
- Add `getbestblockhash`, to return tip of best chain
- Make RPC password resistant to timing attacks
- Clarify help messages and add examples
- Add `getrawchangeaddress` call for raw transaction change destinations
- Reject insanely high fees by default in `sendrawtransaction`
- Add RPC call `decodescript` to decode a hex-encoded transaction script
- Make `validateaddress` provide redeemScript
- New RPC ping command to request ping, new `pingtime` and `pingwait` fields in `getpeerinfo` output
- Adding new `addrlocal` field to `getpeerinfo` output
- Add verbose boolean to `getrawmempool`
- Add rpc command `getunconfirmedbalance` to obtain total unconfirmed balance
- Explicitly ensure that wallet is unlocked in `importprivkey`
- Add check for valid keys in `importprivkey`
- Add `getwalletinfo`, `getblockchaininfo` and `getnetworkinfo` calls (will replace hodge-podge getinfo at some point)
- Add a relayfee field to `getnetworkinfo`
- Fix RPC related shutdown hangs and leaks
- Always show syncnode in `getpeerinfo`
- `sendrawtransaction`: report the reject code and reason, and make it possible to re-send transactions that are already in the mempool
- `getmininginfo` show right genproclimit
- Avoid a segfault on getblock if it can’t read a block from disk
- Add paranoid return value checks in base58

#### Startup commands
- whitebind: Bind to given address and whitelist peers connecting to it
- whitelist: Whitelist peers connecting from the given netmask or ip. Can be specified multiple times.
- stopafterblockimport: Stop running after importing blocks from disk
- New option: -nospendzeroconfchange to never spend unconfirmed change outputs
- Rename option -tor to -onion to better reflect what it does
- Add -disablewallet mode to let Lindad run entirely without wallet (when built with wallet)
- Update default -rpcsslciphers to include TLSv1.2
- Make -logtimestamps default on and rework help-message
- RPC client option: -rpcwait, to wait for server start
- Remove -logtodebugger
- Allow -noserver with Lindad
- Fix -printblocktree output
- Show error message if ReadConfigFile fails

#### Miscellaneous:
- Replace non-threadsafe C functions (gmtime, strerror and setlocale)
- Add missing cs_main and wallet locks
- key.cpp: fail with a friendlier message on missing ssl EC support
- Remove bignum dependency for scripts

### Bug Fixes