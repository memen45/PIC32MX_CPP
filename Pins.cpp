#include "Pins.hpp"

void Pins::lowison(void){         m_lowison = true; }
void Pins::lowisoff(void){        m_lowison = false; }
void Pins::digital(void){         *(m_pt+1) = m_pn; }
void Pins::analog(void){          *(m_pt+2) = m_pn; }
void Pins::output(void){          *(m_pt+5) = m_pn; }
void Pins::input(void){           *(m_pt+6) = m_pn; }

void Pins::odc_off(void){         *(m_pt+17) = m_pn; }
void Pins::odc_on(void){          *(m_pt+18) = m_pn; }
void Pins::pullup_off(void){      *(m_pt+21) = m_pn; }
void Pins::pullup_on(void){       *(m_pt+22) = m_pn; }
void Pins::pulldn_off(void){      *(m_pt+25) = m_pn; }
void Pins::pulldn_on(void){       *(m_pt+26) = m_pn; }

void Pins::icnoff(){              *(m_pt+29) = 1<<15; }
void Pins::icnon(){               *(m_pt+30) = 1<<15; }
void Pins::icnmatch(){            *(m_pt+29) = 1<<11; }
void Pins::icnedge(){             *(m_pt+30) = 1<<11; }
void Pins::icnrisingoff(){        *(m_pt+33) = m_pn; }
void Pins::icnrising(){           icnedge();*(m_pt+34) = m_pn; }
void Pins::icnfallingoff(){       *(m_pt+41) = m_pn; }
void Pins::icnfalling(){          icnedge();*(m_pt+42) = m_pn; }
void Pins::icnmismatch(){         icnfalling(); icnmatch(); }
bool Pins::icnflag(){             return *(m_pt+44) & m_pn; }
bool Pins::icnstat(){             return *(m_pt+36) & m_pn; }

