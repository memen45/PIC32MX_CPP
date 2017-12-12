#pragma once

class Syskey {

    public:

    //public functions
    static void     lock    ();
    static void     unlock  ();

    private:

    enum { SYSKEY_ADDR = 0xBF803670, DMACON = 0xBF808900, DMASUSP = 1<<12 };

};