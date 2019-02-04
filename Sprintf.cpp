#include "Sprintf.hpp"
#include <cstdio> //vsnprintf

            auto Sprintf::
sprintf     (char const *fmt, ...) -> char*
            {
            va_list args;
            va_start(args, fmt);
            vsnprintf(m_buf, 256, fmt, args);
            markup(m_buf);
            va_end(args);
            return m_buf;
            }