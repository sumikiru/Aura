// Author sumikiru, made after learning Druid Mechanics' Course.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
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
			ScaledDamage);
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
		ScaledDamage);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 NextLevel)
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
		ScaledDamage);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride,
                                     AActor* HomingTarget)
{
	/*True if this is the server or single player*/
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	/**
	 * 因为GetCombatSocketLocation改为了在蓝图中编译生成函数
	 * 所以不要在接口中直接调用Event函数（CombatInterface->GetCombatSocketLocation()），而是调用Execute_GetCombatSocketLocation
	 * 以该方式调用，不需要再进行ICombatInterface的Cast结果检查
	 */
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(
		GetAvatarActorFromActorInfo(),
		SocketTag /*旧为FAuraGameplayTags::Get().CombatSocket_Weapon*/);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread,
	                                                                             EffectiveNumProjectiles);

	for (const FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rot.Quaternion());

		checkf(ProjectileClass, TEXT("Projectile Class is not set"));
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		// 是敌人
		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			// 由于HomingTargetComponent是TWeakObjectPtr，所以需要在AuraProjectile中新增一个TObjectPtr作为中介，确保GC
			// 而不是直接Projectile->ProjectileMovementComponent->HomingTargetComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->ProjectileMovementComponent->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);
		Projectile->ProjectileMovementComponent->bIsHomingProjectile = bLaunchHomingProjectiles;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
