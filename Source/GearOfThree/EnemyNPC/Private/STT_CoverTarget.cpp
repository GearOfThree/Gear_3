#include "STT_CoverTarget.h"
#include "GearAIController.h"
#include "NPCCharacter.h"
#include "GameFramework/Character.h"
#include "StateTreeExecutionContext.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus FSTT_CoverTarget::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{

    ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
    if (!Owner)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ FAIL: Owner is NOT ANPCCharacter!"));
        return EStateTreeRunStatus::Failed;
    }

    AGearAIController* GearAIC = Cast<AGearAIController>(Owner->GetController());
    if (!GearAIC) { 
        UE_LOG(LogTemp, Error, TEXT("❌ FAIL: GearAIC Cast Failed! Current: %s"), 
            Owner->GetController() ? *Owner->GetController()->GetName() : TEXT("NULL")); 
        return EStateTreeRunStatus::Failed; 
    }

    const FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    if (!InstanceData.CoverQuery)
    {
        UE_LOG(LogTemp, Error, TEXT("❌ FAIL: CoverQuery Asset is MISSING!"));
        return EStateTreeRunStatus::Failed;
    }

    GearAIC->FindCoverLocation(InstanceData.CoverQuery);
    
    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_CoverTarget::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    InstanceData.TimeSinceLastUpdate += DeltaTime;
    InstanceData.TotalTimeInState += DeltaTime;

    if (InstanceData.TimeSinceLastUpdate < 0.1f) return EStateTreeRunStatus::Running;
    InstanceData.TimeSinceLastUpdate = 0.0f;

    ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
    if (!Owner) return EStateTreeRunStatus::Failed;

    AGearAIController* GearAIC = Cast<AGearAIController>(Owner->GetController());
    if (!GearAIC) return EStateTreeRunStatus::Failed;

    // 이동 시작 및 완료 체크 로직
    if (!InstanceData.bHasStartedMoving)
    {
        if (GearAIC->GetMoveStatus() != EPathFollowingStatus::Idle)
        {
            InstanceData.bHasStartedMoving = true;
        }
        else if (InstanceData.TotalTimeInState > 2.0f)
        {
            return EStateTreeRunStatus::Failed;
        }
    }
    else if (GearAIC->GetMoveStatus() == EPathFollowingStatus::Idle)
    {
        if (!Owner->bIsCrouched) 
        {
            UE_LOG(LogTemp, Log, TEXT("✅ Cover Reached: Doing Crouch"));
            Owner->Crouch();
        }
    
        // 🚨 바로 Succeeded를 리턴하지 말고 Running을 유지합니다.
        // 이렇게 해야 ExitState가 호출되지 않아 앉은 상태가 유지됩니다.
        return EStateTreeRunStatus::Running; 
    }

    return EStateTreeRunStatus::Running;
}

void FSTT_CoverTarget::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    if (ACharacter* Owner = Cast<ACharacter>(Context.GetOwner()))
    {
        Owner->UnCrouch();
    }
}