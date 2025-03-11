// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(
			TEXT(
				// Title
				"<Title>Arcane Shards</>\n\n"

				// Details: Level
				"<Small>Level: </><Level>%d</>\n"
				// Details: ManaCost
				"<Small>ManaCost: </><ManaCost>%.1f</>\n"
				// Details: Cooldown
				"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

				// Description
				"<Default>Summon a shard of arcane energy, "
				"causing radial arcane damage of </>"

				// Damage
				"<Damage>%d</><Default> at the shard origins</>"),

			// Values
			Level,
			ManaCost,
			Cooldown,
			ScaledDamage);
	}
	return FString::Printf(
		TEXT(
			// Title
			"<Title>Arcane Shards</>\n\n"

			// Details: Level
			"<Small>Level: </><Level>%d</>\n"
			// Details: ManaCost
			"<Small>ManaCost: </><ManaCost>%.1f</>\n"
			// Details: Cooldown
			"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

			// Additional Number of Shock Targets
			"<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"

			// Damage
			"<Damage>%d</><Default> at the shard origins</>"),

		// Values
		Level,
		ManaCost,
		Cooldown,
		FMath::Min(Level, MaxNumShards),
		ScaledDamage);
}

FString UAuraArcaneShards::GetNextLevelDescription(int32 NextLevel)
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
			"<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"

			// Damage
			"<Damage>%d</><Default> at the shard origins</>"),

		// Values
		NextLevel,
		ManaCost,
		Cooldown,
		FMath::Min(NextLevel, MaxNumShards),
		ScaledDamage);
}

void UAuraArcaneShards::CauseDamageToAllTargets(const FVector& RadialDamageOrigin)
{
	TArray<AActor*> OverlappingTargets;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetAvatarActorFromActorInfo());
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingTargets,
		ActorsToIgnore, // 或者TArray<AActor*>({GetAvatarActorFromActorInfo()}),
		RadialDamageOuterRadius,
		RadialDamageOrigin);

	for (AActor* TargetActor : OverlappingTargets)
	{
		FVector ImpulseDirectionOverride = TargetActor->GetActorLocation() - RadialDamageOrigin;
		CauseDamage(TargetActor,
		            RadialDamageOrigin,
		            true,
		            ImpulseDirectionOverride,
		            true,
		            ImpulseDirectionOverride,
		            true,
		            45.f);
	}
}
