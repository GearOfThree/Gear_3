// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_Move.generated.h"

class ALeech;

UCLASS()
class GEAROFTHREE_API USTT_Move : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	USTT_Move(const FObjectInitializer& ObjectInitializer); // 생성자 추가
	
	// 겹침 밀어내기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Separation")
	float SeparationStrength = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Separation")
	bool bSeparationSweep = false;

	// 라인트레이스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Ground")
	float GroundCheckDistance = 100.f;

	// 공중이면 중력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Falling")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Falling")
	float TerminalVelocity = -4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Falling")
	float GroundNormalZThreshold = 0.6f;

	// 플레이어 추적 이동
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Chase")
	float MoveSpeed = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Move|Chase")
	float TurnRate = 10.f;

private:
	UPROPERTY(Transient)
	float FallVelocity = 0.f;

	UPROPERTY(Transient)
	FVector Velocity = FVector::ZeroVector;

public:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;
};