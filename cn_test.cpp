#include <cstdint>
#include "Pins.hpp"
#include "Osc.hpp"
#include "Irq.hpp"

struct Leds {
    void update(){
        for(auto& i : m_rgb) i.off();
        m_rgb[m_state++].on();
        if(m_state == 3) m_state = 0;
    }
    private:
    uint8_t m_state{0};
    Pins m_rgb[3]{
        {Pins::D1, Pins::OUT},
        {Pins::C3, Pins::OUT},
        {Pins::C15, Pins::OUT}
    };
};
Leds leds;
Pins sw3{ Pins::C4, Pins::INPU };

int main()
{
    //set osc to 24MHz
    Osc osc;
    osc.pll_set(osc.MUL12, osc.DIV4);
    //set sw3 to icn falling, and enable
    sw3.icn_falling();
    sw3.icn( true );
    //enable cn irq, and global
    Irq::init( Irq::CHANGE_NOTICE_C, 1, 0, true );
    Irq::global( true );
    //let irq do its work
    for(;;){ Osc::idle(); }
}

//pin C4 (sw3)
ISR( CHANGE_NOTICE_C ){
    Irq irq;
    irq.flag_clr( irq.CHANGE_NOTICE_C );
    sw3.icn_flagclr(); //<--need to clear this flag also
    leds.update();
}
