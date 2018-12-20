# Linda 3.1.0.1

## How to Upgrade
If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Linda-Qt or Lindad file.

## About this Release

### What's New
- Coin splitting will only split your coins if they are above 100K
- Updated masternode winner selection to make rewards fairer. You should expect 1 reward every 24 hours.
- Added checks so masternodes don't earn twice on their coins by staking. If you earned a reward as a masternode and want to stake with the coins instead you'll need to send them to yourself or wait ~9 days
- Remove old masternodes on unsupported wallets.
- Prevent running more than 1 masternode per IP. This is to stop unfair payments to people attempting to trick the network without contributing