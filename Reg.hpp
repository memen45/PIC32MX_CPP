#pragma once

/*=============================================================================
 Register (SFR) writes/read
=============================================================================*/

#include <cstdint>

class Reg {

    public:

    //public functions
    template <typename T> static void set(T, uint32_t);
    template <typename T> static uint32_t set(T, uint32_t, bool);
    template <typename T> static void clr(T, uint32_t);
    template <typename T> static void inv(T, uint32_t);
    template <typename T> static bool is_set(T, uint32_t);
    template <typename T> static bool is_clr(T, uint32_t);
    template <typename T> static uint32_t val(T);
    template <typename T> static void val(T, uint32_t);
    template <typename T> static void val16(T, uint16_t);
    template <typename T> static uint16_t val16(T);
    template <typename T> static void val8(T, uint8_t);
    template <typename T> static uint8_t val8(T);

    private:

    enum { CLR = 1, SET = 2, INV = 3 };

};

/*=============================================================================
 all functions inline
=============================================================================*/
//all register values cast to volatile uint32_t*
//so can use enum values as register argument
//SET/CLR/INV offsets will be calculated in words (4 bytes)
template <typename T> void Reg::set(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+SET) = v;
}
//same name as set, but 3 args, last is true/false = set/clr
template <typename T> uint32_t Reg::set(T r, uint32_t v, bool sc){
    *(reinterpret_cast <volatile uint32_t*>(r)+CLR+sc) = v;
}
template <typename T> void Reg::clr(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+CLR) = v;
}
template <typename T> void Reg::inv(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+INV) = v;
}
template <typename T> bool Reg::is_set(T r, uint32_t v){
    return *(reinterpret_cast <volatile uint32_t*>(r)) & v;
}
template <typename T> bool Reg::is_clr(T r, uint32_t v){
    return ! *(reinterpret_cast <volatile uint32_t*>(r)) & v;
}
template <typename T> uint32_t Reg::val(T r){
    return *(reinterpret_cast <volatile uint32_t*>(r));
}
template <typename T> void Reg::val(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)) = v;
}
template <typename T> void Reg::val16(T r, uint16_t v){
    *(reinterpret_cast <volatile uint16_t*>(r)) = v;
}
template <typename T> uint16_t Reg::val16(T r){
    return *(reinterpret_cast <volatile uint16_t*>(r));
}
template <typename T> void Reg::val8(T r, uint8_t v){
    *(reinterpret_cast <volatile uint8_t*>(r)) = v;
}
template <typename T> uint8_t Reg::val8(T r){
    return *(reinterpret_cast <volatile uint8_t*>(r));
}