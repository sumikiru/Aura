// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraArcaneShards.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

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
		CauseDamage(TargetActor, RadialDamageOrigin);
	}
}
