
#include "int_t.h"
#include <float.h>

double ___floatuntidf(tu_int a)
{
    if (a == 0)
        return 0.0;
    const unsigned N = sizeof(tu_int) * CHAR_BIT;
    int sd = N - __clzti2(a);  
    int e = sd - 1; 
    switch (sd)
    {
    case DBL_MANT_DIG + 1:
        a <<= 1;
        break;
    case DBL_MANT_DIG + 2:
        break;
    default:
        a = (a >> (sd - (DBL_MANT_DIG+2))) |
            ((a & ((tu_int)(-1) >> ((N + DBL_MANT_DIG+2) - sd))) != 0);
    };
    a |= (a & 4) != 0;  
    ++a; 
    a >>= 2;
    if (a & ((tu_int)1 << DBL_MANT_DIG))
    {
        a >>= 1;
        ++e;
    }
    double_bits fb;
    fb.u.s.high = ((e + 1023) << 20) | ((uint32_t)(a >> 32) & 0x000FFFFF); 
    fb.u.s.low = (uint32_t)a; 
    return fb.f;
}
