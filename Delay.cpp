#include "Delay.hpp"

/*=============================================================================
 DelayCP0 functions
=============================================================================*/
bool DelayCP0::expired(){
    if((cp0.count() - m_start) >= m_countn) m_expired = true;
    return m_expired;
}
void DelayCP0::restart(){
    m_start = cp0.count();
    m_expired = false;
}
void DelayCP0::wait_us(uint32_t n){
    if(n > 300000000) n = 300000000;
    set_count(n);
    while(! expired());
}
void DelayCP0::wait_ms(uint32_t n){
    if(n > 300000) n = 300000;
    wait_us(1000 * n);
}
void DelayCP0::wait_s(uint16_t n){
    if(n > 300) n = 300;
    wait_ms(1000 * n);
}
void DelayCP0::set_us(uint32_t n){
    if(n > 300000000) n = 300000000;
    set_count(n);
}
void DelayCP0::set_ms(uint32_t n){
    if(n > 300000) n = 300000;
    set_us(1000 * n);
}
void DelayCP0::set_s(uint16_t n){
    if(n > 300) n = 300;
    set_ms(1000 * n);
}

//private
void DelayCP0::set_count(uint32_t n){
    restart();
    m_countn = osc.sysclk() / 2000000 * n;
}


