Copyright (c) 2009-2012 Bitcoin Developers
Distributed under the MIT/X11 software license, see the accompanying
file license.txt or http://www.opensource.org/licenses/mit-license.php.
This product includes software developed by the OpenSSL Project for use in
the OpenSSL Toolkit (http://www.openssl.org/).  This product includes
cryptographic software written by Eric Young (eay@cryptsoft.com) and UPnP
software written by Thomas Bernard.


See readme-qt.rst for instructions on building Linda QT, the
graphical user interface.

WINDOWS BUILD NOTES
===================

Compilers Supported
-------------------
Since we need to compile with QT5 statically, we will use recent mingw-w64 compiler.
Download Msys2 from their site here: https://www.msys2.org/ OR use Chocolatey Package Manager to install Msys2.

Prepare environment
-------------------
In your install directory c:\msys64, execute mingw32.exe for msys shell with path to 32bit mingw compiler or mingw64.exe for msys shell with path to 64bit compiler. We will use 32bit compiler in this guide.

In MSYS launched from Mingw32.exe for the first time, we need to download up to date packages:

	pacman -Sy
	pacman --needed -S bash pacman pacman-mirrors msys2-runtime

You must exit out from MSYS2-shell, restart MSYS2-shell, then run below command, to complete rest of other components update:

	pacman -Su

Download development packages:

	pacman -S base-devel git mercurial cvs wget p7zip
	pacman -S perl ruby python2 mingw-w64-i686-toolchain

Install QT 5
---------------
Obtain Pre-Built Qt files and Use instantly without Building/Compiling (version currently used is 5.10.1)

	pacman -S mingw-w64-i686-qt5-static mingw-w64-i686-jasper 
	
Dependencies
------------

Other dependencies will directly be compiled from source and placed here:

	C:\deps

They are already built on windows, but sourcecode is included so that you can rebuild them if required.

Their licenses:

	Berkeley DB    New BSD license with additional requirement that linked software must be free open source
	Boost          MIT-like license
	miniupnpc      New (3-clause) BSD license
	OpenSSL        Old BSD license with the problematic advertising requirement

Versions used in this release:

	Berkeley DB   5.0.32.NC
	Boost         1.58.0
	miniupnpc     1.9
	OpenSSL       1.0.2g

OpenSSL
-----------
One of Linda’s dependencies that will not need to be compiled cause static lib package available (currently 1.0.2n):

	pacman -S mingw-w64-i686-openssl

If you prefer to build manually download 1.0.2g version here: https://www.openssl.org/source/old/1.0.2/openssl-1.0.2g.tar.gz<br>
place archive in deps folders then from Mingw32.exe or Mingw64.exe:
```
tar xvfz openssl-1.0.2g.tar.gz
cd openssl-1.0.2g 
// for 32-bit
./configure no-shared no-dso mingw
// for 64-bit
./configure no-shared no-dso mingw64
make depend
make
```


Build Berkeley DB
-----------
Download Berkley DB 5.0.32 from here 
http://download.oracle.com/berkeley-db/db-5.0.32.NC.tar.gz
<br>Put the archive in c:\deps.

MSYS shell launched from Mingw32.exe or Mingw64.exe:

	cd /c/deps/
	tar xvfz db-5.0.32.NC.tar.gz

	cd /c/db-5.0.32.NC/build_unix
	sh ../dist/configure --enable-mingw --enable-cxx --disable-replication
	make

Build Boost
-----
Download Boost 1.58 here : https://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.gz/download
<br>Put the archive in c:\deps

From Mingw Msys shell:

	cd /c/deps
	tar xvfz boost_1_58_0.tar.gz

From DOS prompt configured for MingW:
(You must remember to set your path properly for the architecture you are compiling for. msys64/mingw32/bin for x86 and msys64/mingw64/bin for x64. Also, in both of those directories, you must rename mingw32make.exe to make.exe in order to run make in the CMD enviroment. To switch to x64 you will have to switch your path. (Settings > Enviroment Variables)

	cd c:\deps\boost_1_58_0
	bootstrap.bat mingw
	b2 --build-type=complete --with-chrono --with-filesystem --with-program_options --with-system --with-thread toolset=gcc variant=release link=static threading=multi runtime-link=static stage

Build MiniUPnPc
---------
UPnP support is optional, make with `USE_UPNP=` to disable it.
Download it from here http://miniupnp.free.fr/files/download.php?file=miniupnpc-1.9.tar.gz
and place it in your deps folder, next from the msys shell unpack it like this.

	cd /c/deps/
	tar xvfz miniupnpc-1.9.tar.gz

From DOS prompt configured for MingW (launch cmd after setting PATH variable):

	cd c:\deps\miniupnpc-1.9
	make -f Makefile.mingw init upnpc-static
	
Important: then rename "miniupnpc-1.9" directory to "miniupnpc".

Build Linda QT
-------
First we need to adapt c:\Linda\Linda-qt.pro file to fit dependencies compiled above. Modify this section at the beginning of the file accordingly:

    windows {
      DEPS_PATH = C:/deps
      SECP256K1_LIB_PATH = src/secp256k1/.libs
      SECP256K1_INCLUDE_PATH = src/secp256k1/include
      MINIUPNPC_LIB_PATH = $$DEPS_PATH/miniupnpc
      MINIUPNPC_INCLUDE_PATH = $$DEPS_PATH
      BOOST_LIB_PATH = $$DEPS_PATH/boost_1_58_0/stage/lib
      BOOST_INCLUDE_PATH = $$DEPS_PATH/boost_1_58_0
      BOOST_LIB_SUFFIX= -mgw73-mt-s-1_58
      BDB_LIB_PATH = $$DEPS_PATH/db-5.0.32.NC/build_unix
      BDB_INCLUDE_PATH = $$DEPS_PATH/db-5.0.32.NC/build_unix
	  # If you built OpenSSL manually uncomment the below
      #OPENSSL_LIB_PATH = $$DEPS_PATH/
      #OPENSSL_INCLUDE_PATH = $$DEPS_PATH/
    }

Build leveldb, in MSYS shell executed from Mingw32.exe:

    cd /c/Linda/src/leveldb
    make clean
    TARGET_OS=NATIVE_WINDOWS make libleveldb.a libmemenv.a

Still in MSYS2 shell, build Secp256k1

    cd /c/Linda/src/secp256k1
    ./autogen.sh
    ./configure
    make

then it's finally possible to compile Linda from Mingw32.exe or Mingw64.exe Msys shell:

	export PATH=/mingw32/qt5-static/bin/:$PATH

	cd /c/Linda
	make clean
	qmake
	make -f Makefile.Release

NOTE: instructions for compiling 64bit windows wallet are the same except that:
- Mingw64.exe will be launched as MSYS2 shell.
- packages name that were beginning with "mingw-w64-i686-" for 32bit env, must be replaced by "mingw-w64-x86_64-" when using pacman to install 64bit libs
-You MUST set your path variable for the CMD prompt sections to work. Ensure you follow those steps properly.
- following path will be used before compiling Linda QT:

      export PATH=/mingw64/qt5-static/bin/:$PATH


Build Lindad
-------
DOS prompt:

    cd c:\Linda\src
    mingw32-make -f makefile.mingw
    strip Lindad.exe
