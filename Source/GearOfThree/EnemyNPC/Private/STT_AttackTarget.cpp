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
    ACharacter* Owner = Cast<ACharacter>(Context.GetOwner());
    AActor* Target = InstanceData.TargetActor; 

    if (!Owner || !Target) return EStateTreeRunStatus::Failed;

    AAIController* AIC = Cast<AAIController>(Owner->GetController());
    UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement();
    if (!AIC || !MoveComp) return EStateTreeRunStatus::Failed;

    // 1. 적과의 거리 계산
    float Distance = Owner->GetDistanceTo(Target);

    // 사거리 밖이면 적을 향해 뛰어갑니다!
    if (Distance > InstanceData.AttackRange)
    {
        AIC->MoveToActor(Target, InstanceData.AttackRange * 0.8f); // 사거리 안쪽까지 접근
        
        Owner->bUseControllerRotationYaw = false; 
        MoveComp->bOrientRotationToMovement = true; // 뛰는 방향 쳐다보기
        AIC->ClearFocus(EAIFocusPriority::Gameplay); // 조준 풀기
    }
    // 사거리 안에 들어오면 멈춰서 쏩니다!
    else
    {
        // 브레이크 꽉! (문워크 방지)
        if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle) AIC->StopMovement();
        MoveComp->Velocity = FVector::ZeroVector;
        MoveComp->bOrientRotationToMovement = false; 

        // 적 바라보기 (조준)
        Owner->bUseControllerRotationYaw = true; 
        AIC->SetFocus(Target);

        // 사격 명령 (연사 속도 제한은 WeaponComponent가 스스로 알아서 막아줌!)
        if (UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>())
        {
            WeaponComp->Fire(); 
        }
    }

    return EStateTreeRunStatus::Running;
}