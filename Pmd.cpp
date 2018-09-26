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
unlock      () -> void
            {
            Sys::unlock();
            Reg::clrbit(PMDCON, 1<<PMDLOCK);
            }

//-----------------------------------------------------------------private-----
            static auto
lock        () -> void
            {
            Reg::setbit(PMDCON, 1<<PMDLOCK);
            Sys::lock();
            }

//=============================================================================
            auto Pmd::
disable     (PMD e, bool tf) -> void
            {
            unlock();
            if(e == ALL){ //all PMD1-7 (0x10 register spacing)
                for(uint8_t i = 0; i < 7*16; i += 16) Reg::setbit(PMD1+i, tf);
            } else {
                Reg::setbit(PMD1 + 16 * (e / 32), (1<<(e % 32)), tf);
            }
            lock();
            }
