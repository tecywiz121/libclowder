/* vim: et sw=4 sts=4 ts=4 : */
#ifndef CLOWDER_STATE_HPP
#define CLOWDER_STATE_HPP

#include <clowder/common.hpp>

#include <atomic>
#include <iostream>

#include <botan/init.h>
#include <botan/libstate.h>

namespace clowder
{

class uses_botan
{
private:
    static std::atomic_size_t _count;

protected:
    uses_botan()
    {
        size_t old = _count++;
        bool exists = Botan::Global_State_Management::global_state_exists();
        if (!exists && 0 == old) {
            Botan::LibraryInitializer::initialize();
        }
    }

public:
    virtual ~uses_botan()
    {
        size_t now = --_count;
        if (0 == now) {
            Botan::LibraryInitializer::deinitialize();
        }
    }
};

}

#endif /* CLOWDER_STATE_HPP */
