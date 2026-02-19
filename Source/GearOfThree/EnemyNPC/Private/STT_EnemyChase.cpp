#include "STT_EnemyChase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "AIController.h"
#include "NPCCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_EnemyChase::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	//데이터 가져오기
	const FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	AActor* Owner = Cast<AActor>(Context.GetOwner());
	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner) return EStateTreeRunStatus::Failed;

	AAIController* AIController = Cast<AAIController>(PawnOwner->GetController());
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);

	if (AIController && Player)
	{
		// InstanceData.AttackRange 로 접근!
		AIController->MoveToActor(Player, InstanceData.AttackRange * 0.8f);
		return EStateTreeRunStatus::Running;
	}
	
	// [로그] 추격 상태 진입
	UE_LOG(LogTemp, Log, TEXT("=== [STATE] Enter ChaseState ==="));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Entering Chase State"));
	
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_EnemyChase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	AAIController* AIC = Cast<AAIController>(Owner->GetController());

	// [변경] 타겟 사용
	AActor* Target = InstanceData.TargetActor; 

	if (!Owner || !AIC || !Target) return EStateTreeRunStatus::Failed;

	float Distance = Owner->GetDistanceTo(Target);

	// 1. 공격 범위 안이면 공격 상태로 전환
	if (Distance <= InstanceData.AttackRange)
	{
		AIC->StopMovement();
		return EStateTreeRunStatus::Succeeded;
	}

	// 2. 주기적인 이동 명령 갱신
	// 타겟이 계속 움직이므로, AI의 이동 상태가 Idle이 되면 다시 타겟을 향해 뜁니다.
	if (AIC->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		AIC->MoveToActor(Target, InstanceData.AttackRange * 0.9f);
	}

	return EStateTreeRunStatus::Running;
}