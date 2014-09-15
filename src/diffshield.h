/*
 * diffshield.h
 *
 *  Created on: Sep 14, 2014
 *      Author: Glenn Allen (TrollByFire)
 */

// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIFFSHIELD_H_
#define DIFFSHIELD_H_

#include "uint256.h"

class CBlockIndex;
class CBlockHeader;

#define SHIELD_FORK_TARGET 20000

namespace litecoindark
{
	namespace difficulty_shield
	{
		class difficulty_engine
		{
		protected:

			// Timespan
			const int64 target_timespan;

			// Spacing
			const int64 target_spacing;

			// Interval
			int64 interval;

		public:
			difficulty_engine();
			virtual ~difficulty_engine();

			unsigned compute_min_work(unsigned base, int64 time);
			virtual unsigned get_next_work_required(const CBlockIndex* last_index, const CBlockHeader* block) = 0;
		};

		class legacy_difficulty_engine : public difficulty_engine
		{
		public:
			legacy_difficulty_engine();
			virtual ~legacy_difficulty_engine();

			virtual unsigned get_next_work_required(const CBlockIndex* last_index, const CBlockHeader* block);
		};

		class litecoindark_difficulty_engine : public difficulty_engine
		{
		public:
			litecoindark_difficulty_engine();
			virtual ~litecoindark_difficulty_engine();

			virtual unsigned get_next_work_required(const CBlockIndex* last_index, const CBlockHeader* block);
		};
	}
}

#endif /* DIFFSHIELD_H_ */
