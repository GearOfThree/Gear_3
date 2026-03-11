#include "STE_TargetSelector.h"
#include "NPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"

void FSTE_TargetSelector::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	
	// 흘러간 시간(DeltaTime)을 누적합니다.
	InstanceData.TimeSinceLastUpdate += DeltaTime;

	// 누적된 시간이 1초(1.0f)가 안 되었다면? 
	// 무거운 탐색 연산을 하지 않고 그냥 함수를 종료(return)합니다!
	if (InstanceData.TimeSinceLastUpdate < 1.0f)
	{
		return; 
	}
	// 1초가 지났다면, 다음 1초를 세기 위해 타이머를 다시 0으로 돌려놓습니다.
	InstanceData.TimeSinceLastUpdate = 0.0f;
	
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);
	AActor* FinalTarget = nullptr;

	// 적군인지 아군인지 분기
	if (Owner->TeamSide == ETeamSide::Enemy)
	{
		// 적군 로직
		float DistanceToPlayer = Owner->GetDistanceTo(Player);
		if (DistanceToPlayer <= InstanceData.PlayerPriorityRange)
		{
			FinalTarget = Player; // 플레이어가 가까우면 플레이어 공격
		}
		else
		{
			FinalTarget = Owner->FindClosestEnemy(); // 멀면 근처 아군 NPC 검색
			if (!FinalTarget) FinalTarget = Player;
		}
	}
	else if (Owner->TeamSide == ETeamSide::Ally)
	{
		// 아군 로직
		FinalTarget = Owner->FindClosestEnemy(); 
		// 적군이 하나도 없다면? FinalTarget은 nullptr가 됨
	}
	UE_LOG(LogTemp, Error, TEXT("Final Target: %s"), *GetNameSafe(FinalTarget));

	//Evaluator 자체 출력 데이터 업데이트 (State Tree 내부 바인딩용)
	InstanceData.SelectedTarget = FinalTarget;

	//주인 캐릭터의 변수 업데이트 (무기 조준용)
	Owner->SetCurrentTargetActor(FinalTarget);
	
}