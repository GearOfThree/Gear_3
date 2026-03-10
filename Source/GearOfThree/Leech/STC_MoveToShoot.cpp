// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STC_MoveToShoot.h"

#include "Leech/Leech.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

bool USTC_MoveToShoot::TestCondition(FStateTreeExecutionContext& Context) const
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
	bool bInRange = dist <= 1200.f;
	
	float CurrTime = OwnerActor->GetWorld()->GetTimeSeconds();
	bool bCooldown = CurrTime - Leech->LastShootTime >= 5.f;
	
	bool bShootable = bInRange && bCooldown;
	
	return bShootable;
}
