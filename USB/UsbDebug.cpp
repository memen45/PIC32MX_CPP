#include "UsbDebug.hpp"
#include <cstdio>   //printf
#include "Cp0.hpp"  //print cp0 count
#include <cstdarg>  //variadic args


bool UsbDebug::m_enable{false};
Uart* UsbDebug::m_uart{nullptr};


// set uart device, optionally enable/disable (default = enable)
//=============================================================================
            auto UsbDebug::
debug       (Uart* u, bool tf) -> void
            {
            m_uart = u;
            m_enable = tf;
            }

// enable/disable
//=============================================================================
            auto UsbDebug::
debug       (bool tf) -> void
            {
            m_enable = tf;
            }

// get enable/disable state
//=============================================================================
            auto UsbDebug::
debug       () -> bool
            {
            return m_enable;
            }




