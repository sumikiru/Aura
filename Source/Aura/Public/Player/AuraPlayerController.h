// Author: sumikiru ,learning after Druid Mechanics' Course.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraInputMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();
	/*
	 * 对于需要进行访问追踪的UPROPERTY成员变量，可以使用TObjPtr<T>来替换裸指针
	 * 对于函数参数、局部指针变量等，则建议使用UObject* 裸指针
	 * TObjPtr作为UPROPERTY成员时可以完全替换裸指针，且在release版本时完全等于裸指针
	 */
	IEnemyInterface* LastActor;
	IEnemyInterface* CurrentActor;
};
