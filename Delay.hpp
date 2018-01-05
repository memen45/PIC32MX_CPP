#pragma once

/*=============================================================================
 Delay using Coprocessor0 Count
=============================================================================*/

#include <cstdint>
#include "Cp0.hpp"
#include "Osc.hpp"

class DelayCP0 {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    public:

    bool    expired     ();
    void    reset       ();
    void    wait_us     (uint32_t);
    void    wait_ms     (uint32_t);
    void    set_us      (uint32_t);
    void    set_ms      (uint32_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Cp0 cp0;
    static Osc osc;

    void    reset       (uint32_t);
    void    wait        (uint32_t);

    uint32_t    m_start{0};
    uint32_t    m_countn{0};
    bool        m_expired{true};
};
