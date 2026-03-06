// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STC_ToDead.h"

#include "Leech/Leech.h"
#include "GameFramework/Actor.h"

bool USTC_ToDead::TestCondition(FStateTreeExecutionContext& Context) const
{
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}

	const ALeech* Leech = Cast<ALeech>(OwnerActor);
	const bool bDead = (Leech && Leech->bIsDeadLeech);

	if (!bHasLast || bDead != bLastDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("[STC_ToDead] %s bDead %d -> %d"),
			*GetNameSafe(OwnerActor),
			bHasLast ? (bLastDead ? 1 : 0) : -1,
			bDead ? 1 : 0);

		bHasLast = true;
		bLastDead = bDead;
	}

	return bDead;
}