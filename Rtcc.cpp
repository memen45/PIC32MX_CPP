#include "Rtcc.hpp"
#include "Osc.hpp"

//=============================================================================
    void        Rtcc::alarm             (bool tf)
//=============================================================================
{
    conset(RTCCON1, ALARMEN, tf);
}

//=============================================================================
    void        Rtcc::chime             (bool tf)
//=============================================================================
{
    conset(RTCCON1, CHIME, tf);
}

//=============================================================================
    void        Rtcc::alarm_interval    (AMASK e)
//=============================================================================
{
    conset(RTCCON1, AMASK_CLR<<AMASK_SHIFT, 0);
    conset(RTCCON1, e<<AMASK_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::alarm_repeat      (uint8_t v)
//=============================================================================
{
    conset(RTCCON1, ALMRPT_CLR<<ALMRPT_SHIFT, 0);
    conset(RTCCON1, v<<ALMRPT_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::on                (bool tf)
//=============================================================================
{
    if(tf and r.val16(RTCCON2 + 2)){        //div not set, so
        clk_div(CLK_DIV_32KHZ);             //init ourselves
        if(Osc::sosc()) clk_src(SOSC);      //use sosc if on
        else clk_src(LPRC);                 //else use lprc
        clk_pre(PRE1);                      //should be already
    }
    conset(RTCCON1, ON, tf);
}

//=============================================================================
    void        Rtcc::out               (bool tf)
//=============================================================================
{
    conset(RTCCON1, PINON, tf);
}

//=============================================================================
    void        Rtcc::pin_src           (OUTSEL v)
//=============================================================================
{
    conset(RTCCON1, OUTSEL_CLR<<OUTSEL_SHIFT, 0);
    conset(RTCCON1, v<<OUTSEL_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::clk_div           (uint16_t v)
//=============================================================================
{
    unlock();
    r.val(RTCCON2+2, v);
    lock();
}

//=============================================================================
    void        Rtcc::clk_frdiv         (uint8_t v)
//=============================================================================
{
    conset(RTCCON2, FRDIV_CLR<<FRDIV_SHIFT, 0);
    conset(RTCCON2, (v & FRDIV_CLR)<<FRDIV_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::clk_pre           (PS e)
//=============================================================================
{
    conset(RTCCON2, PS_CLR<<PS_SHIFT, 0);
    conset(RTCCON2, e<<PS_SHIFT, 1);
}

//=============================================================================
    void        Rtcc::clk_src           (CLKSEL e)
//=============================================================================
{
    if(e == SOSC) Osc::sosc(true);
    conset(RTCCON2, CLKSEL_CLR<<CLKSEL_SHIFT, 0);
    conset(RTCCON2, e<<CLKSEL_SHIFT, 1);
}

//=============================================================================
    bool        Rtcc::alarm_evt         ()
//=============================================================================
{
    return r.anybit(RTCSTAT, ALMSTAT);
}

//=============================================================================
    bool        Rtcc::time_busy         ()
//=============================================================================
{
    return r.anybit(RTCSTAT, SYSNCSTAT);
}

//=============================================================================
    bool        Rtcc::alarm_busy        ()
//=============================================================================
{
    return r.anybit(RTCSTAT, ALMSYNCSTAT);
}

//=============================================================================
    bool        Rtcc::half_sec          ()
//=============================================================================
{
    return r.anybit(RTCSTAT, HALFSTAT);
}

//raw time, date
//=============================================================================
    uint32_t    Rtcc::time              ()
//=============================================================================
{
    return r.val(RTCTIME);
}

//=============================================================================
    uint32_t    Rtcc::date              ()
//=============================================================================
{
    return r.val(RTCDATE);
}

//=============================================================================
    uint32_t    Rtcc::alarm_time        ()
//=============================================================================
{
    return r.val(ALMTIME);
}

//=============================================================================
    uint32_t    Rtcc::alarm_date        ()
//=============================================================================
{
    return r.val(ALMDATE);
}

//=============================================================================
    void        Rtcc::time              (uint32_t v)
//=============================================================================
{
    conval(RTCTIME, v);
} //wrlock
//=============================================================================
    void        Rtcc::date              (uint32_t v)
//=============================================================================
{
    conval(RTCTIME, v);
} //wrlock
//=============================================================================
    void        Rtcc::alarm_time        (uint32_t v)
//=============================================================================
{
    r.val(ALMTIME, v);
}

//=============================================================================
    void        Rtcc::alarm_date        (uint32_t v)
//=============================================================================
{
    r.val(ALMTIME, v);
}

//RTCCON1 lock off by default, these functions will lock RTCCON1 when done
//private functions
//=============================================================================
    void        Rtcc::unlock            ()
//=============================================================================
{
    sys.unlock();
    r.clrbit(RTCCON1, WRLOCK);
}

//=============================================================================
    void        Rtcc::lock              ()
//=============================================================================
{
    r.setbit(RTCCON1, WRLOCK);
    sys.lock();
}

//=============================================================================
    void        Rtcc::conset            (uint32_t addr, uint32_t v, bool tf)
//=============================================================================
{
    unlock();
    r.setbit(addr, v, tf);
    lock();
}

//=============================================================================
    void        Rtcc::conval            (uint32_t addr, uint32_t v)
//=============================================================================
{
    unlock();
    r.val(addr, v);
    lock();
}