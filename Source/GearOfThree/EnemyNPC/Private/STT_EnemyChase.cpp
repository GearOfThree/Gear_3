#include "STT_EnemyChase.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "AIController.h"
#include "NPCCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_EnemyChase::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	//추격 상태 진입 (시작하자마자 로그부터 찍게)
	UE_LOG(LogTemp, Log, TEXT("🏃‍♂️ [STATE] Enter Chase State"));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("🏃‍♂️ [Scion] Entering Chase State"));

	// 데이터 가져오기
	const FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// 액터 캐스팅 (Tick 함수처럼 깔끔하게 ANPCCharacter로 캐스팅)
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return EStateTreeRunStatus::Failed;

	AAIController* AIC = Cast<AAIController>(Owner->GetController());
    
	//플레이어가 아니라, StateTree에서 바인딩해 준 'TargetActor'를 가져옵니다.
	AActor* Target = InstanceData.TargetActor; 

	if (AIC && Target)
	{
		//이전 상태의 조준(시선 고정)을 풀어줍니다
		AIC->ClearFocus(EAIFocusPriority::Gameplay);
		Owner->bUseControllerRotationYaw = false; 
		if (Owner->GetCharacterMovement())
		{
			Owner->GetCharacterMovement()->bOrientRotationToMovement = true; // 이동하는 방향 쳐다보기
		}

		// 타겟을 향해 이동 명령!
		AIC->MoveToActor(Target, InstanceData.AttackRange * 0.8f);
		return EStateTreeRunStatus::Running;
	}
    
	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_EnemyChase::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	AAIController* AIC = Cast<AAIController>(Owner->GetController());

	// 타겟 사용
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