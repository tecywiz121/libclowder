/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_COMMON_HPP
#define CLOWDER_COMMON_HPP

/* ************************************************************************* *
 * Public API Visibility                                                     *
 * ************************************************************************* */
#if defined(_WIN32) || defined(__CYGWIN__)
#   define CLOWDER_SHARED_IMPORT __declspec(dllimport)
#   define CLOWDER_SHARED_EXPORT __declspec(dllexport)
#else /* WIN32 */
#   if __GNUC__ >= 4
#       define CLOWDER_SHARED_IMPORT __attribute__ ((visibility ("default")))
#       define CLOWDER_SHARED_EXPORT __attribute__ ((visibility ("default")))
#   else
#       define CLOWDER_SHARED_IMPORT
#       define CLOWDER_SHARED_EXPORT
#   endif /* __GNUC__ */
#endif

#ifdef CLOWDER_SHARED                             // Defined if using shared lib.
#   ifdef CLOWDER_SHARED_EXPORTS                  // Defined when building clowder.
#       define CLOWDER_API CLOWDER_SHARED_EXPORT
#   else
#       define CLOWDER_API CLOWDER_SHARED_IMPORT
#   endif /* CLOWDER_SHARED_EXPORTS */
#else                                           // Must be a static library.
#   define CLOWDER_API
#endif /* CLOWDER_SHARED */

#include <stdexcept>

class CLOWDER_API not_implemented : public std::logic_error
{
public:
    explicit not_implemented(const char* what_arg)
        : logic_error(what_arg) {}
    explicit not_implemented(const std::string& what_arg)
        : logic_error(what_arg) {}
};

#endif /* CLOWDER_COMMON_HPP */
