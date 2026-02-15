#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "STT_FollowPlayer.generated.h"

USTRUCT()
struct GEAROFTHREE_API FSTT_FollowPlayerInstanceData
{
	GENERATED_BODY()

	// 플레이어를 추적하기 시작하는 거리
	UPROPERTY(EditAnywhere, Category = "Settings")
	float FollowRadius = 400.0f;

	// 플레이어에게 도달했다고 판단하여 멈추는 거리
	UPROPERTY(EditAnywhere, Category = "Settings")
	float StopRadius = 200.0f;
};

USTRUCT(meta = (DisplayName = "Follow Player", Category = "Ally NPC"))
struct GEAROFTHREE_API FSTT_FollowPlayer : public FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTT_FollowPlayerInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	// 매 프레임 플레이어와의 거리를 체크하여 이동 명령을 내립니다.
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
};