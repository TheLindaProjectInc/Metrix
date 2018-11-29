Copyright (c) 2009-2012 Bitcoin Developers  
Copyright (c) 2018 The Linda Project Inc Developers  
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.



UNIX BUILD NOTES
================

Dependencies
---------------------

These dependencies are required:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 libssl      | Crypto           | Random Number Generation
 libgmp      | Secp256k1        | Secp256k1 Dependency
 libboost    | Utility          | Library for threading, data structures, etc
 libevent    | Networking       | OS independent asynchronous networking
 libdb4.8    | Berkeley DB      | Wallet storage (only needed when wallet enabled)
 libsecp256k1| Secp256k1        | Elliptic Curve Cryptography

Optional dependencies:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 miniupnpc   | UPnP Support     | Firewall-jumping support

For the versions used in the release, see [release-process.md](release-process.md) under *Fetch and build inputs*.

System requirements
--------------------

C++ compilers are memory-hungry. It is recommended to have at least 1 GB of
memory available when compiling LindaCoin Core. With 512MB of memory or less
compilation will take much longer due to swap thrashing.

Dependency Build Instructions: Ubuntu & Debian
----------------------------------------------
Build requirements:

    sudo apt-get install build-essential libtool automake autotools-dev autoconf pkg-config libssl-dev libgmp3-dev libevent-dev bsdmainutils

On at least Ubuntu 14.04+ and Debian 7+ there are generic names for the
individual boost development packages, so the following can be used to only
install necessary parts of boost:

    sudo apt-get install libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-program-options-dev libboost-test-dev libboost-thread-dev

If that doesn't work, you can install all boost development packages with:

    sudo apt-get install libboost-all-dev

BerkeleyDB is required for the wallet. db4.8 packages are available [here](https://launchpad.net/~bitcoin/+archive/bitcoin).
You can add the repository and install using the following commands:

    sudo add-apt-repository ppa:bitcoin/bitcoin
    sudo apt-get update
    sudo apt-get install libdb4.8-dev libdb4.8++-dev
    
If that does not work alternatively download and compile: 
    
    cd ~
    mkdir bitcoin/db4/
    
    wget 'http://download.oracle.com/berkeley-db/db-4.8.30.NC.tar.gz'
    tar -xzvf db-4.8.30.NC.tar.gz
    cd db-4.8.30.NC/build_unix/
    ../dist/configure --enable-cxx
    make
    sudo make install

 
Ubuntu and Debian have their own libdb-dev and libdb++-dev packages, but these will install
BerkeleyDB 5.1 or later, which break binary wallet compatibility with the distributed executables which
are based on BerkeleyDB 4.8. If you do not care about wallet compatibility,
pass `--with-incompatible-bdb` to configure.

To build Secp256k1:

    cd src/secp256k1/ && ./configure && make
    sudo make install
    sudo ldconfig

Optional:

    sudo apt-get install libminiupnpc-dev (see --with-miniupnpc and --enable-upnp-default)

Notes
-----
1) You only need Berkeley DB if the wallet is enabled (see the section *Disable-Wallet mode* below).

2) The release is built with GCC and then "strip transferd" to strip the debug
symbols, which reduces the executable size by about 90%.


Build Lindad
----

This builds Lindad and Linda-cli using the dynamic dependancies of the current system.

```
./autogen.sh
./configure
make
strip src/Lindad
strip src/Lindad-cli
```

# STATIC BUILD INSTRUCTIONS
The following instructions have been tested on the following distributions:

1. Ubuntu 16.04.4
2. Linux Mint 18.3

The advantage of building the wallet this way is that it will be able to be executed even on a fresh ubuntu installation without adding additional libraries. There will be no dependencies error messages at startup in case some shared libs are missing. The current release was build that way.

## Get the building environment ready (same as above)

Open a terminal window. If git is not installed in your system, install it by issuing the following command
```
sudo apt-get install git
```
Install Linux development tools 
```
sudo apt-get install build-essential libtool automake autotools-dev autoconf pkg-config libgmp3-dev libevent-dev bsdmainutils
```
  
Get the Linda source code
----

Clone the Lindacoin repository

	git clone https://github.com/TheLindaProjectInc/linda.git Linda


## The depends build systems

The depends build system will automatically download and compile the static packages needed to complete a build of the Linda executables.

In its simplest form it will download and compile the packages for the system host type its being run from. E.g. Linux x64  
The same depends system can be used to cross-compile binaries for other operating systems. See the [depends/README.md](../depends/README.md) doc for further instruction on this.

To build dependencies for the current arch+OS:

    cd Linda\depends
    make

### Compiling Lindad


```
cd ..
./autogen.sh
./configure
make
strip src/Lindad
strip src/Lindad-cli

```

If you receive an error on x64 Linux machines about missing boost libraries use the following configure line. Boost libraries may be somewhere else on x64 machines.

```
./configure --with-boost-libdir=/usr/lib/x86_64-linux-gnu

```
