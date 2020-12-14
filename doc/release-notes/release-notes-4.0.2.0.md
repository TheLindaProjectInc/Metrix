# Metrix 4.0.2.0

## Mandatory Update

This is a mandatory update.
Bugfixes to Qtum v0.19.1

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

### Bug fixes
- Fix help message for `getsubsidy`
- Update linearize script.
- Add prevoutStake data to getblock and getblockheader
- Fix a bug where decoderawtransaction did not return the correct call type in some cases.
- Fix a bug where gettransactionreceipt returned wrong cumulativeGasUsed in some cases.
- Fix a bug where sendmanywithdupes did not set all values correctly in some cases.
- Increase default blockGasLimit for winner contract call - Fix for Governor reward payouts applies at block 110001