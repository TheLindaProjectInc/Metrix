Wallet Tools
---------------------

### [BitRPC](/contrib/bitrpc) ###
Allows for sending of all standard Bitcoin commands via RPC rather than as command line args.

### [SpendFrom](/contrib/spendfrom) ###

Use the raw transactions API to send coins received on a particular
address (or addresses).

Repository Tools
---------------------

### [Developer tools](/contrib/devtools) ###
Specific tools for developers working on this repository.
Contains the script `github-merge.sh` for merging github pull requests securely and signing them using GPG.

### [Linearize](/contrib/linearize) ###
Construct a linear, no-fork, best version of the blockchain.

### [Qos](/contrib/qos) ###

A Linux bash script that will set up tc to limit the outgoing bandwidth for connections to the Bitcoin network. This means one can have an always-on bitcoind instance running, and another local bitcoind/bitcoin-qt instance which connects to this node and receives blocks from it.