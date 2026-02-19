// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_EnemyFire.generated.h"

// ... includes ...

USTRUCT()
struct GEAROFTHREE_API FSTT_EnemyFireInstanceData
{
	GENERATED_BODY()
	
	// [설정] 추격 정지 거리(500)보다 조금 더 길게 잡으세요 (예: 600)
	// 그래야 경계선에서 버벅거리지 않습니다.
	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackRange = 600.0f; 

	UPROPERTY(EditAnywhere, Category = "Combat")
	float FireInterval = 0.5f; // 연사 속도

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ReloadTime = 2.0f;   // 장전 시간

	UPROPERTY(EditAnywhere, Category = "Context")
	AActor* TargetActor = nullptr;
	
	// [내부 타이머]
	float TimeSinceLastFire = 0.0f;
	float CurrentReloadTime = 0.0f;
};

USTRUCT(meta = (DisplayName = "Enemy Fire", Category = "Enemy AI"))
struct GEAROFTHREE_API FSTT_EnemyFire : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_EnemyFireInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};
