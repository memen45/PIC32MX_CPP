#include "UsbDebug.hpp"
#include <cstdio>   //printf
#include "Cp0.hpp"  //print cp0 count
#include <cstdarg>  //variadic args


bool UsbDebug::m_enable{false};
Uart* UsbDebug::m_uart{nullptr};

// constructor specifying uart
//=============================================================================
            UsbDebug::
UsbDebug    (Uart* u)
            {
            debug(u);
            }

// set putc device
//=============================================================================
            auto UsbDebug::
debug       (Uart* u) -> void
            {
            m_uart = u;
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




