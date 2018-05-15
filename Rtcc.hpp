#pragma once

//Real-Time Clock & Calendar (RTCC)

#include <cstdint>


struct Rtcc {

    //friendly date/time format
    //hour12 and pm not used when setting datetime
    using datetime_t = struct {
        uint8_t year;       //0-99
        uint8_t month;      //1-12
        uint8_t day;        //1-31
        uint8_t weekday;    //0-6
        uint8_t hour;       //0-23
        uint8_t minute;     //0-59
        uint8_t second;     //0-59
        uint8_t hour12;     //1-12
        bool pm;            //false=am, true=pm
    };

    static auto
    alarm (bool) -> void;

    static auto
    chime (bool) -> void;

    enum AMASK : uint8_t {
        HALFSEC, SECOND1, SECOND10, MINUTE1, MINUTE10,
        HOUR, DAY, WEEK, MONTH, YEAR
    };
    static auto
    alarm_interval (AMASK) -> void;

    static auto
    alarm_repeat (uint8_t) -> void;

    static auto
    on (bool) -> void;

    enum OUTSEL : uint8_t { ALMEVT, CLKSEC, CLKSRC, OFF };
    static auto
    out_pin (OUTSEL) -> void;

    static auto
    clk_div (uint16_t) -> void;

    static auto
    clk_frdiv (uint8_t) -> void;

    enum PS : uint8_t { PRE1, PRE16, PRE64, PRE256 };
    static auto
    clk_pre (PS) -> void;


    enum CLKSEL : uint8_t { SOSC, LPRC, PWRLPIN, FCY };
    static auto
    clk_src (CLKSEL) -> void;

    static auto
    alarm_evt () -> bool;

    static auto
    time_busy () -> bool;

    static auto
    alarm_busy () -> bool;

    static auto
    half_sec () -> bool;

    static auto
    datetime () -> datetime_t;

    static auto
    alarm_datetime () -> datetime_t;

    static auto
    datetime (datetime_t) -> void;

    static auto
    alarm_datetime (datetime_t) -> void;

    static auto
    boot_datetime () -> datetime_t;


    private:

    //time registers
    using time_t = union {
        struct {
        uint8_t             :8;
        uint8_t seconds1    :4;
        uint8_t seconds10   :4;
        uint8_t minutes1    :4;
        uint8_t minutes10   :3;
        uint8_t             :1;
        uint8_t hours1      :4;
        uint8_t hours10     :3;
        uint8_t             :1;
        };
        uint32_t w          :32;
    };

    //date registers
    using date_t = union {
        struct {
        uint8_t weekday     :3;
        uint8_t             :5;
        uint8_t day1        :4;
        uint8_t day10       :2;
        uint8_t             :2;
        uint8_t month1      :4;
        uint8_t month10     :1;
        uint8_t             :3;
        uint8_t year1       :4;
        uint8_t year10      :4;
        };
        uint32_t w          :32;
    };

    static auto
    time () -> time_t;

    static auto
    date () -> date_t;

    static auto
    alarm_time () -> time_t;

    static auto
    alarm_date () -> date_t;

    static auto
    time (time_t) -> void;

    static auto
    date (date_t) -> void;

    static auto
    alarm_time (time_t) -> void;

    static auto
    alarm_date (date_t) -> void;

    static auto
    unlock () -> void;

    static auto
    lock () -> void;

    static auto
    calc_weekday (date_t) -> uint8_t;

    static auto
    dt_to_time (datetime_t) -> time_t;

    static auto
    dt_to_date (datetime_t) -> date_t;

    static auto
    dt_to_dt (date_t, time_t) -> datetime_t;


    static datetime_t m_boot_time;      //store boot time
};
