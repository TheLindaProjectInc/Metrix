// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_UTIL_H
#define BITCOIN_UTIL_H

#ifndef WIN32
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

#include "serialize.h"
#include "tinyformat.h"

#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/thread.hpp>
#include <boost/version.hpp>

#include <openssl/bio.h>
#include <openssl/bn.h>
#include <openssl/buffer.h>
#include <openssl/crypto.h> // for OPENSSL_cleanse()
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <stdint.h>

class uint256;

static const int64_t COIN = 100000000;
static const int64_t CENT = 1000000;

#define BEGIN(a) ((char*)&(a))
#define END(a) ((char*)&((&(a))[1]))
#define UBEGIN(a) ((unsigned char*)&(a))
#define UEND(a) ((unsigned char*)&((&(a))[1]))
#define ARRAYLEN(array) (sizeof(array) / sizeof((array)[0]))

#define UVOIDBEGIN(a) ((void*)&(a))
#define CVOIDBEGIN(a) ((const void*)&(a))
#define UINTBEGIN(a) ((uint32_t*)&(a))
#define CUINTBEGIN(a) ((const uint32_t*)&(a))

// This is needed because the foreach macro can't get over the comma in pair<t1, t2>
#define PAIRTYPE(t1, t2) std::pair<t1, t2>

boost::filesystem::path GetMasternodeConfigFile();

inline void MilliSleep(int64_t n)
{
#if defined(HAVE_WORKING_BOOST_SLEEP_FOR)
    boost::this_thread::sleep_for(boost::chrono::milliseconds(n));
#elif defined(HAVE_WORKING_BOOST_SLEEP)
    boost::this_thread::sleep(boost::posix_time::milliseconds(n));
#else
    //should never get here
#error missing boost sleep implementation
#endif
}

//Dark features

extern bool fMasterNode;
extern bool fLiteMode;
extern int nInstantXDepth;
extern int nDarksendRounds;
extern int nAnonymizeMetrixAmount;
extern int nLiquidityProvider;
extern bool fEnableDarksend;
extern int64_t enforceMasternodePaymentsTime;
extern std::string strMasterNodeAddr;
extern int keysLoaded;
extern bool fSucessfullyLoaded;
extern std::vector<int64_t> darkSendDenominations;


extern std::map<std::string, std::string> mapArgs;
extern std::map<std::string, std::vector<std::string> > mapMultiArgs;
extern bool fDebug;
extern bool fPrintToConsole;
extern bool fPrintToDebugLog;
extern bool fServer;
extern bool fCommandLine;
extern std::string strMiscWarning;
extern bool fLogTimestamps;
extern bool fLogIPs;
extern volatile bool fReopenDebugLog;

void SetupEnvironment();


/* Return true if log accepts specified category */
bool LogAcceptCategory(const char* category);
/* Send a string to the log output */
int LogPrintStr(const std::string& str);

#define LogPrintf(...) LogPrint(NULL, __VA_ARGS__)

/* When we switch to C++11, this can be switched to variadic templates instead
 * of this macro-based construction (see tinyformat.h).
 */
#define MAKE_ERROR_AND_LOG_FUNC(n)                                                              \
    /*   Print to debug.log if -debug=category switch is given OR category is NULL. */          \
    template <TINYFORMAT_ARGTYPES(n)>                                                           \
    static inline int LogPrint(const char* category, const char* format, TINYFORMAT_VARARGS(n)) \
    {                                                                                           \
        if (!LogAcceptCategory(category))                                                       \
            return 0;                                                                           \
        return LogPrintStr(tfm::format(format, TINYFORMAT_PASSARGS(n)));                        \
    }                                                                                           \
    /*   Log error and return false */                                                          \
    template <TINYFORMAT_ARGTYPES(n)>                                                           \
    static inline bool error(const char* format, TINYFORMAT_VARARGS(n))                         \
    {                                                                                           \
        LogPrintStr("ERROR: " + tfm::format(format, TINYFORMAT_PASSARGS(n)) + "\n");            \
        return false;                                                                           \
    }

TINYFORMAT_FOREACH_ARGNUM(MAKE_ERROR_AND_LOG_FUNC)

/* Zero-arg versions of logging and error, these are not covered by
 * TINYFORMAT_FOREACH_ARGNUM
 */
static inline int LogPrint(const char* category, const char* format)
{
    if (!LogAcceptCategory(category))
        return 0;
    return LogPrintStr(format);
}
static inline bool error(const char* format)
{
    LogPrintStr(std::string("ERROR: ") + format + "\n");
    return false;
}


