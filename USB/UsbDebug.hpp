#pragma once
#include <cstdio>  //printf
#include <cstdbool>
#include "Uart.hpp"

struct UsbDebug  {

    UsbDebug(){}        //empty constructor
    UsbDebug(Uart*);    //or specify a uart device

    //send out info via printf
    //(filename, funcname, format, args...)

    static
    void    debug       (const char*, const char*, const char*, ... );

    static
    void    debug       (bool);     //enable/disable

    static
    bool    debug       (void);         //get enable/disable

    static
    void    debug       (Uart*);    //set which uart device

};

