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
		difficulty_engine::difficulty_engine()
			:
			target_timespan(nTargetTimespan),
			target_spacing(nTargetSpacing),
			interval(nInterval)
		{}

		difficulty_engine::difficulty_engine(int64 timespan, int64 spacing, int64 adj_interval)
		:
			target_timespan(timespan),
			target_spacing(spacing),
			interval(adj_interval)
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


		legacy_difficulty_engine::legacy_difficulty_engine(int64 timespan, int64 spacing, int64 adj_interval)
			:
			difficulty_engine(timespan, spacing, adj_interval){}

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
		
		min_difficulty_engine::min_difficulty_engine()
		{}

		min_difficulty_engine::~min_difficulty_engine(){}

		unsigned min_difficulty_engine::get_next_work_required(const CBlockIndex* pindexLast, const CBlockHeader* pblock)
		{
			return bnProofOfWorkLimit.GetCompact();
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

		kgw_difficulty_engine::kgw_difficulty_engine(int64 spacing, uint64 past_blocks_minimum, uint64 past_blocks_maximum)
			:
			difficulty_engine(nTargetTimespan, spacing, nInterval),
			past_blocks_min(past_blocks_minimum),
			past_blocks_max(past_blocks_maximum)
		{

		}

		kgw_difficulty_engine::~kgw_difficulty_engine(){}

		unsigned kgw_difficulty_engine::get_next_work_required(const CBlockIndex* pindexLast, const CBlockHeader* pblock)
		{

			/* current difficulty formula, megacoin - kimoto gravity well */
			const CBlockIndex  *BlockLastSolved                                = pindexLast;
			const CBlockIndex  *BlockReading                                = pindexLast;
			const CBlockHeader *BlockCreating                                = pblock;
													BlockCreating                                = BlockCreating;
			uint64                                PastBlocksMass                                = 0;
			int64                                PastRateActualSeconds                = 0;
			int64                                PastRateTargetSeconds                = 0;
			double                                PastRateAdjustmentRatio                = double(1);
			CBigNum                                PastDifficultyAverage;
			CBigNum                                PastDifficultyAveragePrev;
			double                                EventHorizonDeviation;
			double                                EventHorizonDeviationFast;
			double                                EventHorizonDeviationSlow;

		    if (BlockLastSolved == NULL || BlockLastSolved->nHeight == 0 || (uint64)BlockLastSolved->nHeight < past_blocks_min)
		    {
		    	return bnProofOfWorkLimit.GetCompact();
		    }

			for (unsigned int i = 1; BlockReading && BlockReading->nHeight > 0; i++)
			{
				if (past_blocks_max > 0 && i > past_blocks_max)
				{
					break;
				}

				PastBlocksMass++;

				if (i == 1)
				{
					PastDifficultyAverage.SetCompact(BlockReading->nBits);
				}
				else
				{
					PastDifficultyAverage = ((CBigNum().SetCompact(BlockReading->nBits) - PastDifficultyAveragePrev) / i) + PastDifficultyAveragePrev;
				}

				PastDifficultyAveragePrev = PastDifficultyAverage;

				PastRateActualSeconds                        = BlockLastSolved->GetBlockTime() - BlockReading->GetBlockTime();
				PastRateTargetSeconds                        = target_spacing * PastBlocksMass;
				PastRateAdjustmentRatio                        = double(1);

				if (PastRateActualSeconds < 0)
				{
					PastRateActualSeconds = 0;
				}

				if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0)
				{
					PastRateAdjustmentRatio                        = double(PastRateTargetSeconds) / double(PastRateActualSeconds);
				}

				EventHorizonDeviation     = 1 + (0.7084 * pow((double(PastBlocksMass)/double(144)), -1.228));
				EventHorizonDeviationFast = EventHorizonDeviation;
				EventHorizonDeviationSlow = 1 / EventHorizonDeviation;

				if (PastBlocksMass >= past_blocks_min)
				{
					if ((PastRateAdjustmentRatio <= EventHorizonDeviationSlow) || (PastRateAdjustmentRatio >= EventHorizonDeviationFast))
					{
						assert(BlockReading);
						break;
					}
				}
				if (BlockReading->pprev == nullptr)
				{
					assert(BlockReading);
					break;
				}

				BlockReading = BlockReading->pprev;
			}

			CBigNum bnNew(PastDifficultyAverage);

			if (PastRateActualSeconds != 0 && PastRateTargetSeconds != 0)
			{
					bnNew *= PastRateActualSeconds;
					bnNew /= PastRateTargetSeconds;
			}

		    if (bnNew > bnProofOfWorkLimit)
		    {
		    	bnNew = bnProofOfWorkLimit;
		    }

		    /// debug print
		    printf("Difficulty Retarget - Kimoto Gravity Well\n");
		    printf("PastRateAdjustmentRatio = %g\n", PastRateAdjustmentRatio);
		    printf("Before: %08x  %s\n", BlockLastSolved->nBits, CBigNum().SetCompact(BlockLastSolved->nBits).getuint256().ToString().c_str());
		    printf("After:  %08x  %s\n", bnNew.GetCompact(), bnNew.getuint256().ToString().c_str());

		    return bnNew.GetCompact();
		}
	}
}

