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

    if a delay is set (polled version), and you will not get back to it
    within the rollover time, expire it so the next time its checked for
    expiration it will be expired (without doing the calculation, which
    by now could be completely wrong)- not needed much, but there can be
    times it is handy

        Delay d{ 1000 };
        //I now go something else for 5 minutes
        //if I check if expired, could be wrong calculation as rollover
        //has occurred
        //so anytime you want to force an expiration, do-
        d.expire();
        //now next time its checked, it will be expired no matter what rollover
        //occurred
        //not a great example here, but you will run into times a forced expiration
        //will be needed (or simply change your code sctructure to avoid it)

*/

struct Delay {

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
set_us      (uint32_t) -> void;

            //set ms wait time
            auto
set_ms      (uint32_t) -> void;

            //set s wait time
            auto
set_s       (uint16_t) -> void;


            //blocking

            //wait us
            static auto
wait_us     (uint32_t) -> void;

            //wait ms
            static auto
wait_ms     (uint32_t) -> void;

            //wait s
            static auto
wait_s      (uint16_t) -> void;


            private:

            auto
set_count   (uint32_t) -> void;

            uint32_t    m_start{0};
            uint32_t    m_countn{0};
            bool        m_expired{true};

};
