// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Aura/Public/Characters/AuraCharacterBase.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionProfileName(FName("NoCollision"));
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}
