#pragma once

//Register (SFR) R/W
//kseg1/0 address <-> physical address


#include <cstdint>

/*                                             |-kseg = 1, phys = 0
SFR - kseg0 0x9F800000-0x9F80FFFF bit<31:29> 0b100<-kseg0 = 0
      kseg1 0xBF800000-0xBF80FFFF bit<31:29> 0b101<-kseg1 = 1
      phys  0x1F800000-0x1F80FFFF bit<31:29> 0b000
      access sfr's via kseg1 addresses


     retval     name     args- address(32bit), bitmask, (bool- set=1, clear=0)
  -----------------------------------------------------------------------------
                setbit  (address, bitmask, bool)        -set or clr bit(s)
                setbit  (address, bitmask)              -set bit(s)
                clrbit  (address, bitmask)              -clr bit(s)
                flipbit (address, bitmask)              -invert bit(s)
                anybit  (address, bitmask)              -any bit(s) set?
                allbit  (address, bitmask)              -all bit(s) set?
                anybit0 (address, bitmask)              -any bit(s) clr?
                allbit0 (address, bitmask)              -all bit(s) clr?
    uint32_t    val     (address)                       -read 32bit value
    value16     val16   (address)                       -read 16bit value
    value8      val8    (address)                       -read 8bit value
                val     (address, value8|16|32)         -set 8|16|32bit value
    uint32_t    p2kseg1 (address)                       -phys addr->kseg1
    uint32_t    p2kseg0 (address)                       -phys addr->kseg0
    uint32_t    k2phys  (address)                       -ksegx addr->phys



    address arguments are mostly enums, but can also be uint32_t or pointer
    type, and value arguments are also mostly enums, but can be 8/16/32bit
    values also, the templates work to get the underlying size of the value,
    then set the address argument to a volatile pointer of that type, allowing
    byte/half-word/word access to a register-

    val(T r, V v) -> *(volatile V*)r = v -> type deduced by type passed in

    address- caller makes sure is 32bit number, also be aware of addition on
    the address, if enum- additions are byte, if pointer- addition is word

    value- type will determine address type of pointer, if value needs to be
    something other than its type (size), it will need to be cast-
    val(ENUM_REG+3, ENUM_VAL) - if ENUM_VAL is 32bit, need to cast to 8bit
    in this case in order to access third byte of ENUM_REG
    (unaligned access will cause exception :) )

*/

//some typdefs for general use
using vu32ptr = volatile uint32_t*;
using vu16ptr = volatile uint16_t*;
using vu8ptr = volatile uint8_t*;


//helper templates- get size in bytes of V type
//also provide atomic register offsets
template<int> struct Vsiz {
    using type = void;
};
template <> struct Vsiz<1> {
    using type = uint8_t;
    static const uint8_t CLR = 4;
    static const uint8_t SET = 8;
    static const uint8_t INV = 12;
};
template <> struct Vsiz<2> {
    using type = uint16_t;
    static const uint8_t CLR = 2;
    static const uint8_t SET = 4;
    static const uint8_t INV = 6;
};
template <> struct Vsiz<4> {
    using type = uint32_t;
    static const uint8_t CLR = 1;
    static const uint8_t SET = 2;
    static const uint8_t INV = 3;
};
template<typename T> struct getVsiz {
    using type = typename Vsiz<sizeof(T)>::type;
};


struct Reg {

    //T = address, V = value (V will become uint8/16/32_t)
    //T will become volatile V*, so can access register by
    //byte, half-word, or word, determined by size of V

    //set bitmask bit(s) in register to specified level 0/1
    //set bitmask bit(s) in register to 1
    //clear bitmask bit(s) in register to 0
    template <typename T, typename V> static void setbit(T, V, bool);
    template <typename T, typename V> static void setbit(T, V);
    template <typename T, typename V> static void clrbit(T, V);

    //flip bitmask bit(s) in register
    template <typename T, typename V> static void flipbit (T, V);

