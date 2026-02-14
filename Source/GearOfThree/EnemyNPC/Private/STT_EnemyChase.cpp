#include "STT_EnemyChase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "AIController.h"
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
	AActor* Owner = Cast<AActor>(Context.GetOwner());
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);

	if (!Owner || !Player) return EStateTreeRunStatus::Failed;

	AAIController* AIC = Cast<AAIController>(Cast<APawn>(Owner)->GetController());
	if (!AIC) return EStateTreeRunStatus::Failed;

	float Distance = Owner->GetDistanceTo(Player);

	// 1. 공격 사거리 안에 들어왔는지 체크
	if (Distance <= InstanceData.AttackRange)
	{
		AIC->StopMovement();
		UE_LOG(LogTemp, Log, TEXT("=== [CHASE] Target Reached. Switching to Fire. ==="));
		return EStateTreeRunStatus::Succeeded; // 공격 상태로 전환
	}

	// 2. 이동 명령 보강 (주기적으로 업데이트)
	// AI의 현재 이동 상태를 확인하고, 멈춰있거나 타겟이 멀면 다시 명령을 내림
	EPathFollowingStatus::Type MoveStatus = AIC->GetMoveStatus();
	if (MoveStatus == EPathFollowingStatus::Idle)
	{
		// 멈춰있는 경우 다시 추격 시작
		AIC->MoveToActor(Player, InstanceData.AttackRange * 0.8f);
		UE_LOG(LogTemp, Log, TEXT("=== [CHASE] Restarting Movement to Player ==="));
	}

	return EStateTreeRunStatus::Running;
}