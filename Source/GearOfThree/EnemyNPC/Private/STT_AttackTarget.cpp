#include "STT_AttackTarget.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetMathLibrary.h"

// 🚨 에러의 원인: 이 함수가 없거나 시그니처가 달랐을 겁니다.
EStateTreeRunStatus FSTT_AttackTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    ACharacter* Owner = Cast<ACharacter>(Context.GetOwner());
    if (!Owner) return EStateTreeRunStatus::Failed;

    if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    {
        AIC->StopMovement(); 
    }

    if (UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement())
    {
        MoveComp->StopMovementImmediately(); 
        MoveComp->Velocity = FVector::ZeroVector; 
        MoveComp->bOrientRotationToMovement = false;
    }

    Owner->bUseControllerRotationYaw = false;

    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_AttackTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    
    InstanceData.TimeSinceLastUpdate += DeltaTime;
    if (InstanceData.TimeSinceLastUpdate < 0.1f) return EStateTreeRunStatus::Running; 
    InstanceData.TimeSinceLastUpdate = 0.0f;

    ACharacter* Owner = Cast<ACharacter>(Context.GetOwner());
    AActor* Target = InstanceData.TargetActor; 

    if (!Owner || !Target || !IsValid(Target)) return EStateTreeRunStatus::Failed;

    AAIController* AIC = Cast<AAIController>(Owner->GetController());
    UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement();
    if (!AIC || !MoveComp) return EStateTreeRunStatus::Failed;

    float Distance3D = Owner->GetDistanceTo(Target);
    bool bHasLOS = AIC->LineOfSightTo(Target);

    if (Distance3D > InstanceData.AttackRange * 6.0f) return EStateTreeRunStatus::Failed;

    // 이동 로직 (사거리 밖 혹은 시야 없음)
    if (Distance3D > InstanceData.AttackRange || !bHasLOS)
    {
        Owner->bUseControllerRotationYaw = false; 
        MoveComp->bOrientRotationToMovement = true; 

        // 리치가 떠 있어도 그 밑의 바닥을 향해 이동
        FVector TargetGroundLocation = Target->GetActorLocation();
        TargetGroundLocation.Z = Owner->GetActorLocation().Z; 

        AIC->MoveToLocation(TargetGroundLocation, InstanceData.AttackRange * 0.95f); 

        return EStateTreeRunStatus::Running; 
    }
    
    // 사격 로직
    else
    {
        if (AIC->GetMoveStatus() != EPathFollowingStatus::Idle) AIC->StopMovement();
        
        MoveComp->Velocity = FVector::ZeroVector;
        MoveComp->bOrientRotationToMovement = false; 
        Owner->bUseControllerRotationYaw = true; 
        
        // 상하 조준 보정 (Pitch)
        FVector MuzzleLoc = Owner->GetActorLocation() + FVector(0,0,50); 
        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, Target->GetActorLocation());
        AIC->SetControlRotation(LookAtRot);
        AIC->SetFocus(Target);

        if (UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>())
        {
            WeaponComp->Fire(); 
        }
    }

    return EStateTreeRunStatus::Running;
}