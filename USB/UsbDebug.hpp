#pragma once
#include <cstdio>  //printf
#include <cstdbool>

struct UsbDebug {

    //send out info via printf
    //(filename, funcname, message)
    static
    void    debug       (const char*, const char*);

    static
    void    debug       (bool);     //enable/disable

    static
    bool    debug       ();         //get enable/disable

    static
    const int bufsiz{81};

    static
    char   buffer[bufsiz];


};

