// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageType(Level, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(
			TEXT(
				// Title
				"<Title>Fire Bolt</>\n\n"

				// Details: Level
				"<Small>Level: </><Level>%d</>\n"
				// Details: ManaCost
				"<Small>ManaCost: </><ManaCost>%.1f</>\n"
				// Details: Cooldown
				"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

				// Description
				"<Default>Launches a bolt of fire, "
				"exploading on impact and dealing </>"

				// Damage
				"<Damage>%d</><Default> fire damage with "
				"a chance to burn</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			Damage);
	}
	return FString::Printf(
		TEXT(
			// Title
			"<Title>Fire Bolt</>\n\n"

			// Details: Level
			"<Small>Level: </><Level>%d</>\n"
			// Details: ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Details: Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploading on impact and dealing </>"

			// Damage
			"<Damage>%d</><Default> fire damage with "
			"a chance to burn</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, NumProjectiles),
		Damage);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 NextLevel)
{
	const int32 Damage = GetDamageByDamageType(NextLevel, FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(NextLevel));
	const float Cooldown = GetCooldown(NextLevel);
	return FString::Printf(
		TEXT(
			// Title
			"<Title>Next Level</>\n\n"

			// Details: Level
			"<Small>Level: </><Level>%d</>\n"
			// Details: ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Details: Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Number of FireBolts
			"<Default>Launches %d bolts of fire, "
			"exploading on impact and dealing </>"

			// Damage
			"<Damage>%d</><Default> fire damage with "
			"a chance to burn</>"),

		// Values
		NextLevel,
		ManaCost,
		Cooldown,
		FMath::Min(NextLevel, NumProjectiles),
		Damage);
}
