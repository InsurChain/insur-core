#include "fp32.h"
typedef unsigned __int128 fixuint_t;

fixuint_t ___fixunssfti(uint32_t a) {
    const rep_t aRep = a;
    const rep_t aAbs = aRep & absMask;
    const int sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const rep_t significand = (aAbs & significandMask) | implicitBit;
    if ((unsigned)exponent >= sizeof(fixuint_t) * CHAR_BIT)
        return ~(fixuint_t)0;
    if (exponent < significandBits)
        return significand >> (significandBits - exponent);
    else
        return (fixuint_t)significand << (exponent - significandBits);
}
