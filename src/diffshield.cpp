/*
 * diffshield.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: glenn
 */



#include "diffshield.h"
#include "bignum.h"

static const int64 nTargetTimespan = 18000;
static const int64 nTargetSpacing = 60;
static const int64 nInterval = nTargetTimespan / nTargetSpacing;

//extern CBigNum bnProofOfWorkLimit;
static CBigNum bnProofOfWorkLimit(~uint256(0) >> 20); // LitecoinDark: starting difficulty is 1 / 2^12
extern bool fTestNet;

DifficultyShield::DifficultyShield() :
		target_timespan(nTargetTimespan),
		target_spacing(nTargetSpacing),
		interval(nInterval)
{

}

unsigned int DifficultyShield::ComputeMinWork(unsigned int nBase, int64 nTime)
{

    // Testnet has min-difficulty blocks
    // after nTargetSpacing*2 time between blocks:
    if (fTestNet && nTime > nTargetSpacing*2)
        return bnProofOfWorkLimit.GetCompact();

    CBigNum bnResult;
    bnResult.SetCompact(nBase);
    while (nTime > 0 && bnResult < bnProofOfWorkLimit)
    {
        // Maximum 400% adjustment...
        bnResult *= 4;
        // ... in best-case exactly 4-times-normal target time
        nTime -= nTargetTimespan*4;
    }
    if (bnResult > bnProofOfWorkLimit)
        bnResult = bnProofOfWorkLimit;
    return bnResult.GetCompact();
}