    //test if any bitmask bit(s) in the register are set/clear
    template <typename T, typename V> static bool anybit(T, V);
    template <typename T, typename V> static bool anybit0(T, V);

    //test if all bitmask bit(s) in the register are set/clear
    template <typename T, typename V> static bool allbit(T, V);
    template <typename T, typename V> static bool allbit0(T, V);

    //return val from register- val = 32bit, val16 = 16bit, val8 = 8bit
    template <typename T> static uint32_t val(T);
    template <typename T> static uint16_t val16(T);
    template <typename T> static uint8_t  val8(T);

    //val(address, val)-> *(V*)address = (V)val
    template <typename T, typename V> static void val(T r, V v);

    //physical to kseg0/1 addr, kseg to physical addr
    template <typename T> static uint32_t   p2kseg1 (T);
    template <typename T> static uint32_t   p2kseg0 (T);
    template <typename T> static uint32_t   k2phys  (T);

};

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//  functions inline

//setbit, specify 1/0
template <typename T, typename V> void Reg::setbit(T r, V v, bool sc){
    if(sc) setbit(r,v); else clrbit(r,v);
}

//setbit
template <typename T, typename V> void Reg::setbit(T r, V v){
    using vtype = typename getVsiz<V>::type;
    //((volatile vtype*)r)[Vsiz<sizeof(V)>::SET] = v;
    *((volatile vtype*)r+Vsiz<sizeof(V)>::SET) = v;
}

//clrbit
template <typename T, typename V> void Reg::clrbit(T r, V v){
    using vtype = typename getVsiz<V>::type;
    *((volatile vtype*)r+Vsiz<sizeof(V)>::CLR) = v;
}

//flipbit
template <typename T, typename V> void Reg::flipbit(T r, V v){
    using vtype = typename getVsiz<V>::type;
    *((volatile vtype*)r+Vsiz<sizeof(V)>::INV) = v;
}

//anybit
template <typename T, typename V> bool Reg::anybit(T r, V v){
    using vtype = typename getVsiz<V>::type;
    return *(volatile vtype*)r bitand v;
}

//anybit0
template <typename T, typename V> bool Reg::anybit0(T r, V v){
    using vtype = typename getVsiz<V>::type;
    return (compl *(volatile vtype*)r) bitand v;
}

//allbit
template <typename T, typename V> bool Reg::allbit(T r, V v){
    using vtype = typename getVsiz<V>::type;
    return *(volatile vtype*)r bitand v == v;
}

//allbit0
template <typename T, typename V> bool Reg::allbit0(T r, V v){
    using vtype = typename getVsiz<V>::type;
    return (compl *(volatile vtype*)r) bitand v == v;
}


//return uint32_t value of register r
template <typename T> uint32_t Reg::val(T r){
     return *(volatile uint32_t*)r;
 }
 //return uint16_t value of register r
 template <typename T> uint16_t Reg::val16(T r){
     return *(volatile uint16_t*)r;
 }
 //return uint8_t value of register r
 template <typename T> uint8_t Reg::val8(T r){
     return *(volatile uint8_t*)r;
 }


//set uint32_t/uint16_t/uint8_t value to register r
//(determined by size of V type)
template <typename T, typename V> void Reg::val(T r, V v){
    using vtype = typename getVsiz<V>::type;
    *(volatile vtype*)r = v;
}


//physical to kseg0/1 addr, kseg to physical addr
template <typename T> uint32_t Reg::p2kseg1(T r){
    return (uint32_t)r bitor 0xA0000000; //A0=0b1010_0000
}
template <typename T> uint32_t Reg::p2kseg0(T r){
    return (uint32_t)r bitor 0x80000000; //80=0b1000_0000
}
template <typename T> uint32_t Reg::k2phys(T r){
    return (uint32_t)r bitand 0x1FFFFFFF; //1F=0b0001_1111
}
