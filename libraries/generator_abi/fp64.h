#ifndef __compiler_rt_fp_64_h__
#define __compiler_rt_fp_64_h__

#include <stdint.h>

typedef uint64_t rep_t;

#define REP_C (uint64_t)
#define significandBits 52 
#define exponentBias    (maxExponent >> 1)

#define implicitBit     (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (REP_C(1) << (significandBits + exponentBits))

#endif //__compiler_rt_fp_h__
