// Author: sumikiru ,learning after Druid Mechanics' Course.


#include "Characters/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	//CharacterMovement在ABP事件蓝图初始化动画中创建
	GetCharacterMovement()->bOrientRotationToMovement = true;//将旋转朝向运动
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;//约束到平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true;//开始时与平面对齐

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}
