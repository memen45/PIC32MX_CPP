#pragma once

/*=============================================================================
 Real-Time Clock & Calendar (RTCC)

 not sure exactly when wrlock is required- various info from datasheet and
 reference manual are not really that specific, or info conflicts
=============================================================================*/

#include <cstdint>
#include "Reg.hpp"
#include "Syskey.hpp"

struct Rtcc {

    //alarm mask
    enum AMASK {
        YEAR = 9<<24, MONTH = 8<<24, WEEK = 7<<24, DAY = 6<<24, HOUR= 5<<24,
        MINUTE10 = 4<<24, MINUTE1 = 3<<24, SECOND10 = 2<<24,
        SECOND1 = 1<<24, HALFSEC = 0, AMASKCLR = 15<<24,
    };
    //rtcc pin output select
    enum OUTSEL { CLKSRC = 2<<4, CLKSEC = 1<<4, ALMEVT = 0<<4, CLRSEL = 7<<4 };
    //clock prescale
    enum PRESCALE { PRE256 = 3<<4, PRE64 = 2<<4, PRE16 = 1<<4, PRE1 = 0<<4 };
    //clock select
    enum CLKSEL { FCY = 3, PWRLPIN = 2, LPRC = 1, SOSC = 0 };
    //clock divide precomputed for 32khz (prescale default 1:1)
    enum { CLK_DIV_32KHZ = 0x3FFF };

    //public functions
    static void         alarm           (bool);
    static void         chime           (bool);
    static void         alarm_interval  (AMASK);
    static void         alarm_repeat    (uint8_t);
    static void         on              (bool);
    static void         out             (bool);
    static void         pin_src         (OUTSEL);
    static void         clk_div         (uint16_t);
    static void         clk_frdiv       (uint8_t);
    static void         clk_pre         (PRESCALE);
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

    private:

    static Reg r;

    //private functions
    static void         unlock          ();
    static void         lock            ();
    static void         conset          (uint32_t, uint32_t, bool = 1);
    static void         conval          (uint32_t, uint32_t);

    enum {
        RTCCON1 = 0xBF800000,
        RTCCON2 = 0xBF800010,
        RTCSTAT = 0xBF800030,
        RTCTIME = 0xBF800040,
        RTCDATE = 0xBF800050,
        ALMTIME = 0xBF800060,
        ALMDATE = 0xBF800070,
        ALARMEN = 1<<31, CHIME = 1<<30, ALMRPTCLR = 7<<16,
        ON = 1<<15, WRLOCK = 1<<11, PINON = 1<<7,
        ALMSTAT = 1<<5, SYSNCSTAT = 1<<2,
        ALMSYNCSTAT = 1<<1, HALFSTAT = 1<<0,
        FRDIVSHIFT = 11, FRDIVCLR = 31<<FRDIVSHIFT
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
    conset(RTCCON1, AMASKCLR, 0);
    conset(RTCCON1, e);
}
void Rtcc::alarm_repeat(uint8_t v){
    conset(RTCCON1, ALMRPTCLR, 0);
    conset(RTCCON1, v);
}
void Rtcc::on(bool tf){
    conset(RTCCON1, ON, tf);
}
void Rtcc::out(bool tf){
    conset(RTCCON1, PINON, tf);
}
void Rtcc::pin_src(OUTSEL v){
    conset(RTCCON1, CLRSEL, 0);
    conset(RTCCON1, v, 0);
}
void Rtcc::clk_div(uint16_t v){
    unlock();
    r.val(RTCCON2+2, v);
    lock();
}
void Rtcc::clk_frdiv(uint8_t v){
    conset(RTCCON2, FRDIVCLR, 0);
    conset(RTCCON2, (v&FRDIVCLR)<<FRDIVSHIFT);
}
void Rtcc::clk_pre(PRESCALE e){
    conset(RTCCON2, PRE256, 0);
    conset(RTCCON2, e);
}
void Rtcc::clk_sel(CLKSEL e){
    conset(RTCCON2, FCY, 0);
    conset(RTCCON2, e);
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
    Syskey::unlock();
    r.setbit(RTCCON1, WRLOCK, 0);
}
void Rtcc::lock(){
    r.setbit(RTCCON1, WRLOCK);
    Syskey::lock();
}
void Rtcc::conset(uint32_t r, uint32_t v, bool tf){
    unlock();
    Reg::setbit(r, v, tf);
    lock();
}
void Rtcc::conval(uint32_t r, uint32_t v){
    Rtcc::unlock();
    Reg::val(r, v);
    Rtcc::lock();
}