#pragma once
#include <cstdio>  //printf
#include <cstdbool>

struct UsbDebug {

    //send out info via printf
    //(filename, funcname, format, args...)

    static
    void    debug       (const char*, const char*, const char*, ... );

    static
    void    debug       (bool);     //enable/disable

    static
    bool    debug       ();         //get enable/disable

    //buffer size, made available for snprintf use
    static
    const int bufsiz{81};

    //buffer for snprintf use
    static
    char   buffer[bufsiz];


};

