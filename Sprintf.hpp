#pragma once

#include <cstdint>
#include <cstddef> //size_t
#include <cstdarg>  //variadic args
#include "Markup.hpp"

// cannot use this class directly
//
// devices like uart, usb cdc can inherit Sprintf (private)
// for printf use
// each device that uses Sprintf can add a template to its
// struct for printf, which will call Sprintf::sprintf
// and returns a buffer which the device can ouptut as needed
//
// see Uart.hpp and UsbCdcAcm.hpp for printf template example
//
// each class that inherits Sprintf will get its own buffer
// (set to 256 here, which should be plenty for normal use)


struct Sprintf  : private Markup {

    protected:

            auto
sprintf     (char const *, ...) -> char*;

            auto
sprintf     (char const *, va_list)-> char*;

    private:

            //set size as needed
            static const size_t m_bufsiz{256};
            char m_buf[m_bufsiz];

};

