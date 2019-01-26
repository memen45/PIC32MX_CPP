#pragma once
#include <cstdio>  //printf
#include <cstdbool>

struct UsbDebug {

    static
    char*   getbuf      (int*);    //get a buffer to use

    static
    void    debug       (char*);    //send out info via printf

    static
    void    debug       (bool);     //enable/disable

    static
    bool    debug       ();         //get enable/disable
};
