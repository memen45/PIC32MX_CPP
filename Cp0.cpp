#include "Cp0.hpp"
#include "Irq.hpp"
#include "Reg.hpp"
#include "Osc.hpp"

//-----------------------------------------------------------------private-----
static uint32_t m_compare_count; //save count for reloads
//-----------------------------------------------------------------------------

//=============================================================================
            auto Cp0::
count       () -> uint32_t
            {
            return __builtin_mfc0(9, 0);
            }

//=============================================================================
            auto Cp0::
compare     () -> uint32_t
            {
            return __builtin_mfc0(11, 0);
            }

//=============================================================================
            auto Cp0::
count       (uint32_t v) -> void
            {
            __builtin_mtc0(9, 0, v);
            }

//=============================================================================
            auto Cp0::
compare     (uint32_t v) -> void
            {
            __builtin_mtc0(11, 0, v);
            }

//=============================================================================
            auto Cp0::
compare_reload () -> void
            {
            compare(count() + m_compare_count);
            }

//=============================================================================
            auto Cp0::
compare_irq (bool tf) -> void
            {
            Irq::flag_clr(Irq::CORE_TIMER);
            Irq::on(Irq::CORE_TIMER, tf);
            }

//=============================================================================
            auto Cp0::
compare_us  (uint32_t v) -> void
            {
            m_compare_count = Osc::sysclk() / 2000000 * v;
            compare_reload();
            }

//=============================================================================
            auto Cp0::
compare_ms  (uint16_t v) -> void
            {
            compare_us(v * 1000);
            }
