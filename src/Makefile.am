include Makefile.include
AM_CPPFLAGS =  $(INCLUDES) -I$(top_builddir)/src/obj \
  -I$(top_srcdir)/src/leveldb/include -I$(top_srcdir)/src/leveldb/helpers \
  -I$(builddir) $(BOOST_INCLUDES) $(BDB_CPPFLAGS)
AM_LDFLAGS = $(PTHREAD_CFLAGS)

noinst_LIBRARIES = libbitcoin.a

bin_PROGRAMS = Lindad Linda-cli

SUBDIRS = . $(BUILD_QT) $(BUILD_TEST)
DIST_SUBDIRS = . qt test
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
  netbase.h \
  pbkdf2.h \
  protocol.h \
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
  utilstrencodings.h \
  version.h \
  wallet.h \
  walletdb.h \
  crypto/sha1.h \
  crypto/sha256.h \
  crypto/sha512.h \
  crypto/hmac_sha256.h \
  crypto/rfc6979_hmac_sha256.h \
  crypto/hmac_sha512.h \
  crypto/sha1.h \
  crypto/ripemd160.h

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
	@$(MKDIR_P) $(abs_top_builddir)/src/obj
	@$(top_srcdir)/share/genbuild.sh $(abs_top_builddir)/src/obj/build.h \
	  $(abs_top_srcdir)
version.o: obj/build.h

libbitcoin_a_SOURCES = \
  activemasternode.cpp \
  addrman.cpp \
  alert.cpp \
  bitcoin-cli.cpp \
  bitcoind.cpp \
  bloom.cpp \
  chainparams.cpp \
  checkpoints.cpp \
  core.cpp \
  coins.cpp \
  crypter.cpp \
  darksend.cpp \
  db.cpp \
  eccryptoverify.cpp \
  hash.cpp \
  init.cpp \
  instantx.cpp \
  keepass.cpp \
  kernel.cpp \
  key.cpp \
  keystore.cpp \
  leveldbwrapper.cpp \
  main.cpp \
  masternode.cpp \
  masternodeconfig.cpp \
  miner.cpp \
  net.cpp \
  netbase.cpp \
  noui.cpp \
  pbkdf2.cpp \
  protocol.cpp \
  rpcblockchain.cpp \
  rpcclient.cpp \
  rpcdarksend.cpp \
  rpcdump.cpp \
  rpcmining.cpp \
  rpcmisc.cpp \
  rpcnet.cpp \
  rpcprotocol.cpp \
  rpcrawtransaction.cpp \
  rpcserver.cpp \
  rpcwallet.cpp \
  script.cpp \
  scrypt.cpp \
  spork.cpp \
  stealth.cpp \
  sync.cpp \
  timedata.cpp \
  txdb.cpp \
  txmempool.cpp \
  util.cpp \
  utilstrencodings.cpp \
  version.cpp \
  wallet.cpp \
  walletdb.cpp \
  crypto/sha1.cpp \
  crypto/sha256.cpp \
  crypto/sha512.cpp \
  crypto/hmac_sha256.cpp \
  crypto/rfc6979_hmac_sha256.cpp \
  crypto/hmac_sha512.cpp \
  crypto/ripemd160.cpp \
  $(JSON_H) \
  $(BITCOIN_CORE_H)


if GLIBC_BACK_COMPAT
libbitcoin_a_SOURCES += compat/glibc_compat.cpp
libbitcoin_a_SOURCES += compat/glibcxx_compat.cpp
endif

nodist_libbitcoin_a_SOURCES = $(top_srcdir)/src/obj/build.h
#

# Lindad binary #
Lindad_LDADD = \
  libbitcoin.a \
  $(LIBLEVELDB) \
  $(LIBMEMENV)
Lindad_SOURCES = bitcoind.cpp
#

if TARGET_WINDOWS
Lindad_SOURCES += bitcoind-res.rc
endif

AM_CPPFLAGS += $(BDB_CPPFLAGS)
Lindad_LDADD += $(BOOST_LIBS) $(BDB_LIBS)

# Linda-cli binary #
Linda_cli_LDADD = \
  libbitcoin.a \
  $(LIBLEVELDB) \
  $(LIBMEMENV) \
  $(BOOST_LIBS) \
  $(BDB_LIBS)
Linda_cli_SOURCES = bitcoin-cli.cpp
#

if TARGET_WINDOWS
Linda_cli_SOURCES += bitcoin-cli-res.rc
endif

# NOTE: This dependency is not strictly necessary, but without it make may try to build both in parallel, which breaks the LevelDB build system in a race
leveldb/libleveldb.a: leveldb/libmemenv.a

leveldb/%.a:
	@echo "Building LevelDB ..." && $(MAKE) -C $(@D) $(@F) CXX="$(CXX)" \
	  CC="$(CC)" PLATFORM=$(TARGET_OS) AR="$(AR)" $(LEVELDB_TARGET_FLAGS) \
	  OPT="$(CXXFLAGS) $(CPPFLAGS)"

qt/bitcoinstrings.cpp: $(libbitcoin_a_SOURCES)
	@test -n $(XGETTEXT) || echo "xgettext is required for updating translations"
	@cd $(top_srcdir); XGETTEXT=$(XGETTEXT) share/qt/extract_strings_qt.py

CLEANFILES = leveldb/libleveldb.a leveldb/libmemenv.a *.gcda *.gcno

DISTCLEANFILES = obj/build.h

EXTRA_DIST = leveldb Makefile.include 

clean-local:
	-$(MAKE) -C leveldb clean
	rm -f leveldb/*/*.gcno leveldb/helpers/memenv/*.gcno