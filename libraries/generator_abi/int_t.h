#ifndef __compiler_int_t_h__
#define __compiler_int_t_h__
#include <stdint.h>

typedef union
{
    unsigned __int128 all;
    struct
    {
        uint64_t low;
        uint64_t high;
    }s;
} utwords;

typedef union
{
    udwords u;
    double  f;
} double_bits;

typedef union
{
    __int128 all;
    struct
    {
        uint64_t low;
        int64_t high;
    }s;
} twords;

typedef union
{
    uint64_t all;
    struct
    {
        uint32_t low;
        uint32_t high;
    }s;
} udwords;
#endif// __compiler_int_t_h__
