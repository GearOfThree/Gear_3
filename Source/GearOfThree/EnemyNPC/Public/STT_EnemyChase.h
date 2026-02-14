// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_EnemyChase.generated.h"

// [1] 데이터 전용 구조체 정의 (여기에 변수를 넣습니다)
USTRUCT()
struct GEAROFTHREE_API FSTT_EnemyChaseInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float AttackRange = 500.0f;
	
	UPROPERTY(EditAnywhere, Category = "Context")
	AActor* TargetActor = nullptr;
};

// [2] 태스크 정의
USTRUCT(meta = (DisplayName = "Enemy Chase Player", Category = "Enemy AI"))
struct GEAROFTHREE_API FSTT_EnemyChase : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	// [3] 이 태스크는 위의 데이터 구조체를 사용한다고 선언
	using FInstanceDataType = FSTT_EnemyChaseInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};