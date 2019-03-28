AM_CPPFLAGS =  $(INCLUDES)
AM_LDFLAGS = $(PTHREAD_CFLAGS) $(LIBTOOL_LDFLAGS)

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
BITCOIN_INCLUDES = -I$(builddir) -I$(builddir)/obj $(BITCOIN_CONFIG_INCLUDES) $(BOOST_CPPFLAGS) $(LEVELDB_CPPFLAGS)

noinst_LIBRARIES = \
  libbitcoin_server.a \
  libbitcoin_common.a \
  libbitcoin_cli.a \
  crypto/libbitcoin_crypto.a

if ENABLE_WALLET
BITCOIN_INCLUDES += $(BDB_CPPFLAGS)
noinst_LIBRARIES += libbitcoin_wallet.a
endif

bin_PROGRAMS = Lindad Linda-cli

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
  chainparamsseeds.h \
  checkpoints.h \
  checkqueue.h \
  clientversion.h \
  coincontrol.h \
  compat.h \
  core.h \
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
  script.h \
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
  util.h \
  version.h \
  wallet.h \
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
libbitcoin_common_a-version.$(OBJEXT): obj/build.h

libbitcoin_server_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_server_a_SOURCES = \
  activemasternode.cpp \
  addrman.cpp \
  allocators.cpp \
  alert.cpp \
  bitcoin-cli.cpp \
  bitcoind.cpp \
  bloom.cpp \
  checkpoints.cpp \
  coins.cpp \
  init.cpp \
  kernel.cpp \
  keystore.cpp \
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

libbitcoin_wallet_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_wallet_a_SOURCES = \
  db.cpp \
  keepass.cpp \
  crypter.cpp \
  rpcdump.cpp \
  rpcwallet.cpp \
  stealth.cpp \
  wallet.cpp \
  walletdb.cpp \
  $(BITCOIN_CORE_H)

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

libbitcoin_common_a_CPPFLAGS = $(BITCOIN_INCLUDES)
libbitcoin_common_a_SOURCES = \
  base58.cpp \
  allocators.cpp \
  chainparams.cpp \
  core.cpp \
  darksend.cpp \
  eccryptoverify.cpp \
  hash.cpp \
  instantx.cpp \
  key.cpp \
  masternode.cpp \
  masternodeconfig.cpp \
  netbase.cpp \
  protocol.cpp \
  pbkdf2.cpp \
  random.cpp \
  rpcprotocol.cpp \
  script.cpp \
  scrypt.cpp \
  stealth.cpp \
  sync.cpp \
  timedata.cpp \
  util.cpp \
  version.cpp \
  compat/glibc_sanity.cpp \
  compat/glibcxx_sanity.cpp \
  $(BITCOIN_CORE_H)

libbitcoin_cli_a_SOURCES = \
  rpcclient.cpp \
  $(BITCOIN_CORE_H)

if GLIBC_BACK_COMPAT
libbitcoin_common_a_SOURCES += compat/glibc_compat.cpp
libbitcoin_common_a_SOURCES += compat/glibcxx_compat.cpp
endif

nodist_libbitcoin_common_a_SOURCES = $(srcdir)/obj/build.h
#

# Lindad binary #
Lindad_LDADD = \
  libbitcoin_server.a \
  libbitcoin_common.a \
  crypto/libbitcoin_crypto.a \
  $(LIBLEVELDB) \
  $(LIBMEMENV)
if ENABLE_WALLET
Lindad_LDADD += libbitcoin_wallet.a
endif
Lindad_SOURCES = bitcoind.cpp
#

if TARGET_WINDOWS
Lindad_SOURCES += bitcoind-res.rc
endif

Lindad_LDADD += $(BOOST_LIBS) $(BDB_LIBS)
Lindad_CPPFLAGS = $(BITCOIN_INCLUDES)

# Linda-cli binary #
Linda_cli_LDADD = \
  libbitcoin_cli.a \
  libbitcoin_common.a \
  crypto/libbitcoin_crypto.a \
  $(BOOST_LIBS)
Linda_cli_SOURCES = bitcoin-cli.cpp
Linda_cli_CPPFLAGS = $(BITCOIN_INCLUDES)
#

if TARGET_WINDOWS
Linda_cli_SOURCES += bitcoin-cli-res.rc
endif

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb

clean-local:
	-$(MAKE) -C leveldb clean
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

LIBBITCOIN_SERVER=libbitcoin_server.a
LIBBITCOIN_WALLET=libbitcoin_wallet.a
LIBBITCOIN_COMMON=libbitcoin_common.a
LIBBITCOIN_CLI=libbitcoin_cli.a
LIBBITCOIN_CRYPTO=crypto/libbitcoin_crypto.a
