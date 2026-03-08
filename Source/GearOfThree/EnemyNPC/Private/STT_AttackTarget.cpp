#include "STT_AttackTarget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_AttackTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    ACharacter* Owner = Cast<ACharacter>(Context.GetOwner());
    if (!Owner) return EStateTreeRunStatus::Failed;

    // 물리적 브레이크 (문워크 방지)
    if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    {
        AIC->StopMovement(); 
    }

    if (UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately(); 
        MoveComp->Velocity = FVector::ZeroVector; 
        MoveComp->bOrientRotationToMovement = false; // 뛰는 방향 쳐다보기 끄기
    }

    Owner->bUseControllerRotationYaw = false; // 엔진 강제 회전 끄고, Tick에서 직접 부드럽게 회전시킴

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_AttackTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    
    // 시간 누적
    InstanceData.TimeSinceLastUpdate += DeltaTime;

    // 0.1초(1초에 10번)가 안 지났다면 무거운 연산 스킵!
    if (InstanceData.TimeSinceLastUpdate < 0.1f)
    {
        // 스킵하더라도 실패(Failed)가 아니라 "계속 진행 중(Running)"이라고 트리에 알려줘야 합니다.
        return EStateTreeRunStatus::Running; 
    }

    // 0.1초가 지났다면 타이머 초기화 후 아래 로직 실행
    InstanceData.TimeSinceLastUpdate = 0.0f;

    // =======================================================
    
    ACharacter* Owner = Cast<ACharacter>(Context.GetOwner());
    AActor* Target = InstanceData.TargetActor; 

    if (!Owner || !Target) return EStateTreeRunStatus::Failed;

    AAIController* AIC = Cast<AAIController>(Owner->GetController());
    UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement();
    if (!AIC || !MoveComp) return EStateTreeRunStatus::Failed;

    // 1. 적과의 거리 계산
    float Distance = Owner->GetDistanceTo(Target);

    // 추격으로 돌아가는 기준을 사거리보다 조금 더 멀게(1.2배) 잡습니다.
    // 사거리가 3000이라면, 3600을 벗어날 때까지는 Attack 상태를 유지하며 쫓아갑니다.
    if (Distance > InstanceData.AttackRange * 1.2f) 
    {
        return EStateTreeRunStatus::Failed; // 정말 멀어졌을 때만 Chase로 복귀
    }

    // 사거리 밖이면 적을 향해 뛰어갑니다!
    if (Distance > InstanceData.AttackRange)
    {
        AIC->MoveToActor(Target, InstanceData.AttackRange * 0.8f); 
        
        Owner->bUseControllerRotationYaw = false; 
        MoveComp->bOrientRotationToMovement = true; 
        AIC->ClearFocus(EAIFocusPriority::Gameplay); 
    }
    // 사거리 안에 들어오면 멈춰서 쏩니다!
    else
    {
        if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle) AIC->StopMovement();
        MoveComp->Velocity = FVector::ZeroVector;
        MoveComp->bOrientRotationToMovement = false; 

        Owner->bUseControllerRotationYaw = true; 
        AIC->SetFocus(Target);

        // 엄폐 중이었을 수 있으니 무조건 일어섭니다!
        Owner->UnCrouch();

        // 기어스의 빼꼼 샷: Target이 내 눈에 보이는지(LineOfSight) 확인!
        if (AIC->LineOfSightTo(Target))
        {
            if (UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>())
            {
                WeaponComp->Fire(); 
            }
        }
        else
        {
            // 일어섰는데도 안 보인다면, 타겟이 엄폐물 뒤로 숨었거나 이동한 것입니다.
            // 이럴 때는 다시 추격(Chase) 상태로 돌아가도록 트리에 알립니다.
            return EStateTreeRunStatus::Failed; 
        }
    }

    return EStateTreeRunStatus::Running;
}