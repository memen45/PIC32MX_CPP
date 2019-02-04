#include "Sprintf.hpp"
#include <cstdio> //vsnprintf

// printf format into buffer, then check for markup
// return buffer
//=============================================================================
            auto Sprintf::
sprintf     (char const *fmt, ...) -> char*
            {
            va_list args;
            va_start(args, fmt);
            vsnprintf(m_buf, m_bufsiz, fmt, args);
            markup(m_buf);
            va_end(args);
            return m_buf;
            }