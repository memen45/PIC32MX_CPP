#include "Rtcc.hpp"
#include "Osc.hpp"
#include "Reg.hpp"
#include "Sys.hpp"

enum : uint32_t {
    RTCCON1 = 0xBF800000,
        ALARMEN = 31,
        CHIME = 30,
        AMASK_SHIFT = 24, AMASK_MASK = 15,
        ALMRPT_SHIFT = 16, ALMRPT_MASK = 255,
        ON = 15,
        WRLOCK = 11,
        PINON = 7,
        OUTSEL_SHIFT = 4, OUTSEL_MASK = 7,
    RTCCON2 = 0xBF800010,
        FRDIV_SHIFT = 11, FRDIV_MASK = 31,
        PS_SHIFT = 4, PS_MASK = 3,
        CLKSEL_SHIFT = 9, CLKSEL_MASK = 3,
    RTCSTAT = 0xBF800030,
        ALMSTAT = 5,
        SYSNCSTAT = 2,
        ALMSYNCSTAT = 1,
        HALFSTAT = 0,
    RTCTIME = 0xBF800040,
    RTCDATE = 0xBF800050,
    ALMTIME = 0xBF800060,
    ALMDATE = 0xBF800070,
};

//clock divide precomputed for 32khz (prescale default 1:1)
//need 2Hz
enum : uint16_t { CLK_DIV_32KHZ = 0x3FFF };

//-----------------------------------------------------------------private-----
//store boot time
Rtcc::datetime_t Rtcc::m_boot_time;
//-----------------------------------------------------------------------------

//=============================================================================
    auto Rtcc::
    alarm (bool tf) -> void
//=============================================================================
{
    Reg::setbit(RTCCON1, 1<<ALARMEN, tf);
}

//=============================================================================
    auto Rtcc::
    chime (bool tf) -> void
//=============================================================================
{
    Reg::setbit(RTCCON1, 1<<CHIME, tf);
}

//=============================================================================
    auto Rtcc::
    alarm_interval (AMASK e) -> void
//=============================================================================
{
    Reg::clrbit(RTCCON1, AMASK_MASK<<AMASK_SHIFT);
    Reg::setbit(RTCCON1, e<<AMASK_SHIFT);
}

//=============================================================================
    auto Rtcc::
    alarm_repeat (uint8_t v) -> void
//=============================================================================
{
    Reg::clrbit(RTCCON1, ALMRPT_MASK<<ALMRPT_SHIFT);
    Reg::setbit(RTCCON1, v<<ALMRPT_SHIFT);
}

//=============================================================================
    auto Rtcc::
    on (bool tf) -> void
//=============================================================================
{
    //datasheet shows DIV=0 on reset,
    //rtcc frm shows DIV=0x3FFF- frm is correct
    clk_div(CLK_DIV_32KHZ);             //(same as reset value)
    clk_src(Osc::sosc() ? SOSC : LPRC); //use sosc if on
    clk_pre(PRE1);                      //(same as reset value)
    unlock();
    Reg::setbit(RTCCON1, 1<<ON, tf);
    lock();
    //if boot time not already recorded, store it
    //assumes rtcc will be turned on early in startup and datetime
    //already set (or reset was not due to power loss)
    if(not m_boot_time.year) m_boot_time = datetime();
    //if rtcc was reset, and no datetime set, boot_time will still be clear
}

//=============================================================================
    auto Rtcc::
    out_pin (OUTSEL v) -> void
//=============================================================================
{
    if(v != OFF){
        unlock();
        Reg::clrbit(RTCCON1, OUTSEL_MASK<<OUTSEL_SHIFT);
        Reg::setbit(RTCCON1, v<<OUTSEL_SHIFT);
        lock();
    }
    unlock();
    Reg::setbit(RTCCON1, 1<<PINON, v != OFF);
    lock();
}

//=============================================================================
    auto Rtcc::
    clk_div (uint16_t v) -> void
//=============================================================================
{
    unlock();
    Reg::val(RTCCON2+2, v);
    lock();
}

//=============================================================================
    auto Rtcc::
    clk_frdiv (uint8_t v) -> void
//=============================================================================
{
    unlock();
    Reg::clrbit(RTCCON2, FRDIV_MASK<<FRDIV_SHIFT);
    Reg::setbit(RTCCON2, (v bitand FRDIV_MASK)<<FRDIV_SHIFT);
    lock();
}

//=============================================================================
    auto Rtcc::
    clk_pre (PS e) -> void
//=============================================================================
{
    unlock();
    Reg::clrbit(RTCCON2, PS_MASK<<PS_SHIFT);
    Reg::setbit(RTCCON2, e<<PS_SHIFT);
    lock();
}

//=============================================================================
    auto Rtcc::
    clk_src (CLKSEL e) -> void
//=============================================================================
{
    if(e == SOSC) Osc::sosc(true);
    unlock();
    Reg::clrbit(RTCCON2, CLKSEL_MASK<<CLKSEL_SHIFT);
    Reg::setbit(RTCCON2, e<<CLKSEL_SHIFT);
    lock();
}

//=============================================================================
    auto Rtcc::
    alarm_evt () -> bool
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<ALMSTAT);
}

