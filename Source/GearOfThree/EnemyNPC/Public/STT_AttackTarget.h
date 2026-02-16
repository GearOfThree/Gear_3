#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_AttackTarget.generated.h"

USTRUCT()
struct GEAROFTHREE_API FSTT_AttackTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Settings")
	float FireRate = 0.5f; // 발사 간격

	float LastFireTime = 0.0f;
};

USTRUCT(meta = (DisplayName = "Attack Target", Category = "NPC Combat"))
struct GEAROFTHREE_API FSTT_AttackTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_AttackTargetInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};