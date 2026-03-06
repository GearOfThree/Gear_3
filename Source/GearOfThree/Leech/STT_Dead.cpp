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

	// Orbit 덮어쓰기 차단
	if (ALeech* Leech = Cast<ALeech>(OwnerActor))
	{
		Leech->bOrbiting = false;
		// 재진입 방지: Dead 트리거를 소비(원샷)
		Leech->bIsDeadLeech = false;
	}

	// 재진입해도 딱 1번만 실행되게
	if (!bDestroyRequested)
	{
		bDestroyRequested = true;

		UE_LOG(LogTemp, Warning, TEXT("[STT_Dead] QUEUE DESTROY %s"), *GetNameSafe(OwnerActor));

		if (DestroyDelay <= 0.f)
		{
			OwnerActor->Destroy();
		}
		else
		{
			OwnerActor->SetLifeSpan(DestroyDelay);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[STT_Dead] EnterState %s"), *GetNameSafe(OwnerActor));
	return EStateTreeRunStatus::Running;
}
/*
EStateTreeRunStatus USTT_Dead::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
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

	Elapsed += DeltaTime;

	// 딱 한 번만 Destroy 요청
	if (!bDestroyRequested && Elapsed >= DestroyDelay)
	{
		bDestroyRequested = true;
		UE_LOG(LogTemp, Warning, TEXT("[STT_Dead] Destroy %s"), *GetNameSafe(OwnerActor));
		OwnerActor->Destroy();

		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Running;
}
*/