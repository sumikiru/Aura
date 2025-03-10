// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Aura/Public/Characters/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("BurnDebuffComponent");
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Burn;
	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>("StunDebuffComponent");
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Stun;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false); // 只需要有一个组件生成重叠事件，否则例如火焰效果会触发多次
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true); // 生成重叠事件，针对ECR_Overlap

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionProfileName(FName("NoCollision"));

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	// 记得在蓝图中设置好Passive Spell Tag
	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionNiagaraComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);
	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonNiagaraComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonNiagaraComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);
}

void AAuraCharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 让EffectAttachComponent不随着旋转，注意需要设置PrimaryActorTick.bCanEverTick = true
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 被标记为Replicated的属性必须在这里设定
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
}

float AAuraCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageTaken = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	OnDamageDelegate.Broadcast(DamageTaken);
	return DamageTaken;
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::ReceiveKnockback(const FVector& KnockbackForce)
{
	LaunchCharacter(KnockbackForce, true, true);
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	MulticastHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(
		this,
		DeathSound,
		GetActorLocation(),
		GetActorRotation()
	);

	//布娃娃效果
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse * 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
	bDead = true;
	// 确保能够删除DebuffComponent（非必须）
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	OnDeathDelegate.Broadcast(this);
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0 : BaseWalkSpeed;
}

void AAuraCharacterBase::OnRep_Stunned()
{
}

void AAuraCharacterBase::OnRep_Burned()
{
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	// 不一定有武器：checkf(Weapon, TEXT("Weapon is not set"));
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	// 武器
	if (IsValid(Weapon) && MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
	{
		if (WeaponTipSocketName == NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponTipSocketName is not set!"));
		}
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	// 非武器（如爪子）
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		if (LeftHandSocketName == NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponTipSocketName is not set!"));
		}
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		if (RightHandSocketName == NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponTipSocketName is not set!"));
		}
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		if (TailSocketName == NAME_None)
		{
			UE_LOG(LogTemp, Warning, TEXT("TailSocketName is not set!"));
		}
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector::ZeroVector;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for (const FTaggedMontage& TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 AAuraCharacterBase::GetMinionCount_Implementation()
{
	return MinionCount;
}

void AAuraCharacterBase::IncrementMinionCount_Implementation(int32 Amount)
{
	MinionCount = FMath::Max(0, MinionCount + Amount);
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnAscRegisteredDelegate;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeathDelegate;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool bInShock)
{
	bIsBeingShocked = bInShock;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

FOnDamageSignature& AAuraCharacterBase::GetOnDamageSignature()
{
	return OnDamageDelegate;
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameEffectClass, const float& Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameEffectClass);
	FGameplayEffectContextHandle EffectContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this); //注意加上
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameEffectClass, Level, EffectContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributeEffectClass, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributeEffectClass, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributeEffectClass, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->AddCharacterAbilities(StartupAbilities);
	AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMaterialInstance);
		StartDissolveTimeline(DynamicMaterialInstance);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterialInstance = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMaterialInstance);
		StartWeaponDissolveTimeline(DynamicMaterialInstance);
	}
}
