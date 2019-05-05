#ifndef __compiler_rt_fp_32_h__
#define __compiler_rt_fp_32_h__

#include <stdint.h>

typedef uint32_t rep_t;

#define REP_C (uint32_t)
#define significandBits 23 
#define exponentBias    (maxExponent >> 1)

#define implicitBit     (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (REP_C(1) << (significandBits + exponentBits))

#endif //__compiler_rt_fp_h__
