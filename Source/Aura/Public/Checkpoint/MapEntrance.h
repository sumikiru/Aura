// Author sumikiru, made after learning Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()

public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);

	/** Highlight Interface */
	virtual void HighlightActor_Implementation() override;
	/** End Highlight Interface */

	/** SaveInterface */
	virtual void LoadActor_Implementation() override;
	/** End SaveInterface */

	/**
	 * 软引用不会主动加载，只在使用时才会加载；
	 * 可随时灵活加载，减小加载负担，缩短启动时间；
	 * 需要手动加载
	 */
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	                             bool bFromSweep, const FHitResult& SweepResult) override;
};
