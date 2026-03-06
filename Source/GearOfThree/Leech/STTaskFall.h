// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h" 
#include "STTaskFall.generated.h"

// 중첩 금지라서 밖으로 빼기
USTRUCT()
struct GEAROFTHREE_API FSTT_Fall_InstanceData
{
	GENERATED_BODY()

	UPROPERTY()
	float FallVelocity = 0.f;
};

USTRUCT()
struct GEAROFTHREE_API FSTT_Fall_CPP : public FStateTreeTaskBase
{
	GENERATED_BODY()

	FSTT_Fall_CPP()
	{
		bShouldCallTick = true;              // 매 프레임 Tick 호출
		bShouldCallTickOnlyOnEvents = false; // 이벤트 있을 때만 Tick 같은 모드 방지
	}
public:
	UPROPERTY(EditAnywhere, Category="Fall")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, Category="Fall")
	float TerminalVelocity = -4000.f;

	UPROPERTY(EditAnywhere, Category="Fall")
	float GroundNormalZThreshold = 0.6f;

	using FInstanceDataType = FSTT_Fall_InstanceData;

	// ✅ InstanceData 타입 알려주기
	virtual const UStruct* GetInstanceDataType() const override
	{
		return FSTT_Fall_InstanceData::StaticStruct();
	}

	// ✅ 대부분의 버전에서 const가 맞음 (override 실패 방지)
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};