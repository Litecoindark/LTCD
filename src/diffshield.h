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

/**
 *
 */
class DifficultyShield
{
public:
	DifficultyShield();

	// Timespan

	const int64 target_timespan;	// Timespan between difficulty adjustments

	// Spacing

	const int64 target_spacing; // Target block spacing

	// Interval

	int64 interval;

	unsigned int ComputeMinWork(unsigned int nBase, int64 nTime);

};



#endif /* DIFFSHIELD_H_ */
