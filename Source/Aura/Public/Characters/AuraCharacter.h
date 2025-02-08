// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//~ Begin PlayerInterface
	virtual void AddToXP_Implementation(int32 InXP) override;
	//~ End PlayerInterface

	//~ Begin CombatInterface
	virtual int32 GetPlayerLevel() override;
	//~ End CombatInterface

private:
	virtual void InitAbilityActorInfo() override;
};
