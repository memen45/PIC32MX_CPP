#include "Markup.hpp"

#include <cstdio>  //memcpy
#include <cstring> //strlen


            //cannot get constexpr to return a struct for some reason
            //in this version of gcc, so do something else to create
            //codes (strings) with the length of string...
            // -1 as not including leading markup char
            #define MKSTR(str) { str, strlen(str)-1 }

            const Markup::codes_t Markup::m_codes[] = {
                MKSTR( m_blacK ),   MKSTR( m_Red ),
                MKSTR( m_Green ),   MKSTR( m_Yellow ),
                MKSTR( m_Blue ),    MKSTR( m_Magenta ),
                MKSTR( m_Cyan ),    MKSTR( m_White ),

                MKSTR( m_black ),   MKSTR( m_red ),
                MKSTR( m_green ),   MKSTR( m_yellow ),
                MKSTR( m_blue ),    MKSTR( m_magenta ),
                MKSTR( m_cyan ),    MKSTR( m_white ),

                MKSTR( m_reset ),   MKSTR( m_italic ),
                MKSTR( m_normal ),  MKSTR( m_underline ),
                {0,0}
            };


// process string from Sprintf
// replace markup code with ansi code
//=============================================================================
            auto Markup::
markup      (char* str, size_t siz) -> bool
            {
            char* str_copy = str;
            char buf[siz];
            size_t idxbuf = 0;
            bool any = false;

            for(; idxbuf < siz-2; str++){
                buf[idxbuf] = *str;     //always copy (will get 0 terminator)
                if(not *str) break;     //0 terminator found
                if(*str != m_trigger){  //no markup
                    idxbuf++;           //keep copied byte
                    continue;           //next
                }

                //triggered
                any = true;             //flag the need to do a memcpy later
                str++;                  //skip trigger char

                if(*str == m_trigger) continue; //was escaped markup char

                //now inside of markup, get all until find markup end char
                for( ; idxbuf < siz-2; str++ ){
                    if( not *str ) return strip(str_copy);  //0 terminator in markup
                    if(*str == m_trigger_end ) break;       //markup done
                    if(*str == m_turnoff) m_ison = false;   //check for on/off
                    if(*str == m_turnon) m_ison = true;
                    if(not m_ison) continue;                //skip markup if off

                    //check for valid markup char, insert string data if found
                    for(auto i = 0; m_codes[i].siz; i++){
                        if(m_codes[i].ansistr[0] != *str) continue; //no match
                        if(idxbuf+m_codes[i].siz >= siz-2) return strip(str_copy); //cannot fit
                        memcpy((void*)&buf[idxbuf], m_codes[i].ansistr+1, m_codes[i].siz);
                        idxbuf += m_codes[i].siz;
                        break;
                    }
                }
            }
            if(idxbuf >= siz-1) return strip(str_copy);
            if(any) memcpy((void*)str_copy, buf, idxbuf+1);
            return true; //markup success
            }

//strip markup from string (modify string in place)
//if markup fails due to size, all markup gets stripped and is still printed
//=============================================================================
            auto Markup::
strip       (char* src) -> bool
            {
            char* dst = src;
            // "{{"" -> "{"  "{Wk}" -> ""
            while(*dst = *src, *src){
                if( *src++ != m_trigger ){ dst++; continue; }
                if( *src == m_trigger){ src++; dst++; continue; }
                while( *src and *src++ != m_trigger_end );
            }
            return false; //markup failed
            }

