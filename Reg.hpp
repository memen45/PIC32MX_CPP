#pragma once

/*=============================================================================
 Register (SFR) writes/read
=============================================================================*/

#include <cstdint>

struct Reg {

    //public functions
    template <typename T> static void       set     (T, uint32_t);
    template <typename T> static void       set     (T, uint32_t, bool);
    template <typename T> static void       clr     (T, uint32_t);
    template <typename T> static void       inv     (T, uint32_t);
    template <typename T> static bool       is_set  (T, uint32_t);
    template <typename T> static bool       is_clr  (T, uint32_t);
    template <typename T> static bool       is_set8 (T, uint8_t);//usb regs use
    template <typename T> static bool       is_clr8 (T, uint8_t);//only 8bits
    template <typename T> static uint32_t   val     (T);
    template <typename T> static void       val     (T, uint32_t);
    template <typename T> static void       val16   (T, uint16_t);
    template <typename T> static uint16_t   val16   (T);
    template <typename T> static void       val8    (T, uint8_t);
    template <typename T> static uint8_t    val8    (T);

    //physical to kseg0/1 addr, kseg to physical addr
    template <typename T> static uint32_t   p2kseg1 (T); //unused in PIC32MM
    template <typename T> static uint32_t   p2kseg0 (T);
    template <typename T> static uint32_t   k2phys  (T);

    private:

    enum { CLR = 1, SET, INV };

};

/*=============================================================================
 all functions inline
=============================================================================*/
//reg values cast to volatile uint32_t*,uint16_t* or uint8_t*
//specified in function name- no number=32bit, val16=16bit, set8=8bit

//templates not used in v values so specific functions can be used by name
//instead of v type (prevent wrong type passed, then using wrong function)-
//if I want v treated as an 8bit value, I can specify by function name
//rather than having to typecast in call or specify template parameter

//templates used so can use enum values (or uint32_t) as register argument
//SET/CLR/INV offsets will be calculated in words (4 bytes)

//32bit
template <typename T> void Reg::set(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+SET) = v;
}
//same name as set, but 3 args, last is true/false = set/clr
template <typename T> void Reg::set(T r, uint32_t v, bool sc){
    *(reinterpret_cast <volatile uint32_t*>(r)+CLR+sc) = v;
}
template <typename T> void Reg::clr(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+CLR) = v;
}
template <typename T> void Reg::inv(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)+INV) = v;
}
//is_set/is_clr can check multiple bits
template <typename T> bool Reg::is_set(T r, uint32_t v){
    return (*(reinterpret_cast <volatile uint32_t*>(r)) & v) == v;
}
template <typename T> bool Reg::is_clr(T r, uint32_t v){
    return (*(reinterpret_cast <volatile uint32_t*>(r)) | ~v) == ~v;
}
template <typename T> uint32_t Reg::val(T r){
    return *(reinterpret_cast <volatile uint32_t*>(r));
}
template <typename T> void Reg::val(T r, uint32_t v){
    *(reinterpret_cast <volatile uint32_t*>(r)) = v;
}


//16bit
template <typename T> void Reg::val16(T r, uint16_t v){
    *(reinterpret_cast <volatile uint16_t*>(r)) = v;
}
template <typename T> uint16_t Reg::val16(T r){
    return *(reinterpret_cast <volatile uint16_t*>(r));
}


//8bit
template <typename T> bool Reg::is_set8(T r, uint8_t v){
    return (*(reinterpret_cast <volatile uint8_t*>(r)) & v) == v;
}
template <typename T> bool Reg::is_clr8(T r, uint8_t v){
    return ! (*(reinterpret_cast <volatile uint8_t*>(r)) | v) == v ;
}
template <typename T> void Reg::val8(T r, uint8_t v){
    *(reinterpret_cast <volatile uint8_t*>(r)) = v;
}
template <typename T> uint8_t Reg::val8(T r){
    return *(reinterpret_cast <volatile uint8_t*>(r));
}



//physical to kseg0/1 addr, kseg to physical addr
//(kseg1 not used in PIC32MM- no cache)
template <typename T> uint32_t Reg::p2kseg1(T r){
    return (reinterpret_cast <uint32_t>(r)) | 0xA0000000; //A0=0b1010_0000
}
template <typename T> uint32_t Reg::p2kseg0(T r){
    return (reinterpret_cast <uint32_t>(r) | 0x80000000); //80=0b1000_0000
}
template <typename T> uint32_t Reg::k2phys(T r){
    return (reinterpret_cast <uint32_t>(r)) & 0x1FFFFFFF; //1F=0b0001_1111
}