/*
 * diffshield.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: glenn
 */



#include "diffshield.h"
#include "bignum.h"
#include "main.h"

static const int64 nTargetTimespan = 18000;
static const int64 nTargetSpacing = 60;
static const int64 nInterval = nTargetTimespan / nTargetSpacing;

static CBigNum bnProofOfWorkLimit(~uint256(0) >> 20); // LitecoinDark: starting difficulty is 1 / 2^12
extern bool fTestNet;

extern int nHeight;


namespace litecoindark
{
	namespace difficulty_shield
	{
		difficulty_engine::difficulty_engine() :
			target_timespan(nTargetTimespan),
			target_spacing(nTargetSpacing),
			interval(nInterval)
		{}

		difficulty_engine::~difficulty_engine(){}

		unsigned difficulty_engine::compute_min_work(unsigned base, int64 time)
		{
		    // Testnet has min-difficulty blocks
		    // after nTargetSpacing*2 time between blocks:
		    if (fTestNet && time > target_spacing*2)
		        return bnProofOfWorkLimit.GetCompact();

		    CBigNum bnResult;
		    bnResult.SetCompact(base);

		    while (time > 0 && bnResult < bnProofOfWorkLimit)
		    {
		        // Maximum 400% adjustment...
		        bnResult *= 4;
		        // ... in best-case exactly 4-times-normal target time
		        time -= target_timespan * 4;
		    }

		    if (bnResult > bnProofOfWorkLimit)
		        bnResult = bnProofOfWorkLimit;

		    return bnResult.GetCompact();
		}


		legacy_difficulty_engine::legacy_difficulty_engine()
		{}

		legacy_difficulty_engine::~legacy_difficulty_engine() {}

		unsigned legacy_difficulty_engine::get_next_work_required(const CBlockIndex* pindexLast, const CBlockHeader* pblock)
		{
			unsigned int nProofOfWorkLimit = bnProofOfWorkLimit.GetCompact();

			// Genesis block
			if (pindexLast == nullptr)
				return nProofOfWorkLimit;

			// Only change once per interval
			if ((pindexLast->nHeight+1) % interval != 0)
			{
				// Special difficulty rule for testnet:
				if (fTestNet)
				{
					// If the new block's timestamp is more than 2* 10 minutes
					// then allow mining of a min-difficulty block.
					if (pblock->nTime > pindexLast->nTime + target_spacing * 2)
						return nProofOfWorkLimit;
					else
					{
						// Return the last non-special-min-difficulty-rules-block
						const CBlockIndex* pindex = pindexLast;
						while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
							pindex = pindex->pprev;
						return pindex->nBits;
					}
				}

				return pindexLast->nBits;
			}

			// LitecoinDark: This fixes an issue where a 51% attack can change difficulty at will.
			// Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
			int blockstogoback = interval - 1;
			if ((pindexLast->nHeight+1) != interval)
				blockstogoback = interval;

			// Go back by what we want to be 14 days worth of blocks
			const CBlockIndex* pindexFirst = pindexLast;
			for (int i = 0; pindexFirst && i < blockstogoback; i++)
				pindexFirst = pindexFirst->pprev;
			assert(pindexFirst);

			// Limit adjustment step
			int64 actual_timespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
			printf("  nActualTimespan = %"PRI64d"  before bounds\n", actual_timespan);

			if (actual_timespan < target_timespan / 4)
				actual_timespan = target_timespan / 4;
			if (actual_timespan > target_timespan * 4)
				actual_timespan = target_timespan * 4;

			// Retarget
			CBigNum bnNew;
			bnNew.SetCompact(pindexLast->nBits);
			bnNew *= actual_timespan;
			bnNew /= target_timespan;

			if (bnNew > bnProofOfWorkLimit)
				bnNew = bnProofOfWorkLimit;

			/// debug print
			printf("DifficultyShield RETARGET\n");
			printf("target_timespan = %"PRI64d"    actual_timespan = %"PRI64d"\n", target_timespan, actual_timespan);
			printf("Before: %08x  %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString().c_str());
			printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

			return bnNew.GetCompact();
		}

		litecoindark_difficulty_engine::litecoindark_difficulty_engine()
		{}

		litecoindark_difficulty_engine::~litecoindark_difficulty_engine(){}

		unsigned litecoindark_difficulty_engine::get_next_work_required(const CBlockIndex* pindexLast, const CBlockHeader* pblock)
		{
			unsigned int nProofOfWorkLimit = bnProofOfWorkLimit.GetCompact();

			// Genesis block
			if (pindexLast == nullptr)
				return nProofOfWorkLimit;

			// Only change once per interval
			if ((pindexLast->nHeight+1) % interval != 0)
			{
				// Special difficulty rule for testnet:
				if (fTestNet)
				{
					// If the new block's timestamp is more than 2* 10 minutes
					// then allow mining of a min-difficulty block.
					if (pblock->nTime > pindexLast->nTime + target_spacing * 2)
						return nProofOfWorkLimit;
					else
					{
						// Return the last non-special-min-difficulty-rules-block
						const CBlockIndex* pindex = pindexLast;
						while (pindex->pprev && pindex->nHeight % nInterval != 0 && pindex->nBits == nProofOfWorkLimit)
							pindex = pindex->pprev;
						return pindex->nBits;
					}
				}

				return pindexLast->nBits;
			}

			// LitecoinDark: This fixes an issue where a 51% attack can change difficulty at will.
			// Go back the full period unless it's the first retarget after genesis. Code courtesy of Art Forz
			int blockstogoback = interval - 1;
			if ((pindexLast->nHeight+1) != interval)
				blockstogoback = interval;

			// Go back by what we want to be 14 days worth of blocks
			const CBlockIndex* pindexFirst = pindexLast;
			for (int i = 0; pindexFirst && i < blockstogoback; i++)
				pindexFirst = pindexFirst->pprev;
			assert(pindexFirst);

			// Limit adjustment step
			int64 actual_timespan = pindexLast->GetBlockTime() - pindexFirst->GetBlockTime();
			printf("  nActualTimespan = %"PRI64d"  before bounds\n", actual_timespan);

			if (actual_timespan < target_timespan / 4)
				actual_timespan = target_timespan / 4;
			if (actual_timespan > target_timespan * 4)
				actual_timespan = target_timespan * 4;

			// Retarget
			CBigNum bnNew;
			bnNew.SetCompact(pindexLast->nBits);
			bnNew *= actual_timespan;
			bnNew /= target_timespan;

			if (bnNew > bnProofOfWorkLimit)
				bnNew = bnProofOfWorkLimit;

			/// debug print
			printf("DifficultyShield RETARGET\n");
			printf("target_timespan = %"PRI64d"    actual_timespan = %"PRI64d"\n", target_timespan, actual_timespan);
			printf("Before: %08x  %s\n", pindexLast->nBits, CBigNum().SetCompact(pindexLast->nBits).getuint256().ToString().c_str());
			printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

			return bnNew.GetCompact();
		}
	}
}

