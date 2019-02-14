#pragma once

#include <cstdio>  //printf
#include <cstdbool>
#include "Uart.hpp"
#include "Cp0.hpp"

// send out debug info to a uart

struct UsbDebug  {

            //set which uart device, optionally enable/disable
            static auto
debug       (Uart*, bool = true) -> void;

            //enable/disable
            static auto
debug       (bool) -> void;

            //get enable/disable
            static auto
debug       (void) -> bool;

            //printf type
            auto
debug       (char const *fmt, ...) -> void;

            //printf type with filename,function header
            auto
debug       (char const* fil, char const* func, char const* fmt, ...) -> void;

    private:

            static Uart* m_uart;
            static bool m_enable;

};

