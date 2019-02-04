#pragma once

#include <cstdint>
#include <cstdbool>
#include <cstring> //strlen


struct Markup {

    protected:

Markup      () //prevent direct use
            {
            }

            auto
markup      (char* str) -> bool;


            //strip markup from string (modify string in place)
            auto
strip       (char* src) -> bool;

    private:

            bool m_ison{true};

            static const char m_trigger{'@'};
            static const char m_turnon{'+'};
            static const char m_turnoff{'-'};

            //ansi forecolor
            static constexpr const char* m_blacK   = "K\033[38;5;0m";
            static constexpr const char* m_Red     = "R\033[38;5;196m";
            static constexpr const char* m_Green   = "G\033[38;5;48m";
            static constexpr const char* m_Yellow  = "Y\033[38;5;226m";
            static constexpr const char* m_Blue    = "B\033[38;5;26m";
            static constexpr const char* m_Magenta = "M\033[38;5;163m";
            static constexpr const char* m_Cyan    = "C\033[38;5;6m";
            static constexpr const char* m_White   = "W\033[38;5;15m";
            //ansi backcolor
            static constexpr const char* m_black   = "k\033[48;5;0m";
            static constexpr const char* m_red     = "r\033[48;5;196m";
            static constexpr const char* m_green   = "g\033[48;5;48m";
            static constexpr const char* m_yellow  = "y\033[48;5;226m";
            static constexpr const char* m_blue    = "b\033[48;5;26m";
            static constexpr const char* m_magenta = "m\033[48;5;163m";
            static constexpr const char* m_cyan    = "c\033[48;5;6m";
            static constexpr const char* m_white   = "w\033[48;5;15m";
            //reset colors/attributes
            static constexpr const char* m_reset   = "!\033[0;38;5;15m\033[48;5;0m";

            using
codes_t     = struct
            {
            const char* ansistr;
            const uint8_t siz;
            };

            //store all markup char+ansi codes and size of string-1
            static const codes_t m_codes[];

};
