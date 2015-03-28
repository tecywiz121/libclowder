/* vim: et sw=4 sts=4 ts=4 : */
#include "state.hpp"

namespace clowder
{

#if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1, 11, 14)
std::atomic_size_t uses_botan::_count;
#endif

}

