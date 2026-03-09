#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_CoverTarget.generated.h"

USTRUCT()
struct GEAROFTHREE_API FSTT_CoverTargetInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Parameter")
	class UEnvQuery* CoverQuery = nullptr;

	UPROPERTY(EditAnywhere, Category = "Parameter")
	float TimeSinceLastUpdate = 0.f;

	UPROPERTY()
	bool bHasStartedMoving = false;

	UPROPERTY()
	float TotalTimeInState = 0.f;
};

USTRUCT(meta = (DisplayName = "Cover Target Task", Category = "AI|Custom"))
struct GEAROFTHREE_API FSTT_CoverTarget : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_CoverTargetInstanceData;

protected:
	// 🚨 템플릿 대신 이 함수를 통해 데이터 구조체를 명시적으로 연결합니다.
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};