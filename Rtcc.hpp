#pragma once

//Real-Time Clock & Calendar (RTCC)

#include <cstdint>


struct Rtcc {

    static void         alarm           (bool);
    static void         chime           (bool);

    enum AMASK : uint8_t {
        HALFSEC = 0, SECOND1, SECOND10, MINUTE1, MINUTE10, HOUR,
        DAY, WEEK, MONTH, YEAR
    };
    static void         alarm_interval  (AMASK);

    static void         alarm_repeat    (uint8_t);
    static void         on              (bool);
    static void         out             (bool);

    enum OUTSEL : uint8_t { ALMEVT, CLKSEC, CLKSRC };
    static void         pin_src         (OUTSEL);

    static void         clk_div         (uint16_t);
    static void         clk_frdiv       (uint8_t);

    enum PS : uint8_t { PRE1 = 0, PRE16, PRE64, PRE256 };
    static void         clk_pre         (PS);

    enum CLKSEL : uint8_t { SOSC = 0, LPRC, PWRLPIN, FCY };
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

    static void         unlock          ();
    static void         lock            ();
    static void         conset          (uint32_t, uint32_t, bool = 1);
    static void         conval          (uint32_t, uint32_t);

};
