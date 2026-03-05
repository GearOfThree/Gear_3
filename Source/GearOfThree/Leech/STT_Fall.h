// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_Fall.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API USTT_Fall : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

	
public:
	USTT_Fall(const FObjectInitializer& ObjectInitializer); // 생성자 추가
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fall")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fall")
	float TerminalVelocity = -4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fall")
	float GroundNormalZThreshold = 0.6f;

private:
	UPROPERTY(Transient)
	float FallVelocity = 0.f;

public:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;
};