//=============================================================================
    auto Rtcc::
    time_busy () -> bool
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<SYSNCSTAT);
}

//=============================================================================
    auto Rtcc::
    alarm_busy () -> bool
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<ALMSYNCSTAT);
}

//=============================================================================
    auto Rtcc::
    half_sec () -> bool
//=============================================================================
{
    return Reg::anybit(RTCSTAT, 1<<HALFSTAT);
}

//=============================================================================
    auto Rtcc::
    datetime () -> datetime_t
//=============================================================================
{
    time_t t = time();
    date_t d = date();
    datetime_t dt = dt_to_dt(d, t);
    return dt;
}

//=============================================================================
    auto Rtcc::
    alarm_datetime () -> datetime_t
//=============================================================================
{
    time_t t = alarm_time();
    date_t d = alarm_date();
    datetime_t dt = dt_to_dt(d, t);
    return dt;
}

//=============================================================================
    auto Rtcc::
    datetime (datetime_t v) -> void
//=============================================================================
{
    date(dt_to_date(v));
    time(dt_to_time(v));
}

//=============================================================================
    auto Rtcc::
    alarm_datetime (datetime_t v) -> void
//=============================================================================
{
    alarm_date(dt_to_date(v));
    alarm_time(dt_to_time(v));
}

//=============================================================================
    auto Rtcc::
    boot_datetime () -> datetime_t
//=============================================================================
{
    return m_boot_time;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    time () -> time_t
//-----------------------------------------------------------------------------
{
    time_t t;
    t.w = Reg::val(RTCTIME);
    return t;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    date () -> date_t
//-----------------------------------------------------------------------------
{
    date_t d;
    d.w = Reg::val(RTCDATE);
    return d;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    alarm_time () -> time_t
//-----------------------------------------------------------------------------
{
    time_t t;
    t.w = Reg::val(ALMTIME);
    return t;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    alarm_date () -> date_t
//-----------------------------------------------------------------------------
{
    date_t d;
    d.w = Reg::val(ALMDATE);
    return d;
}

//wrlock
//-----------------------------------------------------------------private-----
    auto Rtcc::
    time (time_t v) -> void
//-----------------------------------------------------------------------------
{
    unlock();
    Reg::val(RTCTIME, v.w);
    lock();
}

//wrlock
//-----------------------------------------------------------------private-----
    auto Rtcc::
    date (date_t v) -> void
//-----------------------------------------------------------------------------
{
    v.weekday = calc_weekday(v); //get correct weekday
    unlock();
    Reg::val(RTCDATE, v.w);
    lock();
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    alarm_time (time_t v) -> void
//-----------------------------------------------------------------------------
{
    Reg::val(ALMTIME, v.w);
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    alarm_date (date_t v) -> void
//-----------------------------------------------------------------------------
{
    v.weekday = calc_weekday(v); //get correct weekday
    Reg::val(ALMTIME, v.w);
}

//RTCCON1.ON, RTCCON2, DATE/TIME registers need WRLOCK=0
//-----------------------------------------------------------------private-----
    auto Rtcc::
    unlock () -> void
//-----------------------------------------------------------------------------
{
    Sys::unlock();
    Reg::clrbit(RTCCON1, 1<<WRLOCK);
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    lock () -> void
//-----------------------------------------------------------------------------
{
    Reg::setbit(RTCCON1, 1<<WRLOCK);
    Sys::lock();
}

//calc weekday from date
//(date_t) month 1-12, day 1-31, year 0-99
//return weekday, 0 (sunday) - 6(saturday)
//-----------------------------------------------------------------private-----
    auto Rtcc::
    calc_weekday (date_t v) -> uint8_t
//-----------------------------------------------------------------------------
{
    uint8_t m = v.month10*10 + v.month1;
    uint8_t d = v.day10*10 + v.day1;
    uint16_t y = 2000 + v.year10*10 + v.year1;

    static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    dt_to_date (datetime_t v) -> date_t
//-----------------------------------------------------------------------------
{
    date_t d;
    d.year10 = v.year/10; d.year1 = v.year%10;
    d.month10 = v.month/10; d.month1 = v.month%10;
    d.day10 = v.day/10; d.day1 = v.day %10;
    return d;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    dt_to_time (datetime_t v) -> time_t
//-----------------------------------------------------------------------------
{
    time_t t;
    t.hours10 = v.hour/10; t.hours1 = v.hour%10;
    t.minutes10 = v.minute/10; t.minutes1 = v.minute%10;
    t.seconds10 = v.second/10; t.seconds1 = v.second%10;
    return t;
}

//-----------------------------------------------------------------private-----
    auto Rtcc::
    dt_to_dt (date_t d, time_t t) -> datetime_t
//-----------------------------------------------------------------------------
{
    datetime_t dt;
    dt.year = d.year10*10 + d.year1;
    dt.month = d.month10*10 + d.month1;
    dt.day = d.day10*10 + d.day1;
    dt.weekday = d.weekday;
    dt.hour = t.hours10*10 + t.hours1;
    dt.minute = t.minutes10*10 + t.minutes1;
    dt.second = t.seconds10*10 + t.seconds1;
    dt.hour12 = dt.hour % 12 ? dt.hour % 12 : 12;
    dt.pm = dt.hour >= 12;
    return dt;
}