void PrintExceptionContinue(std::exception* pex, const char* pszThread);
void ParseParameters(int argc, const char* const argv[]);
void FileCommit(FILE* fileout);
bool RenameOver(boost::filesystem::path src, boost::filesystem::path dest);
bool TryCreateDirectory(const boost::filesystem::path& p);
bool TruncateFile(FILE* file, unsigned int length);
int RaiseFileDescriptorLimit(int nMinFD);
void AllocateFileRange(FILE* file, unsigned int offset, unsigned int length);
boost::filesystem::path GetDefaultDataDir(const std::string dirName="metrix");
const boost::filesystem::path& GetDataDir(bool fNetSpecific = true);
boost::filesystem::path GetConfigFile();
#ifndef WIN32
boost::filesystem::path GetPidFile();
void CreatePidFile(const boost::filesystem::path& path, pid_t pid);
#endif
void ReadConfigFile(std::map<std::string, std::string>& mapSettingsRet, std::map<std::string, std::vector<std::string> >& mapMultiSettingsRet);
#ifdef WIN32
boost::filesystem::path GetSpecialFolderPath(int nFolder, bool fCreate = true);
#endif
void ShrinkDebugFile();
void runCommand(std::string strCommand);
void checkMigrateDataDir();

inline std::string leftTrim(std::string src, char chr)
{
    std::string::size_type pos = src.find_first_not_of(chr, 0);

    if (pos > 0)
        src.erase(0, pos);

    return src;
}

inline bool IsSwitchChar(char c)
{
#ifdef WIN32
    return c == '-' || c == '/';
#else
    return c == '-';
#endif
}

/**
 * Return string argument or default value
 *
 * @param strArg Argument to get (e.g. "-foo")
 * @param default (e.g. "1")
 * @return command-line argument or default value
 */
std::string GetArg(const std::string& strArg, const std::string& strDefault);

/**
 * Return integer argument or default value
 *
 * @param strArg Argument to get (e.g. "-foo")
 * @param default (e.g. 1)
 * @return command-line argument (0 if invalid number) or default value
 */
int64_t GetArg(const std::string& strArg, int64_t nDefault);

/**
 * Return boolean argument or default value
 *
 * @param strArg Argument to get (e.g. "-foo")
 * @param default (true or false)
 * @return command-line argument or default value
 */
bool GetBoolArg(const std::string& strArg, bool fDefault);

/**
 * Set an argument if it doesn't already have a value
 *
 * @param strArg Argument to set (e.g. "-foo")
 * @param strValue Value (e.g. "1")
 * @return true if argument gets set, false if it already had a value
 */
bool SoftSetArg(const std::string& strArg, const std::string& strValue);

/**
 * Set a boolean argument if it doesn't already have a value
 *
 * @param strArg Argument to set (e.g. "-foo")
 * @param fValue Value (e.g. false)
 * @return true if argument gets set, false if it already had a value
 */
bool SoftSetBoolArg(const std::string& strArg, bool fValue);

void SetThreadPriority(int nPriority);
void RenameThread(const char* name);

// Standard wrapper for do-something-forever thread functions.
// "Forever" really means until the thread is interrupted.
// Use it like:
//   new boost::thread(boost::bind(&LoopForever<void (*)()>, "dumpaddr", &DumpAddresses, 900000));
// or maybe:
//    boost::function<void()> f = boost::bind(&FunctionWithArg, argument);
//    threadGroup.create_thread(boost::bind(&LoopForever<boost::function<void()> >, "nothing", f, milliseconds));
template <typename Callable>
void LoopForever(const char* name, Callable func, int64_t msecs)
{
    std::string s = strprintf("Metrix-%s", name);
    RenameThread(s.c_str());
    LogPrintf("%s thread start\n", name);
    try {
        while (1) {
            MilliSleep(msecs);
            func();
        }
    } catch (boost::thread_interrupted) {
        LogPrintf("%s thread stop\n", name);
        throw;
    } catch (std::exception& e) {
        PrintExceptionContinue(&e, name);
        throw;
    } catch (...) {
        PrintExceptionContinue(NULL, name);
        throw;
    }
}
// .. and a wrapper that just calls func once
template <typename Callable>
void TraceThread(const char* name, Callable func)
{
    std::string s = strprintf("Metrix-%s", name);
    RenameThread(s.c_str());
    try {
        LogPrintf("%s thread start\n", name);
        func();
        LogPrintf("%s thread exit\n", name);
    } catch (boost::thread_interrupted) {
        LogPrintf("%s thread interrupt\n", name);
        throw;
    } catch (std::exception& e) {
        PrintExceptionContinue(&e, name);
        throw;
    } catch (...) {
        PrintExceptionContinue(NULL, name);
        throw;
    }
}

#endif // BITCOIN_UTIL_H