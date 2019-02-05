#include "Rtcc.hpp"
#include "Osc.hpp"
#include "Sys.hpp"

enum : uint32_t {
RTCCON = 0xBF800200,
	CAL_SHIFT = 0, CAL_MASK = 1023,
    ON = 15,
	SIDL = 13,
	RTSECSEL = 7,
	RTCCLKON = 6,
	RTCWREN = 3,
	SYNCSTAT = 2,
	HALFSTAT = 1,
	PINON = 0,
RTCALRM = 0xBF800210,
    ALRMEN = 15,
    CHIME = 14,
	PIV = 13,
	ALRMSYNCSTAT = 12,
    AMASK_SHIFT = 8, AMASK_MASK = 15,
    ALMRPT_SHIFT = 0, ALMRPT_MASK = 255,
RTCTIME = 0xBF800220,
RTCDATE = 0xBF800230,
ALMTIME = 0xBF800240,
ALMDATE = 0xBF800250
};

//time registers
using time_t = union {
    struct {
    uint8_t             :8;
    uint8_t seconds1    :4;
    uint8_t seconds10   :4;
    uint8_t minutes1    :4;
    uint8_t minutes10   :4;
    uint8_t hours1      :4;
    uint8_t hours10     :4;
    };
    uint32_t w          :32;
};

//date registers
using date_t = union {
    struct {
    uint8_t weekday     :4;
    uint8_t             :4;
    uint8_t day1        :4;
    uint8_t day10       :4;
    uint8_t month1      :4;
    uint8_t month10     :4;
    uint8_t year1       :4;
    uint8_t year10      :4;
    };
    uint32_t w          :32;
};

//-----------------------------------------------------------------private-----
//store boot time
Rtcc::datetime_t Rtcc::m_boot_time;

//-----------------------------------------------------------------private-----
            static auto
read_verify (uint32_t reg) -> uint32_t
            {
            uint32_t v;
            while( (v = Reg::val(reg)) != Reg::val(reg) );
            return v;
            }

//-----------------------------------------------------------------private-----
            static auto
write_verify (uint32_t reg, uint32_t val) -> void
            {
            do{ Reg::val(reg, val); } while( Reg::val(reg) != val );
            }


//-----------------------------------------------------------------private-----
            static auto
time        () -> time_t
            {
            time_t t;
            t.w = read_verify(RTCTIME);
            return t;
            }

//-----------------------------------------------------------------private-----
            static auto
date        () -> date_t
            {
            date_t d;
            d.w = read_verify(RTCDATE);
            return d;
            }

//-----------------------------------------------------------------private-----
            static auto
alarm_time  () -> time_t
            {
            time_t t;
            t.w = Reg::val(ALMTIME);
            return t;
            }

//-----------------------------------------------------------------private-----
            static auto
alarm_date  () -> date_t
            {
            date_t d;
            d.w = Reg::val(ALMDATE);
            return d;
            }

//RTCCON.ON, RTCCON2, DATE/TIME registers need RTCWREN=1
//functions cannot call other functions that use unlock/lock inside its
//unlock/lock code (will end up putting the caller back in lock mode)
//-----------------------------------------------------------------private-----
            static auto
unlock      () -> void
            {
            Sys::unlock();
            Reg::setbit(RTCCON, 1<<RTCWREN);
            }

//-----------------------------------------------------------------private-----
            static auto
lock        () -> void
            {
            Reg::clrbit(RTCCON, 1<<RTCWREN);
            Sys::lock();
            }

//calc weekday from date
//(date_t) month 1-12, day 1-31, year 0-99
//return weekday, 0 (sunday) - 6(saturday)
//-----------------------------------------------------------------private-----
            static auto
calc_weekday (date_t v) -> uint8_t
            {
            uint8_t m = v.month10*10 + v.month1;
            uint8_t d = v.day10*10 + v.day1;
            uint16_t y = 2000 + v.year10*10 + v.year1;

            static uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
            y -= m < 3;
            return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
            }

//wrlock
//-----------------------------------------------------------------private-----
            static auto
time        (time_t v) -> void
            {
            unlock();
            write_verify(RTCTIME, v.w);
            lock();
            }

//wrlock
//-----------------------------------------------------------------private-----
            static auto
date        (date_t v) -> void
            {
            v.weekday = calc_weekday(v); //get correct weekday
            unlock();
            write_verify(RTCDATE, v.w);
            lock();
            }

//-----------------------------------------------------------------private-----
            static auto
alarm_time  (time_t v) -> void
            {
            bool b = Rtcc::alarm();
            Rtcc::alarm(false);
            while(Rtcc::alarm_busy());
            Reg::val(ALMTIME, v.w);
            Rtcc::alarm(b);
            }

//-----------------------------------------------------------------private-----
            static auto
