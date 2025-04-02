// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252
#define ECC_Projectile ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Target ECollisionChannel::ECC_GameTraceChannel2
// 比如角色的胶囊体，模型，武器等，还有相机上的碰撞，都设置ExcludeCharacters为忽略
#define ECC_ExcludeCharacters ECollisionChannel::ECC_GameTraceChannel3	// 技能范围选择时的通道，忽略场景中的Character
