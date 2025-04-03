// Author sumikiru, made after learning Druid Mechanics' Course.


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(Box);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Box->SetCollisionObjectType(ECC_WorldStatic);
	Box->SetCollisionResponseToAllChannels(ECR_Ignore);
	Box->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::LoadActor_Implementation()
{
	if (bReached)
	{
		Destroy();
	}
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnSphereOverlap);
}

void AAuraEnemySpawnVolume::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                            int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 如果不是Aura，直接返回
	if (!OtherActor->Implements<UPlayerInterface>())
	{
		return;
	}

	bReached = true;
	for (AAuraEnemySpawnPoint* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			Point->SpawnEnemy();
		}
	}
	/**
	 * 不应该直接在这里Destroy();而是应该禁用碰撞，这样才会在AAuraGameModeBase::SaveWorldState()中使用FActorIterator遍历世界中所有Actor时，
	 * 不会因为!IsValid(Actor)跳过保存
	 * 只需要标记bReached = false, 在下次游戏开始时，BeginPlay()中会Destroy()
	 */
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
