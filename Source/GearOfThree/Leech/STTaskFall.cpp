// Fill out your copyright notice in the Description page of Project Settings.

#include "STTaskFall.h"
#include "GameFramework/Actor.h"

EStateTreeRunStatus FSTT_Fall_CPP::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);
	Data.FallVelocity = 0.f;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_Fall_CPP::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());

	if (!OwnerActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	auto& Data = Context.GetInstanceData<FInstanceDataType>(*this);

	Data.FallVelocity += Gravity * DeltaTime;
	Data.FallVelocity = FMath::Max(Data.FallVelocity, TerminalVelocity);

	FVector Next = OwnerActor->GetActorLocation();
	Next.Z += Data.FallVelocity * DeltaTime;

	FHitResult Hit;
	OwnerActor->SetActorLocation(Next, true, &Hit);

	if (Hit.bBlockingHit && Hit.ImpactNormal.Z >= GroundNormalZThreshold)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FallTask] LANDED -> Succeeded (NormalZ=%.2f) Owner=%s"),
		Hit.ImpactNormal.Z,
		*OwnerActor->GetName());
		
		Data.FallVelocity = 0.f;
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}