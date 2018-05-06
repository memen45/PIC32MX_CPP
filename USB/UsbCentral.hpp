#pragma once

//USB Descriptors

#include "UsbCh9.hpp"
#include "UsbEP.hpp"
#include <cstdint>

struct UsbCentral {

    //function call to service irq for non-endpoint0 (current device)
    using service_t = bool(*)(uint8_t, UsbEP0*);

    static bool             init            (bool);

    //set device before any usb init- called from Usb<device>.cpp
    //const uint8_t*=descriptor array
    //service_t=device service function
    //also calls Usb::init(true) if both args are not 0
    //else calls Usb::init(false),
    //returns result of init
    static bool             set_device      (const uint8_t*, service_t);

    //wValue=descriptor type,index (directly from setup packet)
    //siz = requested size, change to actual size in function if smaller
    //return pointer to wanted bytes
    static const uint8_t*   get_desc        (uint16_t wValue, uint16_t* siz);

    //set configuration (return pointer to config or 0 if bad index)
    static const uint8_t*   set_config      (uint16_t wValue);

    //get ep size from descriptor/config
    static uint16_t         get_epsiz       (uint8_t n, bool tr);

    //get endpoint control info based on descriptor info
    static uint8_t          get_epctrl      (uint8_t n);

    //self-powered, remote wakeup
    static uint8_t          get_status      ();

    //store current descriptor config number
    static uint8_t          current_config;

    //service- called from isr
    static void             service         (uint32_t, uint8_t);
    //service- call registered service (m_service)
    static bool             service         (uint8_t ustat, UsbEP0* = 0);

    //get 1ms timer count, sof count
    static uint32_t         timer1ms        ();
    static uint32_t         sofcount        ();


    private:

    //specific device info
    static const uint8_t*   m_descriptor;
    static service_t        m_service;

    //counters
    static uint32_t         m_timer1ms;
    static uint32_t         m_sofcount;

};