alarm_date  (date_t v) -> void
            {
            v.weekday = calc_weekday(v); //get correct weekday
            bool b = Rtcc::alarm();
            Rtcc::alarm(false);
            while(Rtcc::alarm_busy());
            Reg::val(ALMDATE, v.w);
            Rtcc::alarm(b);
            }

//-----------------------------------------------------------------private-----
            static auto
dt_to_date (Rtcc::datetime_t v) -> date_t
            {
            date_t d;
            d.year10 = v.year/10; d.year1 = v.year%10;
            d.month10 = v.month/10; d.month1 = v.month%10;
            d.day10 = v.day/10; d.day1 = v.day %10;
            return d;
            }

//-----------------------------------------------------------------private-----
            static auto
dt_to_time  (Rtcc::datetime_t v) -> time_t
            {
            time_t t;
            t.hours10 = v.hour/10; t.hours1 = v.hour%10;
            t.minutes10 = v.minute/10; t.minutes1 = v.minute%10;
            t.seconds10 = v.second/10; t.seconds1 = v.second%10;
            return t;
            }

//-----------------------------------------------------------------private-----
            static auto
dt_to_dt    (date_t d, time_t t) -> Rtcc::datetime_t
            {
            Rtcc::datetime_t dt;
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

//=============================================================================
            auto Rtcc::
alarm       (bool tf) -> void
            {
            setbit(RTCALRM, 1<<ALRMEN, tf);
            }

//=============================================================================
            auto Rtcc::
alarm       () -> bool
            {
            return anybit(RTCALRM, 1<<ALRMEN);
            }

//=============================================================================
            auto Rtcc::
chime       (bool tf) -> void
            {
            setbit(RTCALRM, 1<<CHIME, tf);
            }

//=============================================================================
            auto Rtcc::
alarm_interval (AMASK e) -> void
            {
            clrbit(RTCALRM, AMASK_MASK<<AMASK_SHIFT);
            setbit(RTCALRM, e<<AMASK_SHIFT);
            }

//=============================================================================
            auto Rtcc::
alarm_repeat (uint8_t v) -> void
            {
            clrbit(RTCALRM, ALMRPT_MASK<<ALMRPT_SHIFT);
            setbit(RTCALRM, v<<ALMRPT_SHIFT);
            }

//=============================================================================
            auto Rtcc::
on          (bool tf) -> void
            {
            //datasheet shows DIV=0 on reset,
            //rtcc frm shows DIV=0x3FFF- frm is correct
            //reset div/pre values assume sosc, so no need to set
            //if using LPRC, just use Rtcc::clk_src(Rtcc::LPRC) first
            //if using PWRLPIN/FCY, will also habe to use clk_div/clk_pre
            unlock();
            setbit(RTCCON, 1<<ON, tf);
			setbit(RTCCON, 1<<RTCCLKON, tf);
            lock();
            //if boot time not already recorded, store it
            //assumes rtcc will be turned on early in startup and datetime
            //already set (or reset was not due to power loss)
            if(not m_boot_time.year) m_boot_time = datetime();
            //if rtcc was reset, and no datetime set, boot_time will still be clear
            }

//=============================================================================
            auto Rtcc::
out_pin     (OUTSEL v) -> void
            {
            if(v != OFF){
                setbit(RTCCON, 1<<RTSECSEL, v == CLKSEC);
            }
            setbit(RTCCON, 1<<PINON, v != OFF);
            }

//=============================================================================
            auto Rtcc::
alarm_evt   () -> bool
            {
            return !anybit(RTCALRM, 1<<ALRMEN);
            }

//=============================================================================
            auto Rtcc::
time_busy   () -> bool
            {
            return anybit(RTCCON, 1<<SYNCSTAT);
            }

//=============================================================================
            auto Rtcc::
alarm_busy  () -> bool
            {
            return anybit(RTCALRM, 1<<ALRMSYNCSTAT);
            }

//=============================================================================
            auto Rtcc::
half_sec    () -> bool
            {
            return anybit(RTCCON, 1<<HALFSTAT);
            }

//=============================================================================
            auto Rtcc::
datetime    () -> datetime_t
            {
            time_t t = time();
            date_t d = date();
            datetime_t dt = dt_to_dt(d, t);
            return dt;
            }

//=============================================================================
            auto Rtcc::
alarm_datetime () -> datetime_t
            {
            time_t t = alarm_time();
            date_t d = alarm_date();
            datetime_t dt = dt_to_dt(d, t);
            return dt;
            }

//=============================================================================
            auto Rtcc::
datetime    (datetime_t v) -> void
            {
            date(dt_to_date(v));
            time(dt_to_time(v));
            }

//=============================================================================
            auto Rtcc::
alarm_datetime (datetime_t v) -> void
            {
            alarm_date(dt_to_date(v));
            alarm_time(dt_to_time(v));
            }

//=============================================================================
            auto Rtcc::
boot_datetime () -> datetime_t
            {
            return m_boot_time;
            }

