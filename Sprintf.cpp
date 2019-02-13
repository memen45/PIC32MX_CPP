#include "Sprintf.hpp"
#include <cstdio> //vsnprintf

// printf format into buffer (tf=true=markup, false=no markup)
// return buffer
//=============================================================================
            auto Sprintf::
sprintf     (bool tf, char const *fmt, ...) -> char*
            {
            va_list args;
            va_start(args, fmt);
            vsnprintf(m_buf, m_bufsiz, fmt, args);
            va_end(args);
            if(tf) markup(m_buf);
            return m_buf;
            }
