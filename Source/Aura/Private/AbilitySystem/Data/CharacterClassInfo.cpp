// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	// 如果找不到，将会有一个断言
	return CharacterClassInformation.FindChecked(CharacterClass);
}
