#pragma once

/*=============================================================================
 Delay using Coprocessor0 Count
=============================================================================*/

#include <cstdint>

class DelayCP0 {

    public:

    //public functions
    bool    expired     (void);
    void    reset       (void);
    void    wait_us     (uint32_t);
    void    wait_ms     (uint32_t);
    void    set_us      (uint32_t);
    void    set_ms      (uint32_t);

    //constructor
    DelayCP0(uint8_t = 24);

private:

    //private functions
    void    reset       (uint32_t);
    void    wait        (uint32_t);

    //private vars
    uint32_t    m_start;
    uint32_t    m_countn;
    uint8_t     m_cpu_mhz;
    bool        m_expired;
};

