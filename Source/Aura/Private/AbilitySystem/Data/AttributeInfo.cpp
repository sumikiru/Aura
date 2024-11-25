// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AbilitySystem/Data/AttributeInfo.h"

#include "AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if(Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Unable to find attribute info for tag: [%s]"), *AttributeTag.ToString());
	}
	return FAuraAttributeInfo();
}
