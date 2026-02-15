#include "C:\Users\PC\Documents\GitHub\Gear_3\Intermediate\Build\Win64\x64\GearOfThreeEditor\Development\UnrealEd\SharedPCH.UnrealEd.Project.ValApi.ValExpApi.Cpp20.h"

#include "STT_FollowPlayer.h"
#include "NPCCharacter.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_FollowPlayer::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	
	// 1. 필요한 컴포넌트 및 플레이어 가져오기
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return EStateTreeRunStatus::Failed;

	AAIController* AIC = Cast<AAIController>(Owner->GetController());
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);

	if (!AIC || !Player) return EStateTreeRunStatus::Failed;

	// 2. 플레이어와의 거리 계산
	float Distance = Owner->GetDistanceTo(Player);

	// 3. 거리별 행동 결정
	if (Distance > InstanceData.FollowRadius)
	{
		// 플레이어가 멀어지면 추적 시작
		// StopRadius 근처까지 이동하도록 설정
		AIC->MoveToActor(Player, InstanceData.StopRadius);
	}
	else if (Distance <= InstanceData.StopRadius)
	{
		// 충분히 가까워지면 이동 중지
		// 현재 이동 중이라면 멈추게 함
		if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle)
		{
			AIC->StopMovement();
		}
	}

	// 이 태스크는 '따라가기' 상태를 계속 유지해야 하므로 Running을 반환합니다.
	// (전투 상태 등으로 전환되는 것은 State Tree의 Transition에서 담당합니다.)
	return EStateTreeRunStatus::Running;
}
