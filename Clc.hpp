#pragma once

//CLC1-4

#include <cstdint>

struct Clc {

            enum
CLCX        { CLC1, CLC2, CLC3, CLC4 };

            //instantiate Clc with CLCn
Clc         (CLCX);

            enum
GXPOL       { G4 = 1<<19, G3 = 1<<18, G2 = 1<<17, G1 = 1<<16 };

            auto
gate_inv    (GXPOL, bool) -> void;

            auto
on          (bool) -> void;

            auto
stop_idle   (bool) -> void;

            auto
intp        (bool) -> void;

            auto
intn        (bool) -> void;

            auto
out         (bool) -> void;

            auto
out         () -> bool;

            auto
out_inv     (bool) -> void;

            enum
MODE        : uint8_t { ANDOR, ORXOR, AND, SR, DSR, DR, JKR, LSR };

            auto
mode        (MODE e) -> void;

            enum
SELX        { DS1, DS2, DS3, DS4 };

            auto
in_sel      (SELX, uint8_t) -> void;

            auto
in_sel      (uint32_t) -> void;

            enum
GLSX        { GS1, GS2, GS3, GS4 };

            auto
gate_sel    (GLSX, uint8_t) -> void;

            auto
gate_sel    (uint32_t) -> void;


            private:

            volatile uint32_t* m_clcx_con;

};
