# Metrix 4.0.5.0

## Bugfix Update

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

### Bug fixes
- Stop logging OP_SPEND and OP_CALL as unknown transaction types in the debug log. Causing excessive log bloat, particulary with large wallets with lots of transactions.
- Update the QT to utilise explorer.metrixcoin.com for TX urls.