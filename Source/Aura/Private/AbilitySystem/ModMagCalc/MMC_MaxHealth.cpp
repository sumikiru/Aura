// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(0.f, Vigor);

	// 如果GetSourceObject()返回值为空，说明GetContext()没有获取到SourceObject
	// 需要在AAuraCharacterBase::ApplyEffectToSelf中加入EffectContextHandle.AddSourceObject(this);
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	checkf(CombatInterface, TEXT("请检查AAuraCharacterBase::ApplyEffectToSelf中是否加入EffectContextHandle.AddSourceObject(this);"));
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	
	return 80.f + 2.f * Vigor + 10.f * PlayerLevel;
}
