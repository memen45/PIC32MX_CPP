#pragma once

//Real-Time Clock & Calendar (RTCC)

#include <cstdint>
#include "Reg.hpp"


struct Rtcc : private Reg {

            //friendly date/time format
            //hour12, pm, weekday not used when setting datetime
            //(weekday is calculated when setting)
            using
datetime_t  = struct {
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
alarm       (bool) -> void;

            static auto
alarm       () -> bool;

            static auto
chime       (bool) -> void;

            enum
AMASK       : uint8_t {
            HALFSEC, SECOND1, SECOND10, MINUTE1, MINUTE10,
            HOUR, DAY, WEEK, MONTH, YEAR
            };

            static auto
alarm_interval (AMASK) -> void;

            static auto
alarm_repeat (uint8_t) -> void;

            static auto
on          (bool) -> void;

            enum
OUTSEL      : uint8_t { ALMEVT, CLKSEC, OFF };

            static auto
out_pin     (OUTSEL) -> void;

            static auto
alarm_evt   () -> bool;

            static auto
time_busy   () -> bool;

            static auto
alarm_busy  () -> bool;

            static auto
half_sec    () -> bool;

            static auto
datetime    () -> datetime_t;

            static auto
alarm_datetime () -> datetime_t;

            static auto
datetime    (datetime_t) -> void;

            static auto
alarm_datetime (datetime_t) -> void;

            static auto
boot_datetime () -> datetime_t;


            private:

            //store boot time
            static datetime_t m_boot_time;

};
