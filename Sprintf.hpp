#pragma once

#include <cstdint>
#include <cstddef> //size_t
#include <cstdarg>  //variadic args
#include "Markup.hpp"

// cannot use this class directly
//
// devices like uart, usb cdc can inherit Sprintf (private)
// for printf use with simple markup for ansi colors
// each device that uses Sprintf can add a template to its
// struct for printf, which will call Sprintff::sprintf
// and returns a buffer where the device can ouptut as needed
//
// see Uart.hpp and UsbCdcAcm.hpp for printf template example
//
// each Sprintf user will get its own buffer

struct Sprintf  : private Markup {

    protected:

            auto
sprintf     (char const *, ...) -> char*;


    private:

            //set size as needed
            static const size_t m_bufsiz{256};
            //each Sprintf will get its own buffer
            char m_buf[m_bufsiz];

};

