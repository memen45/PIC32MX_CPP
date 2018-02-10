#pragma once

//Real-Time Clock & Calendar (RTCC)

#include <cstdint>
#include "Reg.hpp"
#include "Sys.hpp"

struct Rtcc {

    //alarm mask
    enum AMASK : uint8_t {
        HALFSEC = 0, SECOND1, SECOND10, MINUTE1, MINUTE10, HOUR,
        DAY, WEEK, MONTH, YEAR
    };

    //rtcc pin output select
    enum OUTSEL : uint8_t { ALMEVT, CLKSEC, CLKSRC };

    //clock prescale
    enum PS : uint8_t { PRE1 = 0, PRE16, PRE64, PRE256 };

    //clock select
    enum CLKSEL : uint8_t { SOSC = 0, LPRC, PWRLPIN, FCY };

    //clock divide precomputed for 32khz (prescale default 1:1)
    //need 2Hz
    enum : uint16_t { CLK_DIV_32KHZ = 0x3FFF };

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
    static void         clk_src         (CLKSEL);
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
    static Sys sys;

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
