#pragma once

//USB Config settings

#include <cstdint>
#include "Pins.hpp"
#include "UsbEndpt.hpp"

struct UsbEndpt; //not sure why i have to declare this as above include
//has the info- this is temporary

struct UsbConfig {

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    /////// user provided data ////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

    //---power/wakeup options-------------------------------------------------------
    static const bool self_powered = 1;         //self-powered=1, bus-powered=0
    static const bool remote_wakeup = 1;        //remote wakeup enabled=1

    //---vbus pin-------------------------------------------------------------------
    static Pins vbus_pin;                       //Vbus pin
    static const Pins::RPN vbus_pin_n = Pins::B6;

    //---interrupt------------------------------------------------------------------
    static const uint8_t usb_irq_pri = 5;       //usb interrupt priority
    static const uint8_t usb_irq_subpri = 0;    //usb interrupt sub-priority

    //---interrupt------------------------------------------------------------------
    static UsbEndpt endpoints[];

    //---endpoints------------------------------------------------------------------
    static const uint8_t last_endp = 4; //last endpoint number used

    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    /////// usb uses irq only, no polling /////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////

};



