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
		// 아군은 플레이어를 공격하면 안 됨! 오직 '적군 팀'만 찾음
		FinalTarget = Owner->FindClosestEnemy(); 
        
		// 적군이 하나도 없다면? FinalTarget은 nullptr가 됨
		
		// 디버그 로그 추가
		if (FinalTarget)
		{
			// 적을 찾았을 때: 빨간색 로그
			GEngine->AddOnScreenDebugMessage(10, 0.0f, FColor::Red, 
				FString::Printf(TEXT("👁️ [Eyes] Found Enemy: %s"), *FinalTarget->GetName()));
		}
		else
		{
			// 적을 못 찾았을 때: 노란색 로그
			GEngine->AddOnScreenDebugMessage(10, 0.0f, FColor::Yellow, 
				TEXT("👁️ [Eyes] No Enemy Found... Looking at Player"));
		}
	}
	
	//Evaluator 자체 출력 데이터 업데이트 (State Tree 내부 바인딩용)
	InstanceData.SelectedTarget = FinalTarget;

	//주인 캐릭터의 변수 업데이트 (무기 조준용)
	Owner->SetCurrentTargetActor(FinalTarget);
	
}