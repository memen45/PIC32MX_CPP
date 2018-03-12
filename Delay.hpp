#pragma once

//Delay using Coprocessor0 Count

#include <cstdint>

/*
    all delays limited to 300sec (if exceeded, will be 300sec)
    this will provide plenty of spare time to do unsigned calculation
    on the 32bit count number at 24MHz without a rollover occurring
    before the calculation takes place, which could happen if we allow
    the numbers to get close to the max for 32bit

    delays need to be polled if using the set_ variant
    wait_ variants will block until count expired

    single delay, blocking-

        Delay::wait_ms(500);

    delay, polled-

        Delay d;
        d.set_ms(500);
        for(;;){
            if(d.expired()){
                do_something();
                d.restart();
            }
            do_something_else();
        }
*/

struct Delay {

    //polled
    bool            expired     ();             //time expired?
    void            restart     ();             //restart, do again
    void            set_us      (uint32_t);     //set us wait time
    void            set_ms      (uint32_t);     //set ms wait time
    void            set_s       (uint16_t);     //set s wait time

    //blocking
    static void     wait_us     (uint32_t);
    static void     wait_ms     (uint32_t);
    static void     wait_s      (uint16_t);

    private:

    void            set_count   (uint32_t);

    uint32_t    m_start{0};
    uint32_t    m_countn{0};
    bool        m_expired{true};

};
