#pragma once

//USB Descriptors

#include "UsbCh9.hpp"
#include "UsbEP.hpp"
#include <cstdint>

struct UsbCentral {

    //function call to service irq for non-endpoint0 (current device)
    using service_t = bool(*)(uint8_t, UsbEP0*);

        static auto
    init (bool) -> bool;

    //set device before any usb init- called from Usb<device>.cpp
    //const uint8_t*=descriptor array
    //service_t=device service function
    //also calls Usb::init(true) if both args are not 0
    //else calls Usb::init(false),
    //returns result of init
        static auto
    set_device (const uint8_t*, service_t) -> bool;

    //wValue=descriptor type,index (directly from setup packet)
    //siz = requested size, change to actual size in function if smaller
    //return pointer to wanted bytes
        static auto
    get_desc (uint16_t wValue, uint16_t* siz) -> const uint8_t*;

    //set configuration (return pointer to config or 0 if bad index)
        static auto
    set_config (uint16_t wValue) -> const uint8_t*;

    //get ep size from descriptor/config
        static auto
    get_epsiz (uint8_t n, bool tr) -> uint16_t;

    //get endpoint control info based on descriptor info
        static auto
    get_epctrl (uint8_t n) -> uint8_t;

    //self-powered, remote wakeup
        static auto
    get_status () -> uint8_t;

    //store current descriptor config number
    static uint8_t
    current_config;

    //service- called from isr
        static auto
    service (uint32_t, uint8_t) -> void;

    //service- call registered service (m_service)
        static auto
    service (uint8_t ustat, UsbEP0* = 0) -> bool;

    //get 1ms timer count, sof count
        static auto
    timer1ms () -> uint32_t;

        static auto
    sofcount () -> uint32_t;

};
