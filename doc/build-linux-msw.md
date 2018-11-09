Copyright (c) 2018 Linda Project INC Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
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
 libdb4.8    | Berkeley DB      | Wallet storage (only needed when wallet enabled)
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

Dependency Build Instructions: Ubuntu & Debian
----------------------------------------------
Build requirements:

    sudo apt-get install build-essential libtool automake autotools-dev autoconf pkg-config libssl-dev libgmp3-dev libevent-dev bsdmainutils gcc-multilib g++-multilib

To cross compile for 32bit and 64bit systems both these below will be required. If you only want to compile for one architecture, just install the one you want.

x86:

    sudo apt install g++-mingw-w64-i686

64:

    sudo apt install g++-mingw-w64-x86-64

	
Dependencies
------------

Dependencies will directly be compiled from source for the selected architecture and placed here:

	~/deps
    mkdir ~/deps

You should only compile for 1 architecture in this deps folder. If you wish to compile for both x86 and x64 then 
you will need to either use a different deps folder and link to it in your makefile. Or clean all compiled builds 
and re-compile using the other architecture strings.

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

OpenSSL
-----------

Compiling Openssl

```
cd ~/deps
wget -O openssl-1.0.2g.tar.gz https://www.openssl.org/source/old/1.0.2/openssl-1.0.2g.tar.gz
tar xvzf openssl-1.0.2g.tar.gz
cd openssl-1.0.2g
```
x86:

    ./Configure --cross-compile-prefix=i686-w64-mingw32- no-shared no-dso mingw
x64:

    ./Configure --cross-compile-prefix=x86_64-w64-mingw32- no-shared no-dso mingw64
    make depend
    make



Build Berkeley DB
-----------
Statically build Berkeley DB

```
cd ~/deps
wget -O db-5.0.32.NC.tar.gz http://download.oracle.com/berkeley-db/db-5.0.32.NC.tar.gz
tar xvfz db-5.0.32.NC.tar.gz
cd db-5.0.32.NC/build_unix
```
x86:

    sh ../dist/configure --host=i686-w64-mingw32 --enable-mingw --enable-cxx --disable-replication

x64:

    sh ../dist/configure --host=x86_64-w64-mingw32 --enable-mingw --enable-cxx --disable-replication

    make

Build Boost
-----
Statically build the Boost libraries

```
cd ~/deps
wget -O boost_1_58_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz/download
tar xvfz boost_1_58_0.tar.gz
cd boost_1_58_0
```
x86:

    echo "using gcc : : i686-w64-mingw32-g++ ;" > user-config.jam

x64:

    echo "using gcc : : x86_64-w64-mingw32-g++ ;" > user-config.jam

```
./bootstrap.sh
./b2 --user-config=user-config.jam --with-chrono --with-filesystem --with-program_options --with-system --with-thread toolset=gcc-mingw threading=multi threadapi=win32 --layout=tagged target-os=windows release
```


Build MiniUPnPc
---------
UPnP support is optional, make with `USE_UPNP=1` to enable it.

```
cd ~/deps
wget -O miniupnpc-1.9.tar.gz http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.9.tar.gz
tar xvzf miniupnpc-1.9.tar.gz
mv miniupnpc-1.9 miniupnpc
cd miniupnpc
```
The below is a fix for miniupnp to allow cross compilation. This can be removed in later versions of miniupnp as a patch has been added to fix this.
```
echo "miniupnpcstrings.h:     miniupnpcstrings.h.in VERSION" > fixfile
echo -e "\tsed 's|OS_STRING \".*\"|OS_STRING \"Windows/Mingw32\"|' \$< | \\" >> fixfile 
echo -e "\tsed 's|MINIUPNPC_VERSION_STRING \".*\"|MINIUPNPC_VERSION_STRING \"\$(shell cat VERSION)\"|' > \$@" >> fixfile
sed -i '71s/.*//' Makefile.mingw
sed -i '72s/.*//' Makefile.mingw
sed -i '71r fixfile' Makefile.mingw
```

x86:

    make -f Makefile.mingw DLLWRAP=i686-w64-mingw32-dllwrap CC=i686-w64-mingw32-gcc AR=i686-w64-mingw32-ar init upnpc-static

x64:

    make -f Makefile.mingw DLLWRAP=x86_64-w64-mingw32-dllwrap CC=x86_64-w64-mingw32-gcc AR=x86_64-w64-mingw32-ar init upnpc-static


Build Zlib
-------

```
cd ~/deps
wget -O zlib-1.2.11.tar.gz https://zlib.net/zlib-1.2.11.tar.gz
tar xvzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
```

x86:

```
sed -e s/"PREFIX ="/"PREFIX = i686-w64-mingw32-"/ -i win32/Makefile.gcc
make -f win32/Makefile.gcc
sudo BINARY_PATH=/usr/i686-w64-mingw32/bin INCLUDE_PATH=/usr/i686-w64-mingw32/include LIBRARY_PATH=/usr/i686-w64-mingw32/lib make -f win32/Makefile.gcc install SHARED_MODE=1
```

x64:

```
sed -e s/"PREFIX ="/"PREFIX = x86_64-w64-mingw32-"/ -i win32/Makefile.gcc
make -f win32/Makefile.gcc
sudo BINARY_PATH=/usr/x86_64-w64-mingw32/bin INCLUDE_PATH=/usr/x86_64-w64-mingw32/include LIBRARY_PATH=/usr/x86_64-w64-mingw32/lib make -f win32/Makefile.gcc install SHARED_MODE=1
```

Build Lindad
-------

Build leveldb

    cd ~/Linda/src/leveldb
    make clean
x86:

    CC=i686-w64-mingw32-gcc CXX=i686-w64-mingw32-g++ TARGET_OS=OS_WINDOWS_CROSSCOMPILE make libleveldb.a libmemenv.a
x64:

    CC=x86_64-w64-mingw32-gcc CXX=x86_64-w64-mingw32-g++ TARGET_OS=OS_WINDOWS_CROSSCOMPILE make libleveldb.a libmemenv.a


Build Secp256k1

x86:

    cd ~/Linda/src/secp256k1
    ./autogen
    ./configure --host=i686-w64-mingw32
    make

x64:

    cd ~/Linda/src/secp256k1
    ./autogen
    ./configure --host=x86_64-w64-mingw32
    make

Build Lindad
-------

    cd ~/Linda/src
    make -f makefile.linux-mingw
    strip Lindad.exe
