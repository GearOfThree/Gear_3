// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_Dead.generated.h"

UCLASS()
class GEAROFTHREE_API USTT_Dead : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

public:
	USTT_Dead(const FObjectInitializer& ObjectInitializer);

	// 몇 초 후 파괴할지 (0이면 즉시)
	UPROPERTY(EditAnywhere, Category="Dead")
	float DestroyDelay = 1.0f;

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	//virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, float DeltaTime) override;
	// Tick 안 씀
	
private:
	float Elapsed = 0.f;
	bool bDestroyRequested = false;
};