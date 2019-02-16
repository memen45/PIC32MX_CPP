#pragma once

//Delay using Coprocessor0 Count

#include <cstdint>

/*
    all delays limited to 300sec (if exceeded, will be 300sec)
    this will provide plenty of spare time to do unsigned calculation
    on the 32bit count number at 24MHz without a rollover occurring
    before the calculation takes place, which could happen if we allow
    the numbers to get close to the max for 32bit

    delays need to be polled if using the set variant
    wait variant will block until count expired

    single delay, blocking-

        Delay::wait(500_ms);

    delay, polled-

        Delay d;
        d.set(500_ms);
        for(;;){
            if(d.expired()){
                do_something();
                d.restart();
            }
            do_something_else();
        }

    if a delay is set (polled version), and you will not get back to it
    within the rollover time, expire it so the next time its checked for
    expiration it will be expired (without doing the calculation, which
    by now could be completely wrong)- not needed much, but there can be
    times it is handy

        Delay d;
        d.set( 100_sec );
        //I now go something else for 5 minutes
        //if I check if expired, could be wrong calculation as rollover
        //has occurred
        //so anytime you want to force an expiration, do-
        d.expire();
        //now next time its checked, it will be expired no matter what rollover
        //occurred
        //not a great example here, but you will run into times a forced expiration
        //will be needed (or simply change your code structure to avoid it)

*/

struct Delay {

            //either specify set time in us
Delay       (uint32_t);

            //or do not set any time
Delay       (){};

            //polled

            //time expired?
            auto
expired     () -> bool;

            //force time expired
            auto
expire      () -> void;

            //restart, do again
            auto
restart     () -> void;

            //set us wait time
            auto
set         (uint32_t) -> void;

            //blocking

            //wait us
            static auto
wait        (uint32_t) -> void;

            private:

            auto
set_count   (uint32_t) -> void;

            uint32_t    m_start{0};
            uint32_t    m_countn{0};
            bool        m_expired{true};

};



//set max us as needed (see info at top of file), 300sec works for me
//=============================================================================
static constexpr uint32_t m_max_us = 300000000UL;

// user defined literals use for us time
// Delay d{ 100_ms };
// d.set( 1_sec );
// d.wait( 100_us );
//=============================================================================
            constexpr uint32_t operator ""
_us         (unsigned long long int us)
            {
            return us;
            }

            constexpr uint32_t operator ""
_ms         (unsigned long long int ms)
            {
            return ms*1000_us;
            }

            constexpr uint32_t operator ""
_sec        (unsigned long long int sec)
            {
            return sec*1000_ms;
            }


