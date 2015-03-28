/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_STATE_HPP
#define CLOWDER_STATE_HPP

#include <clowder/common.hpp>

#include <atomic>
#include <iostream>

#include <botan/build.h>
#include <botan/version.h>
#include <botan/init.h>
#if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1, 11, 14)
#   include <botan/libstate.h>
#endif

namespace clowder
{

class uses_botan
{
#if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1, 11, 0)
#   error "libclowder requires at least version 1.11.0 of botan"
#elif BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1, 11, 14)
private:
    static std::atomic_bool _no_conflict;
    static std::atomic_size_t _count;

protected:
    uses_botan()
    {
        size_t old = _count++;
        bool exists = Botan::Global_State_Management::global_state_exists();
        if (0 == old) {
            if (exists) {
                _no_conflict = true;
            } else {
                Botan::LibraryInitializer::initialize();
            }
        }
    }

public:
    virtual ~uses_botan()
    {
        size_t now = --_count;
        if (0 == now) {
            if (_no_conflict) {
                _no_conflict = false;
            } else {
                Botan::LibraryInitializer::deinitialize();
            }
        }
    }
#endif
};

}

#endif /* CLOWDER_STATE_HPP */
