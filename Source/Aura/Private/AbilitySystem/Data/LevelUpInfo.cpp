// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;
	while (bSearching)
	{
		// LevelUpInformation[i] = Level i Information, Level 0 is invalid
		if (LevelUpInformation.Num() - 1 <= Level)
		{
			return Level;
		}
		
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}
