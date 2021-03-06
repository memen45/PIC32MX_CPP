#pragma once

//High/Low-Voltage Detect

#include <cstdint>
#include "Reg.hpp"

struct Vdetect : private Reg {

            static auto
on          (bool) -> void;

            static auto
stop_idle   (bool) -> void;

            static auto
trip_above  (bool) -> void;

            static auto
bgap_stable () -> bool;

            static auto
iref_stable () -> bool;

            static auto
tripped     () -> bool;

            enum
HLVDL       {
            V325_363 = 5,
            V295_330,
            V275_309,
            V265_298,
            V245_280,
            V235_269,
            V225_255,
            V215_244,
            V208_233,
            V200_222,
            VEXT
            };

            //set voltage level limit
            static auto
limit       (HLVDL) -> void;

};
