// Author sumikiru, made after learning Druid Mechanics' Course.


#include "Actor/MagicCircle.h"

#include "Components/DecalComponent.h"

AMagicCircle::AMagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MagicCircleDecal"));
	// MagicCircleDecal->SetupAttachment(GetRootComponent());
	SetRootComponent(MagicCircleDecal); // 由于目前还没有根组件，暂时将MagicCircleDecal设置为RootComponent
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
