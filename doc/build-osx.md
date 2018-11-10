Copyright (c) 2009-2012 Bitcoin Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.


OSX BUILD NOTES
================

Notes
-----

All of the commands should be executed in a Terminal application. The
built-in one is located in `/Applications/Utilities`.

Preparation
-----------

You need to install XCode with all the options checked so that the compiler
and everything is available in /usr not just /Developer. XCode should be
available on your OS X installation media, but if not, you can get the
current version from https://developer.apple.com/xcode/. If you install
Xcode 4.3 or later, you'll need to install its command line tools. This can
be done in `Xcode > Preferences > Downloads > Components` and generally must
be re-done or updated every time Xcode is updated.

There's an assumption that you already have `git` installed, as well. If
not, it's the path of least resistance to install [Github for Mac](https://mac.github.com/)
(OS X 10.7+) or
[Git for OS X](https://code.google.com/p/git-osx-installer/). It is also
available via Homebrew or MacPorts.

Dependencies
---------------------

These dependencies are required:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 libssl      | Crypto           | Random Number Generation
 libgmp      | Secp256k1        | Secp256k1 Dependency
 libboost    | Utility          | Library for threading, data structures, etc
 libevent    | Networking       | OS independent asynchronous networking
 libdb5.8    | Berkeley DB      | Wallet storage (only needed when wallet enabled)
 libsecp256k1| Secp256k1        | Elliptic Curve Cryptography

Optional dependencies:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 miniupnpc   | UPnP Support     | Firewall-jumping support


System requirements
--------------------

C++ compilers are memory-hungry. It is recommended to have at least 1 GB of
memory available when compiling LindaCoin Core. With 512MB of memory or less
compilation will take much longer due to swap thrashing.

# STATIC BUILD INSTRUCTIONS
The following instructions have been tested on the following distributions:

1. OSX High Sierra

The advantage of building the wallet this way is that it will be able to be executed even on a fresh osx installation without adding additional libraries. There will be no dependencies error messages at startup in case some shared libs are missing. The current release was build that way.

## Compile all dependencies manually and use their static libs
### Download and build BerkeleyDB 5.0.32.NC
```
cd ~/
wget 'http://download.oracle.com/berkeley-db/db-5.0.32.NC.tar.gz'
tar -xzvf db-5.0.32.NC.tar.gz
cd db-5.0.32.NC/build_unix/
../dist/configure --enable-cxx --disable-shared 
make

```

### Compiling Boost 1.58

Download Boost 1.58 here : 
https://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz/download<br>
Put the archive in ~/deps

```
cd ~/deps
tar xvfz boost_1_58_0.tar.gz
cd ~/deps/boost_1_58_0
./bootstrap.sh

./b2 --build-type=complete --layout=versioned --with-chrono --with-filesystem --with-program_options --with-system --with-thread variant=release link=static threading=multi runtime-link=static stage

```

### Compiling miniupnpc

Install Miniupnpc. Download it from here http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.9.tar.gz<br>
and place it in your deps folder, then :
```
cd ~/deps
tar xvfz miniupnpc-1.9.tar.gz

cd miniupnpc-1.9
make upnpc-static
```
==> Important : don't forget to rename "miniupnpc-1.9" directory to "miniupnpc"

### Compiling OpenSSL

download 1.0.2g version here : https://www.openssl.org/source/old/1.0.2/openssl-1.0.2g.tar.gz<br>
place archive in deps folders then :
```
tar xvfz openssl-1.0.2g.tar.gz
cd openssl-1.0.2g 
./Configure darwin64-x86_64-cc
make depend
make
```

### Building `Lindad`

1. Clone the github tree to get the source code and go into the directory.
```
git clone https://github.com/TheLindaProjectInc/linda.git
cd linda
```
2.  Build Lindad:
```
cd src
make -f makefile.osx STATIC=1
```