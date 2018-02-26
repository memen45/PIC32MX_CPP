#pragma once

//Real-Time Clock & Calendar (RTCC)

#include <cstdint>


struct Rtcc {

    using time_t = union {
        struct {
        uint8_t :8;
        uint8_t seconds1:4;
        uint8_t seconds10:4;
        uint8_t minutes1:4;
        uint8_t minutes10:3;
        uint8_t :1;
        uint8_t hours1:4;
        uint8_t hours10:3;
        uint8_t :1;
        };
        uint32_t w:32;
    };

    using date_t = union {
        struct {
        uint8_t weekday:3;
        uint8_t :5;
        uint8_t day1:4;
        uint8_t day10:2;
        uint8_t :2;
        uint8_t month1:4;
        uint8_t month10:1;
        uint8_t :3;
        uint8_t year1:4;
        uint8_t year10:4;
        };
        uint32_t w:32;
    };

    using datetime_t = struct {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t weekday;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    };

    static void         alarm           (bool);
    static void         chime           (bool);

    enum AMASK : uint8_t {
        HALFSEC = 0, SECOND1, SECOND10, MINUTE1, MINUTE10, HOUR,
        DAY, WEEK, MONTH, YEAR
    };
    static void         alarm_interval  (AMASK);

    static void         alarm_repeat    (uint8_t);
    static void         on              (bool);

    enum OUTSEL : uint8_t { ALMEVT, CLKSEC, CLKSRC, OFF };
    static void         out_pin         (OUTSEL);

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
    static datetime_t   datetime        ();
    static datetime_t   alarm_datetime  ();
    static void         datetime        (datetime_t);
    static void         alarm_datetime  (datetime_t);

    static datetime_t boot_time;

    private:

    static time_t       time            ();
    static date_t       date            ();
    static time_t       alarm_time      ();
    static date_t       alarm_date      ();
    static void         time            (time_t);
    static void         date            (date_t);
    static void         alarm_time      (time_t);
    static void         alarm_date      (date_t);

    static void         unlock          ();
    static void         lock            ();
    static uint8_t      calc_weekday    (date_t);
    static time_t       dt_to_time      (datetime_t);
    static date_t       dt_to_date      (datetime_t);
    static datetime_t   dt_to_dt        (date_t, time_t);

};
