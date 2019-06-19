Copyright (c) 2018 Linda Project INC Developers  
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (https://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.

WINDOWS CROSSCOMPILE BUILD NOTES
===================

These dependencies are required:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 libssl      | Crypto           | Random Number Generation
 libgmp      | Secp256k1        | Secp256k1 Dependency
 libboost    | Utility          | Library for threading, data structures, etc
 libevent    | Networking       | OS independent asynchronous networking
 libdb5.0    | Berkeley DB      | Wallet storage (only needed when wallet enabled)
 libsecp256k1| Secp256k1        | Elliptic Curve Cryptography

Optional dependencies:

 Library     | Purpose          | Description
 ------------|------------------|----------------------
 miniupnpc   | UPnP Support     | Firewall-jumping support


System requirements
--------------------

C++ compilers are memory-hungry. It is recommended to have at least 1 GB of
memory available when compiling Metrix Core. With 512MB of memory or less
compilation will take much longer due to swap thrashing.

These instructions have been tested on Ubuntu 16.04 x64. This guide was created using reference from the [depends/README.md](../depends/README.md)

Dependency Build Instructions: Ubuntu & Debian
----------------------------------------------
Build requirements:

    sudo apt-get install build-essential libtool automake autotools-dev autoconf pkg-config libssl-dev libgmp3-dev libevent-dev bsdmainutils gcc-multilib g++-multilib

To cross compile for 32bit and 64bit systems both these below will be required. If you only want to compile for one architecture, just install the one you want.

Windows x86:

    sudo apt install g++-mingw-w64-i686

Windows 64:

    sudo apt install g++-mingw-w64-x86-64

Mac OS:

	sudo apt install curl librsvg2-bin libtiff-tools bsdmainutils cmake imagemagick libcap-dev libz-dev libbz2-dev python-setuptools
	
Dependencies
------------

Dependencies will be compiled using the depends build system. This will download and compile the required packages for the OS you wish to build for.

Your local depends folder can contain the downloaded dependancies for each OS type, each will be stored in their respective depends/\<arch> folder.

Their licenses:

    Berkeley DB    New BSD license with additional requirement that linked software must be free open source
    Boost          MIT-like license
    miniupnpc      New (3-clause) BSD license
    OpenSSL        Old BSD license with the problematic advertising requirement
    Zlib           Compatible with the GNU General Public License, and approved by FSF & OSI.

Versions used in this release:

    Berkeley DB   5.0.32.NC
    Boost         1.58.0
    miniupnpc     1.9
    OpenSSL       1.0.2g
    Zlib          1.2.11

Note.. In each instance we download the dependancies with the NO_QT=1 option added. We dont use QT so there is no need to download the large packages associated with it.

This assumes your Metrix source in located in your home drive.  
The system will download and build all the dependancies.  
At the end it will give you a path that is used in during the build steps as your prefix directory.

#### Windows

##### x86

```
cd ~/Metrix/depends
make HOST=i686-w64-mingw32 NO_QT=1
```
##### x64

```
cd ~/Metrix/depends
make HOST=x86_64-w64-mingw32 NO_QT=1
```

#### Linux dependancies

##### x86

```
cd ~/Metrix/depends
make HOST="i686-pc-linux-gnu" NO_QT=1
```

##### x64

This one completes the dependancies for the current OS arch. Its assumed that its Linux x64.

```
cd ~/Metrix/depends
make NO_QT=1
```

#### MacOS dependancies

MacOS is slightly different to all others, in that we also need a copy of the MacOS SDK to allow the build.

##### x64

```
cd ~/Metrix/depends
mkdir -p SDKs
cd SDKs
curl https://codeload.github.com/phracker/MacOSX-SDKs/tar.gz/MacOSX10.11.sdk -o MacOSX10.11.sdk.tar.gz
tar xvzf MacOSX10.11.sdk.tar.gz
mv MacOSX-SDKs-MacOSX10.11.sdk/MacOSX10.11.sdk .
cd ..
make HOST=x86_64-apple-darwin14 NO_QT=1 DARWIN_SDK_PATH=`pwd`/SDKs/MacOSX10.11.sdk
```

Build metrixd
-------

You will now go back to the root metrix directory and begin compiling Metrix.  
If you dont wish to use miniupnpc in the build you can change this to --without-miniupnpc.
The CPPFLAGS will also not be required in this instance.

In all builds the final executables will reside in the ~/Metrix/src directory.

**These builds should be completed one at a time. To build for another OS/arch run the follwing:
```
cd ~/Metrix
make clean
```

#### Windows
##### x86

```
cd ..
./autogen.sh
./configure --prefix=`pwd`/depends/i686-w64-mingw32 --with-miniupnpc  --with-incompatible-bdb BDB_LIBS="-ldb_cxx-5.0" CPPFLAGS="-DSTATICLIB -DMINIUPNP_STATICLIB" LDFLAGS="-static-libgcc -static-libstdc++"
make
strip src/metrixd.exe && strip src/metrix-cli.exe
```

##### x64

```
cd ..
./autogen.sh
./configure --prefix=`pwd`/depends/x86_64-w64-mingw32 --with-miniupnpc --with-incompatible-bdb BDB_LIBS="-ldb_cxx-5.0" CPPFLAGS="-DSTATICLIB -DMINIUPNP_STATICLIB" LDFLAGS="-static-libgcc -static-libstdc++"
make
strip src/metrixd.exe && strip src/metrix-cli.exe
```

#### Linux
##### x86

```
cd ..
./autogen.sh
./configure --prefix=`pwd`/depends/i686-pc-linux-gnu --with-incompatible-bdb BDB_LIBS="-ldb_cxx-5.0"
make
strip src/metrixd && strip src/metrix-cli
```

##### x64

```
cd ..
./autogen.sh
./configure --prefix=`pwd`/depends/x86_64-pc-linux-gnu --with-incompatible-bdb BDB_LIBS="-ldb_cxx-5.0"
make
strip src/metrixd && strip src/metrix-cli
```
#### MacOS
##### x64

```
cd ..
./autogen.sh
./configure --prefix=`pwd`/depends/x86_64-apple-darwin14 --disable-ccache --with-incompatible-bdb BDB_LIBS="-ldb_cxx-5.0" CPPFLAGS="-DNDEBUG"
make
```
