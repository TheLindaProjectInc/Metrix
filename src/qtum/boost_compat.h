// Compatibility shims for building the vendored src/cpp-ethereum submodule against
// modern Boost, without patching the submodule itself. Force-included ahead of every
// translation unit via -include in configure.ac. Grows as new Boost/submodule
// incompatibilities turn up; each fix documents what it's working around.
#pragma once

// Force-included via -include into every translation unit (see configure.ac), C and
// C++ alike, so this must be a no-op for C (plain #define flags are safe there, but
// namespaces/templates/Boost headers are not).
#if defined(__cplusplus)

#include <boost/version.hpp>

// Boost's BOOST_THROW_EXCEPTION macro used to expand to a call to the internal
// helper boost::exception_detail::throw_exception_(x, current_function, file, line).
// Since Boost 1.73 (confirmed present in 1.72.0, gone in 1.73.0) that helper was
// removed in favor of boost::throw_exception(x, source_location) with the macro
// calling that new form directly instead.
//
// src/cpp-ethereum/libdevcore/Common.cpp calls the old internal helper by its fully
// qualified name directly (not via the macro), so it fails to compile against any
// Boost new enough to have dropped it. This restores that old helper with its
// original implementation (see Boost 1.72.0's boost/throw_exception.hpp).
#if BOOST_VERSION >= 107300
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>
#include <boost/throw_exception.hpp>

namespace boost { namespace exception_detail {

template <class E>
BOOST_NORETURN inline void
throw_exception_(E const& x, char const* current_function, char const* file, int line)
{
    boost::throw_exception(
        boost::enable_error_info(x) <<
        boost::throw_function(current_function) <<
        boost::throw_file(file) <<
        boost::throw_line(line));
}

}} // namespace boost::exception_detail
#endif // BOOST_VERSION >= 107300

// boost::filesystem::ifstream/ofstream (thin wrappers over std::ifstream/ofstream
// that accept a boost::filesystem::path directly) live in
// <boost/filesystem/fstream.hpp>, which used to be pulled in by the <boost/filesystem.hpp>
// umbrella header but no longer is on newer Boost (confirmed missing from the
// umbrella on Boost 1.83). src/cpp-ethereum/libdevcore/CommonIO.cpp only includes
// the umbrella header but uses boost::filesystem::ifstream/ofstream, so pull the
// dedicated header in here. The classes themselves still exist unchanged.
#include <boost/filesystem/fstream.hpp>

#endif // defined(__cplusplus)
