// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Player/AuraPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	LastActor = nullptr;
	CurrentActor = nullptr;
	
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if(!CursorHit.bBlockingHit)
	{
		return;
	}

	LastActor = CurrentActor;
	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	/**
	 * Line trace from cursor. There are several scenarios.
	 * A. Last Actor is null && Current Actor is null
	 *	- Do nothing
	 * B. Last Actor is null && Current Actor is valid
	 *  - Highlight Current Actor
	 * C. Last Actor is valid && Current Actor is null
	 *  - Unhighlight Last Actor
	 * D. Both Actors are valid, but Last Actor != Current Actor
	 *  - Unhighlight Last Actor, Highlight Current Actor
	 * E. Both Actors are valid, and Last Actor == Current Actor
	 *  - Do nothing
	 */
	if(LastActor == nullptr)
	{
		if(CurrentActor != nullptr)
		{
			// Case B
			CurrentActor->HighlightActor();
		}
		else
		{
			// Case A
		}
	}
	else
	{
		if(CurrentActor == nullptr)
		{
			//Case C
			LastActor->UnHighlightActor();
		}
		else
		{
			if(LastActor == CurrentActor)
			{
				//Case D
				LastActor->UnHighlightActor();
				CurrentActor->HighlightActor();
			}
			else
			{
				//Case E
			}
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraInputMappingContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);
	Subsystem->AddMappingContext(AuraInputMappingContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetHideCursorDuringCapture(false);
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//注意CastChecked<APawn>(GetPawn())和GetPawn<APawn>()的区别
	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
