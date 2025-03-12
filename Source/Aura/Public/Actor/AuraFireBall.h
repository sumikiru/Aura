// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();
	// 用于FireBall回到ReturnToActor身边时爆炸造成伤害效果
	UFUNCTION(BlueprintCallable)
	void Blast();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

protected:
	virtual void BeginPlay() override;
	virtual void OnHit() override;
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                                  int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Blast")
	float RadialDamageInnerRadius = 50.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Blast")
	float RadialDamageOuterRadius = 300.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Blast")
	float KnockbackMagnitude = 800.f;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Blast")
	float DeathImpulseMagnitude = 600.f;
};
