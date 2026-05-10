// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "AIDebugComponent.generated.h"

class UStateTreeComponent;
class UStateTreeAIComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAROFTHREE_API UAIDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAIDebugComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// --- Configuration ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
	bool bEnableDebug = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
	bool bVisualizeStateTree = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
	bool bVisualizeEQS = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
	bool bVisualizeAnalysis = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Debug")
	FColor DefaultDebugColor = FColor::Cyan;

	// --- State Data ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Debug|State")
	FString CurrentState = TEXT("Unknown");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Debug|State")
	FString TransitionReason;

	UFUNCTION(BlueprintCallable, Category = "AI Debug|State")
	void SetCurrentState(const FString& NewState, const FString& Reason = TEXT(""));

	// --- EQS Data ---
	void VisualizeEQSResults(const TSharedPtr<struct FEnvQueryResult>& Result);

	// --- Analysis Data ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Debug|Analysis")
	float PlayerDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Debug|Analysis")
	bool bHasLOS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Debug|Analysis")
	bool bCanAttack;

private:
	void UpdateAIAnalysis();
	void DrawDebugInfo();
	void UpdateStateTreeInfo();

	FColor GetStateColor() const;

	TWeakObjectPtr<UStateTreeComponent> CachedStateTreeComp;
	TWeakObjectPtr<UStateTreeAIComponent> CachedStateTreeAIComp;

	// Performance metrics
	float LastStateUpdateDelta = 0.0f;
	int32 StateEvaluationCount = 0;
};
