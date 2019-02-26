#pragma once

#include <cstdint>
#include <cstddef>  //size_t
#include <cstdarg>  //variadic args
#include <cstdio>   //vsnprintf
#include "Markup.hpp"

// cannot use this class directly
//
// devices like uart, usb cdc can inherit Sprintf (private)
// for printf use
// each device that uses Sprintf can add a function to its
// struct for printf, which will call Sprintf::sprintf
// and returns a buffer which the device can ouptut as needed
//
// see Uart.xpp and UsbCdcAcm.xpp for printf function examples
// (templates could also be used, and a lot of va_ stuff could be
//  eliminated, but code size increases, which may only be the
//  result of an older version of gcc used by XC32)
//
// each class that inherits Sprintf will get its own buffer
// (default is 256, which should be plenty for normal use)
//
// example to inherit Sprintf-
//  struct Uart : private Sprintf<> {...      //default size buffer
//  struct Uart : private Sprintf<128> {...   //buffer size is 128


template<size_t Tbufsiz = 256>
struct Sprintf  : private Markup {

    protected:

// printf format into buffer, check for markup (ansi colors)
// return buffer
// va_list already setup (expansion of variadic arguments had to be
// expanded previously in a class printf function, so only need va_list)
//=============================================================================
            auto
sprintf     (char const *fmt, va_list args) -> char*
            {
            vsnprintf(m_buf, Tbufsiz, fmt, args);
            markup(m_buf, Tbufsiz);
            return m_buf;
            }

// return pointer to buffer, each class that inherits Sprintf gets its own
// buffer which can be used as needed, this gives a way to get the address
//=============================================================================
            auto
sprintfbuf  () -> char*
            {
            return m_buf;
            }

// return buffer size
//=============================================================================
            auto
sprintfbufsiz() -> size_t
            {
            return Tbufsiz;
            }

    private:

            char m_buf[Tbufsiz];

};

