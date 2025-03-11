// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraElectrocute.h"

FString UAuraElectrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(
			TEXT(
				// Title
				"<Title>Electrocute</>\n\n"

				// Details: Level
				"<Small>Level: </><Level>%d</>\n"
				// Details: ManaCost
				"<Small>ManaCost: </><ManaCost>%.1f</>\n"
				// Details: Cooldown
				"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

				// Description
				"<Default>Emits a beam of lightning, "
				"connecting with the target, repeatedly causing </>"

				// Damage
				"<Damage>%d</><Default> lightning damage with "
				"a chance to stun</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	return FString::Printf(
		TEXT(
			// Title
			"<Title>Electrocute</>\n\n"

			// Details: Level
			"<Small>Level: </><Level>%d</>\n"
			// Details: ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Details: Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Additional Number of Shock Targets
			"<Default>Emits a beam of lightning, "
			"propagating to %d additional targets nearby, causing </>"

			// Damage
			"<Damage>%d</><Default> lightning damage with "
			"a chance to stun</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShockTargets - 1),
		ScaledDamage);
}

FString UAuraElectrocute::GetNextLevelDescription(int32 NextLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(NextLevel);
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

			// Additional Number of Shock Targets
			"<Default>Emits a beam of lightning, "
			"propagating to %d additional targets nearby, causing </>"

			// Damage
			"<Damage>%d</><Default> lightning damage with "
			"a chance to stun</>"),

		// Values
		NextLevel,
		ManaCost,
		Cooldown,
		FMath::Min(NextLevel, MaxNumShockTargets - 1),
		ScaledDamage);
}
