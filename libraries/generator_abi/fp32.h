#ifndef __compiler_rt_fp_32_h__
#define __compiler_rt_fp_32_h__

#include <limits.h>
#include <stdint.h>
typedef uint32_t rep_t;
#define REP_C (uint32_t)
#define significandBits 23 
#define typeWidth       (sizeof(rep_t)*CHAR_BIT)
#define exponentBits    (typeWidth - significandBits - 1)
#define maxExponent     ((1 << exponentBits) - 1)
#define exponentBias    (maxExponent >> 1)

#define implicitBit     (REP_C(1) << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (REP_C(1) << (significandBits + exponentBits))
#define absMask         (signBit - 1U)

#endif //__compiler_rt_fp_h__
