// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Aura/Aura.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInstance* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);
		if (DecalMaterial)
		{
			MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
		}
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	// 需要加上IsLocalController()的判断条件，这样如果是本地客户端显示伤害数字，则服务器不会显示
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); // 动态创建组件，而不是在构造函数中CreateDefaultSubObject
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// 动态绑定组件，相当于SetupAttachment
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); // 分离开是为了播放动画时伤害数字不会跟随TargetCharacter移动
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
	{
		return;
	}
	if (APawn* ControlledPawn = GetPawn())
	{
		// 当行进途中出现障碍物时，虽然利用NavPath绕路，但是人物仍然会碰到两点之间的障碍物并且产生预期之外的平移，最终导致原定路线与实际路线差距较大。
		// 更改人物移动速度、Direction缩小（也会改变速度）等均不现实，应该考虑让NavPath生成的点至少距离障碍物一个pawn胶囊体半径的长度，且尽量多生成点。
		// 同时由于敌人的动态移动可能产生碰撞，应该考虑如果为进入目标半径范围内，应该继续移动
		// 解决方法：频繁更新PathPoints，更新频率够高就会变得自然，但是开销较大。可以使用定时器
		const FVector LocationOnSpline = Spline->
			FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			GetWorldTimerManager().ClearTimer(UpdatePathPointsTimerHandle);
			bAutoRunning = false;
			return;
		}

		ControlledPawn->AddMovementInput(Direction);
	}
}

void AAuraPlayerController::UpdatePathPoints()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn == nullptr)
	{
		return;
	}
	// 将导航路线中的点添加到Spline中。注意：需要加入Module::NavigationSystem，并且在世界场景中加上NavMeshBoundsVolume
	// Project Settings->Navigation System->Allow Client Side Navigation勾选
	// 注意：让静态网格体忽略所有射线检测（Collision Presets->Custom->Ignore Visibility），这样可以实现点击柱子等物体时，不会因为无法到达而持续移动，而是直接到达背后的地面目标
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
	{
		Spline->ClearSplinePoints();
		for (const FVector& PointLoc : NavPath->PathPoints)
		{
			Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
		}
		// 有时候CachedDestination = Hit.ImpactPoint;会使得该CachedDestination在NavPath之外，因此需要更新至NavPath的最后一个点，从而确保AutoRun()中角色能在到达边界后停下
		if (NavPath->PathPoints.Num() > 0) //防止数组越界
		{
			CachedDestination = NavPath->PathPoints.Last();
			bAutoRunning = true;
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
}

void AAuraPlayerController::CursorTrace()
{
	//判断当前事件是否被阻挡，如果事件被阻挡，则清除相关内容
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastActor)
		{
			LastActor->UnHighlightActor();
		}
		if (CurrentActor)
		{
			CurrentActor->UnHighlightActor();
		}
		LastActor = nullptr;
		CurrentActor = nullptr;
		return;
	}

	// 如果当前处于Magic Circle指示阶段，将忽略掉场景中的角色（因此也不会高亮），避免出现CursorTrace到角色，会出现突然闪现一段位置的情况
	// 这是因为CursorTrace到角色身上的位置，然后追踪到地面坐标，水平偏移会突然闪现一段位置
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludeCharacters : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit);
	if (!CursorHit.bBlockingHit)
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
	if (LastActor != CurrentActor)
	{
		if (LastActor)
		{
			LastActor->UnHighlightActor();
		}
		if (CurrentActor)
		{
			CurrentActor->HighlightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		bIsTargeting = CurrentActor != nullptr;
		bAutoRunning = false;
	}
	// 将事件传递给ASC，在ASC中对技能进行相关处理。
	if (GetASC())
	{
		GetASC()->AbilityInputTagPressed(InputTag);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}

	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
	// 只有在未追踪目标状态 且 未按下shift时，进行追踪
	if (!bIsTargeting && !bShiftKeyDown)
	{
		if (FollowTime <= ShortPressThreshold)
		{
			// 定时更新PathPoints，需要注意处于NavMesh中
			GetWorldTimerManager().SetTimer(UpdatePathPointsTimerHandle, this, &AAuraPlayerController::UpdatePathPoints, UpdatePathPointsTimeRate,
			                                true, 0);
			if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				// 添加点击时的Niagara
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
			}
		}
		FollowTime = 0.f;
		bIsTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}

	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LeftMouseButton))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	if (bIsTargeting || bShiftKeyDown)
	{
		// 已瞄准目标，释放技能
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// Move to Target
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}
		//长按移动，不需要NavPath，只需要指哪往哪走
		if (APawn* ControlledPawn = GetPawn())
		{
			GetWorldTimerManager().ClearTimer(UpdatePathPointsTimerHandle);
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraInputMappingContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//不应check(Subsystem);仅在本地的客户端中不为空
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraInputMappingContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetHideCursorDuringCapture(false);
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased,
	                                       &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//注意CastChecked<APawn>(GetPawn())和GetPawn<APawn>()的区别
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
