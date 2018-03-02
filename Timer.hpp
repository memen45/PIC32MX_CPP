#pragma once

//Timer2/Timer3

#include <cstdint>

class Timer {

    public:

    //instantiate with timer number
    enum TMRX { TMR2 = 0, TMR3, TMR4, TMR5 };
    Timer(TMRX);

    void        count       (uint32_t) const;
    uint32_t    count       () const;
    void        period      (uint32_t) const;
    uint32_t    period      () const;
    void        on          (bool) const;
    void        stop_idle   (bool) const;
    void        gate        (bool) const;

    enum PRESCALE { PS1 = 0, PS2, PS4, PS8, PS16, PS32, PS64, PS256 };
    void        prescale    (PRESCALE) const;
    PRESCALE    prescale    ();

    void        mode32      (bool) const; //T2 and T4 only (harmless for T3 and T5)

    enum CLK { PBCLK = 0, TxCK };
    void        clk_src     (CLK) const;

    private:

    volatile uint32_t* m_txcon;
    volatile uint32_t& m_tmrx;
    volatile uint32_t& m_prx;

};
