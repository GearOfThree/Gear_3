// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STC_RushToMove.h"

#include "Leech.h"
#include "StateTreeExecutionContext.h"

bool USTC_RushToMove::TestCondition(FStateTreeExecutionContext& Context) const
{
	// OwnerActor 구하기
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}
	const ALeech* Leech = Cast<ALeech>(OwnerActor);
	if (!Leech || !OwnerActor->GetWorld())
	{
		return false;
	}
	
	return !Leech->bIsRush;
}
