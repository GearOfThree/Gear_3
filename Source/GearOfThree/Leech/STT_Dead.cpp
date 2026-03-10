// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STT_Dead.h"

#include "StateTreeExecutionContext.h"
#include "Leech/Leech.h"
#include "GameFramework/Actor.h"

USTT_Dead::USTT_Dead(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}

EStateTreeRunStatus USTT_Dead::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}
	if (!OwnerActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	ALeech* Leech = Cast<ALeech>(OwnerActor);
	if (Leech)
	{
		Leech->bOrbiting = false;
		Leech->bIsDeadLeech = false;
	}

	if (!bDestroyRequested)
	{
		bDestroyRequested = true;

		UE_LOG(LogTemp, Warning, TEXT("[STT_Dead] QUEUE DESTROY %s"), *GetNameSafe(OwnerActor));

		Leech->Die();
	}

	UE_LOG(LogTemp, Warning, TEXT("[STT_Dead] EnterState %s"), *GetNameSafe(OwnerActor));
	return EStateTreeRunStatus::Running;
}