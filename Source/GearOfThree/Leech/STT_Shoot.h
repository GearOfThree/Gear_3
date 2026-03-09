// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "STT_Shoot.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API USTT_Shoot : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
public:
	USTT_Shoot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;

public:
	// 스폰할 투사체 클래스 (BP_Projectile 등)
	UPROPERTY(EditAnywhere, Category="Shoot")
	TSubclassOf<AActor> ProjectileFactory;

	// 총구/발사 위치 오프셋(리치 기준 로컬 오프셋)
	UPROPERTY(EditAnywhere, Category="Shoot")
	FVector MuzzleLocalOffset = FVector(50.f, 0.f, 60.f);
};
