#ifndef _PINS_H
#define _PINS_H

#include <cstdint>

class Pins {

    public:
        enum PORT : uint32_t {
            A = 0xBF802BB0, //make ANSELA base addr
            B = 0xBF802CB0, //make ANSELB base addr
            C = 0xBF802DB0, //make ANSELC base addr
            D = 0xBF802EB0, //make ANSELD base addr
        };

        //inline
        constexpr Pins( PORT pt, const uint8_t pn ) :
             m_pt( ( volatile uint32_t*)pt ),
             m_pn( 1<<pn ),
             m_lowison()
        {
        }

        //inline
        bool pinval(void){          return *(m_pt+8) & m_pn; }
        bool latval(void){          return *(m_pt+12) & m_pn; }
        void low(void){             *(m_pt+13) = m_pn; }
        void high(void){            *(m_pt+14) = m_pn; }
        void invert(void){          *(m_pt+15) = m_pn; }
        void on(void){              m_lowison ? low() : high(); }
        void off(void){             m_lowison ? high() : low(); }
        bool ison(void){            return m_lowison ? !pinval() : pinval(); }
        bool isoff(void){           return !ison(); }

        //cpp
        void lowison(void);
        void lowisoff(void);
        void digital(void);
        void analog(void);
        void output(void);
        void input(void);
        void odc_off(void);
        void odc_on(void);
        void pullup_off(void);
        void pullup_on(void);
        void pulldn_off(void);
        void pulldn_on(void);

        //input change notification
        //cpp
        void icnoff();
        void icnon();
        void icnmatch();
        void icnedge();
        void icnrisingoff();
        void icnrising();
        void icnfallingoff();
        void icnfalling();
        void icnmismatch();
        bool icnflag();
        bool icnstat();
        //inline
        //void icnflagclear(){        Sfr::clrbit( m_pt+44, m_pn ); }
        void icnflagclear(){        *( m_pt+45) = m_pn; }
    private:
        const uint16_t m_pn;              //pin mask
        volatile uint32_t* m_pt;    //base address
        bool m_lowison;             //pin on val is low
};


#endif // _H
