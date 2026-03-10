// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STC_MoveToRush.h"

#include "Leech.h"
#include "Kismet/GameplayStatics.h"

bool USTC_MoveToRush::TestCondition(FStateTreeExecutionContext& Context) const
{	
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
	FVector MyLoc = Leech->GetActorLocation();
	
	APawn* target = UGameplayStatics::GetPlayerPawn(OwnerActor, 0);
	if (!target)
	{
		return false;
	}
	FVector targetLoc = target->GetActorLocation();
	
	float dist = FVector::Dist(MyLoc, targetLoc);
	bool bInRange = dist <= 200.f;
	
	float CurrTime = OwnerActor->GetWorld()->GetTimeSeconds();
	bool bCooldown = CurrTime - Leech->LastRushTime >= 3.f;
	
	bool bRushable = bInRange && bCooldown;
	
	return bRushable;
}

