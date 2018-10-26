# Linda 3.2.0.0

This is a major release designed to improve performance and handle the increasing volume of transactions on the network.

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/linda/issues

## How to Upgrade
If you are running an older version, shut it down. Wait until it has completely shut down (which might take a few minutes for older versions), then just copy over the appropriate Linda-Qt or Lindad file.

The first time you run after the upgrade a re-indexing process will be started that will take anywhere from 30 minutes to several hours, depending on the speed of your machine.

## About this Release

### What's New
- Implemented Pieter Wuille's many optimizations to the way transactions are verified, so a running, synchronized node uses less working memory and does much less I/O. He also implemented parallel signature checking, so if you have a multi-CPU machine all CPUs will be used to verify transactions. 
- “Bloom filter” support in the network protocol for sending only relevant transactions to lightweight clients.
- Remove peercoins sync checkpoints
- Payments (transaction outputs) of 0.546 times the minimum relay fee (0.00000546 LINDA) are now considered ‘non-standard’, because storing them costs the network more than they are worth and spending them will usually cost their owner more in transaction fees than they are worth.
- Network code performance and robustness improvements
- Blk file performance and robustness improvements (blk files are now stored in blocks folder inside the data dir)
- log timestamps by default
- added checkpoint at POW end block to ignore V2 forks

#### New settings (command-line or Linda.conf file)
- dbcache : controls LevelDB memory usage.
- par : controls how many threads to use to validate transactions. Defaults to the number of CPUs on your machine, use -par=1 to limit to a single CPU.
- reindex : rebuild block and transaction indices from the downloaded block data.

### Bug Fixes
- Fix nodes forwarding transactions with empty vins and getting banned
- Fix a sync error due to a rounding discrepancy