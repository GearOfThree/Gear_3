#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_AttackTarget.generated.h"

USTRUCT()
struct GEAROFTHREE_API FSTT_AttackTargetInstanceData
{
	GENERATED_BODY()

	// 뇌가 누구를 쏠지 알아야 하니, 타겟 변수를 받습니다.
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<AActor> TargetActor = nullptr;

	UPROPERTY(EditAnywhere, Category = "Settings")
	float AttackRange = 1000.0f; 
};

USTRUCT(meta = (DisplayName = "Attack Target Task"))
struct GEAROFTHREE_API FSTT_AttackTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_AttackTargetInstanceData;

protected:
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};