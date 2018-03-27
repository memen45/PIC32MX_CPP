#pragma once

//USB Config settings

#include <cstdint>
#include "Pins.hpp"

struct UsbConfig {

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    /////// user provided data ////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    //---power/wakeup options-------------------------------------------------------
    static const bool self_powered = true;      //self-powered=1, bus-powered=0
    static const bool remote_wakeup = true;     //remote wakeup enabled=1

    //---interrupt------------------------------------------------------------------
    static const uint8_t usb_irq_pri = 1;       //usb interrupt priority
    static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority

    //---endpoints------------------------------------------------------------------
    static const uint8_t last_ep_num = 4;       //last endpoint number used

    //---vbus pin-------------------------------------------------------------------
    static const Pins::RPN vbus_pin_n = Pins::B6; //fixed pin, check datasheet

    //---debug----------------------------------------------------------------------
    static const bool debug_on = true;          //debug usb
};



