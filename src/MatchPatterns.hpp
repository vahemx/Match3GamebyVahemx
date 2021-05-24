#ifndef _MATCH_PATTERNS_HPP_
#define _MATCH_PATTERNS_HPP_

#include "Match3GameDefs.hpp"

/// List match patterns in priority order
MatchPatterns matchPatterns = 
{
	/// Square pattern with radial bomb
	{
		{ RB, RB },
		{ RB, RB },
	},
	/// 4 Element line pattern with linear bomb
	{
		{ LB, LB, LB, LB },
	},
	/// T shape pattern with radial bomb
	{
		{ RB, RB, RB },
		{ 0,  RB,  0 },
		{ 0,  RB,  0 },
	}
	// You can add as many patterns as you wish
};


#endif