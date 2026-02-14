#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"
#include "STE_TargetSelector.generated.h"

USTRUCT()
struct GEAROFTHREE_API FSTE_TargetSelectorInstanceData
{
	GENERATED_BODY()

	// 플레이어가 이 거리(cm) 안에 들어오면 최우선 타겟이 됨
	UPROPERTY(EditAnywhere, Category = "Settings")
	float PlayerPriorityRange = 800.0f;

	// 최종 선정된 타겟 (태스크에 연결될 값)
	UPROPERTY(EditAnywhere, Category = "Output")
	AActor* SelectedTarget = nullptr;
};

USTRUCT(meta = (DisplayName = "Target Selector", Category = "Enemy AI"))
struct GEAROFTHREE_API FSTE_TargetSelector : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()
	using FInstanceDataType = FSTE_TargetSelectorInstanceData;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
    
	virtual void Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};