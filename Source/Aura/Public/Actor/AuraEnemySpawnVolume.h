// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	AAuraEnemySpawnVolume();

	/** Save Interface */
	virtual void LoadActor_Implementation() override;
	/** End Save Interface */

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints; // UObject 的派生类必须通过指针引用，不能直接使用AAuraEnemySpawnPoint。只有指针才能被GC识别

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;
};
