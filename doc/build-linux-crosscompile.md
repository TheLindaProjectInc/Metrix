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

Windows x86:

    sudo apt install g++-mingw-w64-i686

Windows 64:

    sudo apt install g++-mingw-w64-x86-64

Mac OS:

	sudo apt install curl librsvg2-bin libtiff-tools bsdmainutils cmake imagemagick libcap-dev libz-dev libbz2-dev python-setuptools
	
Dependencies
------------

Dependencies will be compiled using the depends build system. This will download and compile the required packages for the OS you wish to build for.

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



Build Lindad
-------


    cd ~/Linda
    ./autogen
	./configure --prefix=`pwd`/depends/x86_64-w64-mingw32 --with-miniupnpc CPPFLAGS="-DSTATICLIB -DMINIUPNP_STATICLIB" LDFLAGS="-static-libgcc -static-libstdc++"
	make
	strip Lindad.exe
	strip Lindad-cli.exe
