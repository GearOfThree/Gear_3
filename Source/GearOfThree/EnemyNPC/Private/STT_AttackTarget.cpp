#include "STT_AttackTarget.h"

#include "AIController.h"
#include "NPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_AttackTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	if (!Owner) return EStateTreeRunStatus::Failed;

	AAIController* AIC = Cast<AAIController>(Owner->GetController());
	AActor* Target = Owner->GetCurrentTarget();

	// 타겟이 없거나 죽었으면 태스크 종료 (Transition이 처리하겠지만 안전장치)
	if (!AIC || !Target) return EStateTreeRunStatus::Succeeded;

	// 1. 거리 계산
	float DistanceToTarget = Owner->GetDistanceTo(Target);

	// 2. [상황 A] 적이 사거리보다 멀리 있다? -> "추격 모드"
	if (DistanceToTarget > InstanceData.AttackRange)
	{
		// 적을 향해 이동 (AttackRange의 90% 지점까지만 이동해서 멈추도록 설정)
		AIC->MoveToActor(Target, InstanceData.AttackRange * 0.9f);
        
		// 이동 중에는 조준보다는 이동 방향을 보는 게 자연스러울 수 있음 (취향 차이)
		Owner->bUseControllerRotationYaw = false; 
		if (Owner->GetCharacterMovement())
			Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	// 3. [상황 B] 적이 사거리 안에 들어왔다? -> "사격 모드"
	else
	{
		// 이동 멈춰! (미세하게 움직이는 것 방지)
		if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle)
		{
			AIC->StopMovement();
		}

		// 적을 바라보게 설정 (조준)
		AIC->SetFocus(Target); 
		Owner->bUseControllerRotationYaw = true; // 몸통 회전

		// --- 발사 로직 ---
		float CurrentTime = Context.GetWorld()->GetTimeSeconds();
		if (CurrentTime - InstanceData.LastFireTime >= InstanceData.FireRate)
		{
			if (UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>())
			{
				WeaponComp->Fire();
                
				// [디버그 로그] 쏘고 있다는 걸 확실히 확인
				UE_LOG(LogTemp, Warning, TEXT("🔥 [Attack] Firing at %s! Distance: %f"), *Target->GetName(), DistanceToTarget);
                
				InstanceData.LastFireTime = CurrentTime;
			}
		}
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_AttackTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 화면에 빨간색으로 "Battle Start" 출력
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("🔴 [State] Battle Mode : START !!!"));
    
	UE_LOG(LogTemp, Warning, TEXT("State Tree: Enter Battle Mode"));

	return EStateTreeRunStatus::Running;
}

void FSTT_AttackTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	// 전투가 끝날 때 (적이 죽었을 때)
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("⚪ [State] Battle Mode : END"));
}