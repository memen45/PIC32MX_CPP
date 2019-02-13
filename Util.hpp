#pragma once


namespace Basename {

    //find offset of / or \ in const char*
    template <size_t S>
    static inline constexpr size_t basename(const char (& str)[S], size_t i = S - 1)
    {
        return (str[i] == '/' || str[i] == '\\') ? i + 1 :
            i > 0 ? basename(str, i - 1) : 0;
    }

    //terminate recursion
    template<typename T>
    static inline constexpr T basename(T (& str)[1]){ return 0; }

    //using this (via macro below) will prevent code generation
    template <size_t V>
    struct const_expr_value
    {
        static constexpr const size_t value = V;
    };

};

// from __FILE__, get the basename (address of, which is const char*)
// filename provided to compiler may have path included, this will
// get the basename so any debug output of filenames can show the
// filename only, not inlcuding any path
// (full file path will still get stored in flash)
#define FILE_BASENAME &__FILE__[ \
        Basename::const_expr_value< Basename::basename(__FILE__) >::value \
    ]
