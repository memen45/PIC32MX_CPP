#pragma once

//USB Descriptors

#include "UsbCh9.hpp"
#include <cstdint>

struct UsbDescriptors {

    //function call to service irq for non-endpoint0 (current device)
    using service_t = bool(*)(uint32_t, uint8_t);

    //set descriptor before any usb init- called from Usb<device>.cpp
    //d=descriptor array, len=descriptor length
    //also calls Usb::init(true) if both args are not 0
    //else calls Usb::init(false), return result of init
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


    static const uint8_t    last_ep_num = 2; //TODO get rid of (temp use)

    static uint8_t          current_config;

    static bool             service         (uint32_t, uint8_t);

    private:

    //specific device info
    static const uint8_t*   m_descriptor;
    static service_t        m_service;

};
