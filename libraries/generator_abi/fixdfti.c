#include "fp64.h"

typedef __int128 fixint_t;
typedef unsigned __int128 fixuint_t;

fixint_t ___fixdfti(uint64_t a) {
    const fixint_t fixint_max = (fixint_t)((~(fixuint_t)0) / 2);
    const fixint_t fixint_min = -fixint_max - 1;
    const rep_t aRep = a;
    const rep_t aAbs = aRep & absMask;
    const fixint_t sign = aRep & signBit ? -1 : 1;
    const int exponent = (aAbs >> significandBits) - exponentBias;
    const rep_t significand = (aAbs & significandMask) | implicitBit;
    if (exponent < significandBits)
        return sign * (significand >> (significandBits - exponent));
    else
        return sign * ((fixint_t)significand << (exponent - significandBits));

}
