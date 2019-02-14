#include "Sprintf.hpp"
#include <cstdio> //vsnprintf

// printf format into buffer, check for markup
// return buffer
//=============================================================================
            auto Sprintf::
sprintf     (char const *fmt, ...) -> char*
            {
            va_list args;
            va_start(args, fmt);
            sprintf(fmt, args);
            va_end(args);
            return m_buf;
            }

            auto Sprintf::
sprintf     (char const *fmt, va_list args) -> char*
            {
            vsnprintf(m_buf, m_bufsiz, fmt, args);
            markup(m_buf, m_bufsiz);
            return m_buf;
            }
