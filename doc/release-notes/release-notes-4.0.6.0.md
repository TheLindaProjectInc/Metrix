# Metrix 4.0.6.0

## Mandatory Update

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

### Bug fixes
- Fix to Governor winner selection returning immature winner and then stalling further payments until maturity is met.
- Stop logging OP_CALL & OP_SEND as unknown transactions in wallet
- Update checkpoint blocks