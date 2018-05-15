#include "Pmd.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum {
    PMDCON = 0xBF8035B0,
        PMDLOCK = 11,
    PMD1 = 0xBF8035C0
};

//-----------------------------------------------------------------private-----
    static auto
    unlock () -> void
//-----------------------------------------------------------------------------
{
    Sys::unlock();
    Reg::clrbit(PMDCON, 1<<PMDLOCK);
}

//-----------------------------------------------------------------private-----
    static auto
    lock () -> void
//-----------------------------------------------------------------------------
{
    Reg::setbit(PMDCON, 1<<PMDLOCK);
    Sys::lock();
}

//=============================================================================
    auto Pmd::
    off (PMD e) -> void
//=============================================================================
{
    unlock();
    Reg::setbit(PMD1 + 16 * (e / 32), (1<<(e % 32)));
    lock();
}

//=============================================================================
    auto Pmd::
    on (PMD e) -> void
//=============================================================================
{
    unlock();
    Reg::clrbit(PMD1 + 16 * (e / 32), (1<<(e % 32)));
    lock();
}

//array of modules to disable/enable
//=============================================================================
    auto Pmd::
    off (PMD* e, uint8_t sz) -> void
//=============================================================================
{
    unlock();
    for(uint8_t i = 0; i < sz; i++){
        Reg::setbit(PMD1 + 16 * (e[i] / 32), (1<<(e[i] % 32)));
    }
    lock();
}

//=============================================================================
    auto Pmd::
    on (PMD* e, uint8_t sz) -> void
//=============================================================================
{
    unlock();
    for(uint8_t i = 0; i < sz; i++){
        Reg::clrbit(PMD1 + 16 * (e[i] / 32), (1<<(e[i] % 32)));
    }
    lock();
}
