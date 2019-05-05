#ifndef __compiler_rt_fp_128_h__
#define __compiler_rt_fp_128_h__

#include <stdint.h>

#define REP_C (__uint128_t)
#define significandBits 112
#define exponentBias    (maxExponent >> 1)

#define implicitBit     (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (REP_C(1) << (significandBits + exponentBits))
#endif //__compiler_rt_fp_h__
