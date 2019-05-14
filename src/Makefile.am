AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)

if USE_LIBSECP256K1
secp256k1/libsecp256k1.la: $(wildcard secp256k1/src/*) $(wildcard secp256k1/include/*)
	@$(MAKE) $(AM_MAKEFLAGS) -C $(@D) $(@F)
endif

if EMBEDDED_LEVELDB
LEVELDB_CPPFLAGS = -I$(srcdir)/leveldb/include
LEVELDB_CPPFLAGS += -I$(srcdir)/leveldb/helpers/memenv
LIBLEVELDB = $(builddir)/leveldb/libleveldb.a
LIBMEMENV  = $(builddir)/leveldb/libmemenv.a

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
$(LIBLEVELDB): $(LIBMEMENV)

$(LIBLEVELDB) $(LIBMEMENV):
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
          OPT="$(CXXFLAGS) $(CPPFLAGS)"
endif

BITCOIN_CONFIG_INCLUDES = -I$(builddir)/config
BITCOIN_INCLUDES = -I$(builddir) -I$(builddir)/obj $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS) $(CRYPTO_CFLAGS) $(SSL_CFLAGS)

if USE_LIBSECP256K1
BITCOIN_INCLUDES += -I$(srcdir)/secp256k1/include
endif

LIBBITCOIN_SERVER=libbitcoin_server.a
LIBBITCOIN_WALLET=libbitcoin_wallet.a
LIBBITCOIN_COMMON=libbitcoin_common.a
LIBBITCOIN_CLI=libbitcoin_cli.a
LIBBITCOIN_UTIL=libbitcoin_util.a
LIBBITCOIN_CRYPTO=crypto/libbitcoin_crypto.a
LIBBITCOIN_UNIVALUE=univalue/libbitcoin_univalue.a

noinst_LIBRARIES = \
  libbitcoin_server.a \
  libbitcoin_common.a \
  libbitcoin_cli.a \
  libbitcoin_util.a \
  univalue/libbitcoin_univalue.a \
  crypto/libbitcoin_crypto.a
  

if ENABLE_WALLET
BITCOIN_INCLUDES += $(BDB_CPPFLAGS)
noinst_LIBRARIES += libbitcoin_wallet.a
endif

bin_PROGRAMS =
TESTS =

if BUILD_BITCOIND
  bin_PROGRAMS += Lindad
endif

if BUILD_BITCOIN_UTILS
  bin_PROGRAMS += Linda-cli Linda-tx
endif

SUBDIRS = .
DIST_SUBDIRS = .
.PHONY: FORCE
# Linda core #
BITCOIN_CORE_H = \
  activemasternode.h \
  addrman.h \
  alert.h \
  allocators.h \
  base58.h \
  bignum.h \
  bloom.h \
  chainparams.h \
  chainparamsbase.h \
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  compat.h \
  compat/sanity.h \
  core.h \
  core_io.h \
  coins.h \
  crypter.h \
  darksend.h \
  db.h \
  eccryptoverify.h \
  hash.h \
  init.h \
  instantx.h \
  keepass.h \
  kernel.h \
  key.h \
  keystore.h \
  leveldbwrapper.h \
  limitedmap.h \
  main.h \
  masternode.h \
  masternodeconfig.h \
  miner.h \
  mruset.h \
  net.h \
  noui.h \
  netbase.h \
  pbkdf2.h \
  protocol.h \
  random.h \
  rpcclient.h \
  rpcprotocol.h \
  rpcserver.h \
  script/compressor.h \
  script/interpreter.h \
  script/script.h \
  script/sign.h \
  script/standard.h \
  scrypt.h \
  serialize.h \
  spork.h \
  stealth.h \
  strlcpy.h \
  sync.h \
  threadsafety.h \
  timedata.h \
  tinyformat.h \
  txdb.h \
  txmempool.h \
  uint256.h \
  ui_interface.h \
  utilstrencodings.h \
  utilmoneystr.h \
  utiltime.h \
  util.h \
  version.h \
  wallet.h \
  wallet_ismine.h \
  walletdb.h \
  compat/sanity.h

JSON_H = \
  json/json_spirit.h \
  json/json_spirit_error_position.h \
  json/json_spirit_reader.h \
  json/json_spirit_reader_template.h \
  json/json_spirit_stream_reader.h \
  json/json_spirit_utils.h \
  json/json_spirit_value.h \
  json/json_spirit_writer.h \
  json/json_spirit_writer_template.h

obj/build.h: FORCE
	@$(MKDIR_P) $(builddir)/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
libbitcoin_util_a-version.$(OBJEXT): obj/build.h

# server: bitcoind
libbitcoin_server_a_CPPFLAGS = $(BITCOIN_INCLUDES) $(MINIUPNPC_CPPFLAGS)
libbitcoin_server_a_SOURCES = \
  activemasternode.cpp \
  addrman.cpp \
  alert.cpp \
  bitcoind.cpp \
  bloom.cpp \
  checkpoints.cpp \
  init.cpp \
  kernel.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  miner.cpp \
  net.cpp \
  noui.cpp \
  rpcblockchain.cpp \
  rpcdarksend.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  spork.cpp \
  txdb.cpp \
  txmempool.cpp \
  $(JSON_H) \
  $(BITCOIN_CORE_H)

# wallet: bitcoind, but only linked
# when wallet enabled
libbitcoin_wallet_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_wallet_a_SOURCES = \
  db.cpp \
  keepass.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  stealth.cpp \
  wallet.cpp \
  wallet_ismine.cpp \
  walletdb.cpp \
  $(BITCOIN_CORE_H)

# crypto primitives library
crypto_libbitcoin_crypto_a_CPPFLAGS = $(BITCOIN_CONFIG_INCLUDES)
crypto_libbitcoin_crypto_a_SOURCES = \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  crypto/common.h \
  crypto/sha1.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/sha1.h \
  crypto/ripemd160.h

# univalue JSON library
univalue_libbitcoin_univalue_a_SOURCES = \
  univalue/univalue.cpp \
  univalue/univalue_read.cpp \
  univalue/univalue_write.cpp \
  univalue/univalue_escapes.h \
  univalue/univalue.h

# common: shared between bitcoind and non-server tools
libbitcoin_common_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_common_a_SOURCES = \
  allocators.cpp \
  base58.cpp \
  chainparams.cpp \
  coins.cpp \
  core.cpp \
  core_read.cpp \
  core_write.cpp \
  darksend.cpp \
  eccryptoverify.cpp \
  hash.cpp \
  instantx.cpp \
  key.cpp \
  keystore.cpp \
  masternode.cpp \
  masternodeconfig.cpp \
  netbase.cpp \
  protocol.cpp \
  pbkdf2.cpp \
  script/compressor.cpp \
  script/interpreter.cpp \
  script/script.cpp \
  script/sign.cpp \
  script/standard.cpp \
  scrypt.cpp \
  stealth.cpp \
  timedata.cpp \
  $(BITCOIN_CORE_H)

# util: shared between all executables.
# This library *must* be included to make sure that the glibc
# backward-compatibility objects and their sanity checks are linked.
libbitcoin_util_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_util_a_SOURCES = \
  allocators.cpp \
  compat/strnlen.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  chainparamsbase.cpp \
  random.cpp \
  rpcprotocol.cpp \
  sync.cpp \
  uint256.cpp \
  util.cpp \
  utilstrencodings.cpp \
  utilmoneystr.cpp \
  utiltime.cpp \
  version.cpp \
  $(BITCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libbitcoin_util_a_SOURCES += compat/glibc_compat.cpp
libbitcoin_util_a_SOURCES += compat/glibcxx_compat.cpp
endif

libbitcoin_cli_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(BITCOIN_CORE_H)

nodist_libbitcoin_util_a_SOURCES = $(srcdir)/obj/build.h
#

# Lindad binary #
Lindad_LDADD = \
  $(LIBBITCOIN_SERVER) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_UNIVALUE) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBLEVELDB) \
  $(LIBMEMENV)

if USE_LIBSECP256K1
  Lindad_LDADD += secp256k1/libsecp256k1.la
endif

if ENABLE_WALLET
Lindad_LDADD += libbitcoin_wallet.a
endif
Lindad_SOURCES = bitcoind.cpp
#

if TARGET_WINDOWS
Lindad_SOURCES += bitcoind-res.rc
endif

Lindad_LDADD += $(BOOST_LIBS) $(BDB_LIBS) $(SSL_LIBS) $(CRYPTO_LIBS)
Lindad_CPPFLAGS = $(BITCOIN_INCLUDES)
Lindad_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS)

# Linda-cli binary #
Linda_cli_LDADD = \
  $(LIBBITCOIN_CLI) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO) \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS)

Linda_cli_SOURCES = \
	bitcoin-cli.cpp

if USE_LIBSECP256K1
  Linda_cli_LDADD += secp256k1/libsecp256k1.la
endif
Linda_cli_CPPFLAGS = $(BITCOIN_INCLUDES)
#

# Linda-tx binary #
Linda_tx_LDADD = \
  $(LIBBITCOIN_UNIVALUE) \
  $(LIBBITCOIN_COMMON) \
  $(LIBBITCOIN_UTIL) \
  $(LIBBITCOIN_CRYPTO) \
  $(LIBBITCOIN_CRYPTO)

if USE_LIBSECP256K1
  Linda_tx_LDADD += secp256k1/libsecp256k1.la
endif

  Linda_tx_LDADD += \
  $(BOOST_LIBS) \
  $(SSL_LIBS) \
  $(CRYPTO_LIBS)
Linda_tx_SOURCES = bitcoin-tx.cpp
Linda_tx_CPPFLAGS = $(BITCOIN_INCLUDES)
#
Linda_tx_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS)


if TARGET_WINDOWS
Linda_cli_SOURCES += bitcoin-cli-res.rc
endif
Linda_cli_LDFLAGS = $(RELDFLAGS) $(AM_LDFLAGS)

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb secp256k1

clean-local:
	-$(MAKE) -C leveldb clean
	-$(MAKE) -C secp256k1 clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno
	-rm -f config.h

.rc.o:
	@test -f $(WINDRES) && $(WINDRES) -i $< -o $@ || \
	  echo error: could not build $@

.mm.o:
	$(OBJC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	  $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)  -c -o $@ $<

%.pb.cc %.pb.h: %.proto
	test -f $(PROTOC) && $(PROTOC) --cpp_out=$(@D) --proto_path=$(abspath $(<D) $<)  || \
	  echo error: could not build $@
