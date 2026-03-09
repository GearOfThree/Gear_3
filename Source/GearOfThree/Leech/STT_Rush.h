// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_Rush.generated.h"

UCLASS()
class GEAROFTHREE_API USTT_Rush : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	USTT_Rush(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
	float RushHorizontalSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
	float RushUpSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
	float Gravity = -980.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rush")
	float RushDuration = 0.8f;

protected:
	float ElapsedTime = 0.f;
	FVector RushVelocity = FVector::ZeroVector;
	bool bStartedRush = false;

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime) override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
};