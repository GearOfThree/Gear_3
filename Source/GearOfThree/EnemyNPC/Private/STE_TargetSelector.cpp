#include "STE_TargetSelector.h"
#include "NPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"

void FSTE_TargetSelector::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);
	AActor* FinalTarget = nullptr;

	if (Player)
	{
		float DistanceToPlayer = Owner->GetDistanceTo(Player);

		// [우선순위 1] 플레이어가 사거리 안에 있는가?
		if (DistanceToPlayer <= InstanceData.PlayerPriorityRange)
		{
			FinalTarget = Player;
		}
		else
		{
			// [우선순위 2] 플레이어가 멀다면 가장 가까운 다른 팀 NPC 검색
			FinalTarget = Owner->FindClosestEnemy();
            
			// [예외] 주변에 아무 적도 없다면, 멀리 있는 플레이어라도 주시
			if (!FinalTarget) FinalTarget = Player;
		}
	}

	InstanceData.SelectedTarget = FinalTarget;
}