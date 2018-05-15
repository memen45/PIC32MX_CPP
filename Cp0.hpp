#pragma once

//Coprocessor0  Count, Compare

#include <cstdint>

struct Cp0 {

        static auto
    count () -> uint32_t;

        static auto
    compare () -> uint32_t;

        static auto
    count (uint32_t) -> void;

        static auto
    compare (uint32_t) -> void;

        static auto
    compare_reload () -> void;

        static auto
    compare_irq (bool) -> void;

        static auto
    compare_us (uint32_t) -> void;

        static auto
    compare_ms (uint16_t) -> void;

};
