#pragma once

/*=============================================================================
 Real-Time Clock & Calendar (RTCC)

 not sure exactly when wrlock is required- various info from datasheet and
 reference manual are not really that specific, or info conflicts
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"
#include "Osc.hpp"

struct Rtcc {

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //alarm mask
    enum AMASK : uint8_t {
        HALFSEC = 0, SECOND1, SECOND10, MINUTE1, MINUTE10, HOUR,
        DAY, WEEK, MONTH, YEAR
    };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //rtcc pin output select
    enum OUTSEL : uint8_t { ALMEVT, CLKSEC, CLKSRC };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //clock prescale
    enum PS : uint8_t { PRE1 = 0, PRE16, PRE64, PRE256 };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //clock select
    enum CLKSEL : uint8_t { SOSC = 0, LPRC, PWRLPIN, FCY };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //clock divide precomputed for 32khz (prescale default 1:1)
    enum : uint16_t { CLK_DIV_32KHZ = 0x3FFF };

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

    static void         alarm           (bool);
    static void         chime           (bool);
    static void         alarm_interval  (AMASK);
    static void         alarm_repeat    (uint8_t);
    static void         on              (bool);
    static void         out             (bool);
    static void         pin_src         (OUTSEL);
    static void         clk_div         (uint16_t);
    static void         clk_frdiv       (uint8_t);
    static void         clk_pre         (PS);
    static void         clk_sel         (CLKSEL);
    static bool         alarm_evt       ();
    static bool         time_busy       ();
    static bool         alarm_busy      ();
    static bool         half_sec        ();
    static uint32_t     time            ();
    static uint32_t     date            ();
    static uint32_t     alarm_time      ();
    static uint32_t     alarm_date      ();
    static void         time            (uint32_t);
    static void         date            (uint32_t);
    static void         alarm_time      (uint32_t);
    static void         alarm_date      (uint32_t);

    //|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    private:

    static Reg r;
    static Syskey sk;

    static void         unlock          ();
    static void         lock            ();
    static void         conset          (uint32_t, uint32_t, bool = 1);
    static void         conval          (uint32_t, uint32_t);

    enum : uint32_t {
        RTCCON1 = 0xBF800000,
            ALARMEN = 1u<<31,
            CHIME = 1<<30,
            AMASK_SHIFT = 24, AMASK_CLR = 15,
            ALMRPT_SHIFT = 16, ALMRPT_CLR = 255,
            ON = 1<<15,
            WRLOCK = 1<<11,
            PINON = 1<<7,
            OUTSEL_SHIFT = 4, OUTSEL_CLR = 7,
        RTCCON2 = 0xBF800010,
            FRDIV_SHIFT = 11, FRDIV_CLR = 31,
            PS_SHIFT = 4, PS_CLR = 3,
            CLKSEL_SHIFT = 9, CLKSEL_CLR = 3,
        RTCSTAT = 0xBF800030,
            ALMSTAT = 1<<5,
            SYSNCSTAT = 1<<2,
            ALMSYNCSTAT = 1<<1,
            HALFSTAT = 1<<0,
        RTCTIME = 0xBF800040,
        RTCDATE = 0xBF800050,
        ALMTIME = 0xBF800060,
        ALMDATE = 0xBF800070,
    };
};

/*=============================================================================
 all functions inline
=============================================================================*/

void Rtcc::alarm(bool tf){
    conset(RTCCON1, ALARMEN, tf);
}
void Rtcc::chime(bool tf){
    conset(RTCCON1, CHIME, tf);
}
void Rtcc::alarm_interval(AMASK e){
    conset(RTCCON1, AMASK_CLR<<AMASK_SHIFT, 0);
    conset(RTCCON1, e<<AMASK_SHIFT, 1);
}
void Rtcc::alarm_repeat(uint8_t v){
    conset(RTCCON1, ALMRPT_CLR<<ALMRPT_SHIFT, 0);
    conset(RTCCON1, v<<ALMRPT_SHIFT, 1);
}
void Rtcc::on(bool tf){
    conset(RTCCON1, ON, tf);
}
void Rtcc::out(bool tf){
    conset(RTCCON1, PINON, tf);
}
void Rtcc::pin_src(OUTSEL v){
    conset(RTCCON1, OUTSEL_CLR<<OUTSEL_SHIFT, 0);
    conset(RTCCON1, v<<OUTSEL_SHIFT, 1);
}
void Rtcc::clk_div(uint16_t v){
    unlock();
    r.val(RTCCON2+2, v);
    lock();
}
void Rtcc::clk_frdiv(uint8_t v){
    conset(RTCCON2, FRDIV_CLR<<FRDIV_SHIFT, 0);
    conset(RTCCON2, (v & FRDIV_CLR)<<FRDIV_SHIFT, 1);
}
void Rtcc::clk_pre(PS e){
    conset(RTCCON2, PS_CLR<<PS_SHIFT, 0);
    conset(RTCCON2, e<<PS_SHIFT, 1);
}
void Rtcc::clk_sel(CLKSEL e){
    if(e == SOSC) Osc::sosc(true);
    conset(RTCCON2, CLKSEL_CLR<<CLKSEL_SHIFT, 0);
    conset(RTCCON2, e<<CLKSEL_SHIFT, 1);
}
bool Rtcc::alarm_evt(){
    return r.anybit(RTCSTAT, ALMSTAT);
}
bool Rtcc::time_busy(){
    return r.anybit(RTCSTAT, SYSNCSTAT);
}
bool Rtcc::alarm_busy(){
    return r.anybit(RTCSTAT, ALMSYNCSTAT);
}
bool Rtcc::half_sec(){
    return r.anybit(RTCSTAT, HALFSTAT);
}
//raw time, date
uint32_t Rtcc::time(){
    return r.val(RTCTIME);
}
uint32_t Rtcc::date(){
    return r.val(RTCDATE);
}
uint32_t Rtcc::alarm_time(){
    return r.val(ALMTIME);
}
uint32_t Rtcc::alarm_date(){
    return r.val(ALMDATE);
}
void Rtcc::time(uint32_t v){
    conval(RTCTIME, v);
} //wrlock
void Rtcc::date(uint32_t v){
    conval(RTCTIME, v);
} //wrlock
void Rtcc::alarm_time(uint32_t v){
    r.val(ALMTIME, v);
}
void Rtcc::alarm_date(uint32_t v){
    r.val(ALMTIME, v);
}

//RTCCON1 lock off by default, these functions will lock RTCCON1 when done
//private functions
void Rtcc::unlock(){
    sk.unlock();
    r.clrbit(RTCCON1, WRLOCK);
}
void Rtcc::lock(){
    r.setbit(RTCCON1, WRLOCK);
    sk.lock();
}
void Rtcc::conset(uint32_t addr, uint32_t v, bool tf){
    unlock();
    r.setbit(addr, v, tf);
    lock();
}
void Rtcc::conval(uint32_t addr, uint32_t v){
    unlock();
    r.val(addr, v);
    lock();
}