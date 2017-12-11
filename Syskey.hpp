#pragma once

class Syskey {

    public:

    //public functions
    static void lock( void );
    static void unlock( void );

    private:

    enum {
        SYSKEY_ADDR = 0xBF803670,
        DMACON = 0xBF808900, DMA_SUSPEND = 1<<12
    };

};