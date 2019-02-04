#pragma once

#include <cstdint>
#include <cstddef> //size_t
#include <cstdarg>  //variadic args
#include "Markup.hpp"

struct Sprintf  : private Markup {

    protected:

            auto
sprintf     (char const *, ...) -> char*;


    private:

            static const size_t m_bufsiz{256};
            char m_buf[m_bufsiz];

};

