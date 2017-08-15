/*
 * util.h - define util function.
 *
 * Author: Shan Jiejing
 * Date: 2017-07-05
 */

#ifndef _UTIL_H_
#define _UTIL_H_

inline unsigned int l2ui(char* p) {
    return (((unsigned int)p[0]) |
            ((unsigned int)p[1] << 8) |
            ((unsigned int)p[2] << 16) |
            ((unsigned int)p[3] << 24));
}

inline unsigned short l2us(char* p) {
    return (((unsigned short)p[0]) |
            ((unsigned short)p[1] << 8));
}

inline void us2l(char* p, unsigned short s) {
    *p = (char)(s & 0x00FF);
    *(p + 1) = (char)((s & 0xFF00) >> 8);
}

inline void ui2l(char* p, unsigned int i) {
    p[0] = (char)(i & 0xff);
    p[1] = (char) ((i & (0xFF << 8)) >> 8);
    p[2] = (char) ((i & (0xFF << 16)) >> 16);
    p[3] = (char) ((i & (0xFF << 24)) >> 24);
}
#endif //_UTIL_H_
