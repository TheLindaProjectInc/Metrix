# Metrix 3.4.5.0

This is a patch version release.

Please report bugs using the issue tracker at github: https://github.com/thelindaproject/metrix/issues

## How to Upgrade
Shut down Metrix, wait until it has completely shut down (which might take a few minutes
for older versions), then just copy over the appropriate metrixd file.

If you are upgrading from 3.1.0.1 or older, the first time you run after the upgrade
a re-indexing process will be started that will take anywhere from 30 minutes to
several hours, depending on the speed of your machine.

## Downgrading warnings
Release 3.4.0.0 makes use of headers-first synchronization and parallel
block download (see further), the block files and databases are not
backwards-compatible with older versions of Metrix Core:

* Blocks will be stored on disk out of order (in the order they are
received, really), which makes it incompatible with some tools or
other programs. Reindexing using earlier versions will also not work
anymore as a result of this.

* The block index database will now hold headers for which no block is
stored on disk, which earlier versions won't support.
This does not affect wallet forward or backward compatibility.

If you want to be able to downgrade smoothly, make a backup of your entire data
directory. Without this your node will need to start syncing (or importing from
bootstrap.dat) anew afterwards. It is possible that the data from a completely
synchronised 3.4 node may be usable in older versions as-is, but this is not
supported and may break as soon as the older version attempts to reindex.

## metrix-cli
We are moving away from the metrixd executable functioning both as a server and
as a RPC client. The RPC client functionality ("tell the running metrix daemon
to do THIS") was split into a separate executable, 'metrix-cli'.

### Bug Fixes
- Fix segment fault when syncing from scratch