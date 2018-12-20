# Linda 3.0.0.0

#### This is a mandatory upgrade. Please upgrade by block 580000 to avoid any interruptions

## How to Upgrade
If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Linda-Qt or Lindad file.

## About this Release

### What's New
- We've moved forward the POW lockout to block 580000. This will turn Linda into a 100% POS chain
- Masternode rewards have been adjusted to a constant 3200 Linda per block to give ~60% return annually
- Staking rewards have been adjusted to give ~50% return annually
- We've added a max coin age for staking of 30 days to prevent unfair block rewards and manipulation of the POS difficulty algorithm
- We've added a max coin value for staking of 100 000 000 Linda to encourage more staking and prevent manipulation of the POS difficulty algorithm
- Versions of Linda older than 3.1.0.0 will be disconnected at block 580000. This will help increase network speed and reduce orphan blocks

### Bug Fixes
- We've fixed an issue that was causing coins to be sent when the wallet was unlocked for staking but the cancel button was clicked when prompted for a passphrase.