#include "Markup.hpp"

#include <cstdio>  //memcpy
#include <cstring> //strlen


            //cannot get constexpr to return a struct for some reason
            //in this version of gcc, so so something else to create
            //codes with length of string...
            // -1 as not including leading markup char
            #define MKSTR(str) { str, strlen(str)-1 }

            const Markup::codes_t Markup::m_codes[] = {
                MKSTR( m_blacK ), MKSTR( m_Red ), MKSTR( m_Green ),
                MKSTR( m_Yellow ), MKSTR( m_Blue ), MKSTR( m_Magenta ),
                MKSTR( m_Cyan ), MKSTR( m_White ),
                MKSTR( m_black ), MKSTR( m_red ), MKSTR( m_green ),
                MKSTR( m_yellow ), MKSTR( m_blue ), MKSTR( m_magenta ),
                MKSTR( m_cyan ), MKSTR( m_white ),
                MKSTR( m_reset ),
                {0,0}
            };


// process string from Sprintf
// add ansi code for markup
//=============================================================================
            auto Markup::
markup      (char* str) -> bool
            {
            //"test @Rstring@W"
            //"test \033[38;5;196mstring\033[38;5;15m"
            char* str_copy = str;
            char buf[256];
            int idxbuf = 0;
            bool any = false;
            for(; idxbuf < 255; str++){
                buf[idxbuf] = *str;
                if(not *str) break;
                if(*str != m_trigger){ idxbuf++;  continue; }
                //triggered
                any = true;
                str++; //skip '@'
                if(*str == m_trigger) continue; //"@@" ,skip second @
                if(*str == m_turnoff) m_ison = false;
                if(*str == m_turnon) m_ison = true;
                if(not m_ison) continue;
                for(auto i = 0; m_codes[i].siz; i++){
                    if(m_codes[i].ansistr[0] != *str) continue;
                    if(idxbuf+m_codes[i].siz >= 254) return strip(str_copy);
                    memcpy((void*)&buf[idxbuf], m_codes[i].ansistr+1, m_codes[i].siz);
                    idxbuf += m_codes[i].siz;
                }
            }
            if(idxbuf >= 255) return strip(str_copy);
            if(any) memcpy((void*)str_copy, buf, idxbuf+1);
            return true;
            }

//strip markup from string (modify string in place)
//if markup fails due to size, all markup gets stripped and is still printed
//=============================================================================
            auto Markup::
strip       (char* src) -> bool
            {
            char* dst = src;
            // @@ -> @  @W -> ""
            while(*dst = *src, *src){
                if(*src == m_trigger and *++src != m_trigger) src++;
                else { dst++; src++; }
            }
            return false;
            }

