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
	
	// 이 거리보다 멀어지면 쫓아감.
	UPROPERTY(EditAnywhere, Category = "Settings")
	float AttackRange = 800.0f; 

	float LastFireTime = 0.0f;
};

USTRUCT(meta = (DisplayName = "Attack Target", Category = "NPC Combat"))
struct GEAROFTHREE_API FSTT_AttackTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_AttackTargetInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	
	// 상태에 진입할 때 1번 실행됨
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	// 상태에서 빠져나갈 때 1번 실행됨
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};