#pragma once

//Delay using Coprocessor0 Count

#include <cstdint>
#include "Cp0.hpp"
#include "Osc.hpp"

/*
    all delays limited to 300sec (if exceeded, will be 300sec)
    this will provide plenty of spare time to do unsigned calculation
    on the 32bit count number at 24MHz without a rollover occurring
    before the calculation takes place, which could happen if we allow
    the numbers to get close to the max for 32bit

    delays need to be polled if using the set_ variant
    wait_ variants will block until count expired
*/

class DelayCP0 {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    public:

    bool    expired     ();             //time expired?
    void    restart     ();             //using same time, restart
    void    wait_us     (uint32_t);     //blocking us wait
    void    wait_ms     (uint32_t);     //blocking ms wait
    void    wait_s      (uint16_t);     //blocking s wait
    void    set_us      (uint32_t);     //set us wait time, no block (poll)
    void    set_ms      (uint32_t);     //set ms wait time, no block (poll)
    void    set_s       (uint16_t);     //set s wait time, no block (poll)

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    private:

    static Cp0 cp0;
    static Osc osc;

    void    set_count   (uint32_t);

    uint32_t    m_start{0};
    uint32_t    m_countn{0};
    bool        m_expired{true};

};
