// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STT_Fall.h"

#include "Components/StateTreeComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagsManager.h"
#include "Leech.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Actor.h"

USTT_Fall::USTT_Fall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}

EStateTreeRunStatus USTT_Fall::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	// Tick이 안 와도 공전 덮어쓰기를 바로 끊기 위해 Enter에서 끊는다
	if (ALeech* Leech = Cast<ALeech>(Context.GetOwner()))
	{
		Leech->bOrbiting = false;
		UE_LOG(LogTemp, Warning, TEXT("[ST] STT_Fall Enter -> bOrbiting=false (%s)"), *Leech->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ST] STT_Fall Enter (Owner=%s)"), *GetNameSafe(Context.GetOwner()));
	}
	
	FallVelocity = 0.f;
	UE_LOG(LogTemp, Warning, TEXT("[ST] STT_Fall Enter"));
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTT_Fall::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
{
	if (bLanded)
	{
		return EStateTreeRunStatus::Running;
	}
	
	UObject* OwnerObj = Context.GetOwner();
	AActor* OwnerActor = Cast<AActor>(OwnerObj);
	UE_LOG(LogTemp, Warning, TEXT("Tick 돌고 있음"));
	if (!OwnerActor)
	{
		// owner가 actor가 아닐 수도 있어서, 컴포넌트면 outer로 한번 더 타고 올라감
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}
	
	if (!OwnerActor)
	{
		return EStateTreeRunStatus::Failed;
	}

	// 중력 적분
	FallVelocity += Gravity * DeltaTime;
	FallVelocity = FMath::Max(FallVelocity, TerminalVelocity);

	const FVector Curr = OwnerActor->GetActorLocation();
	FVector Next = Curr;
	Next.Z += FallVelocity * DeltaTime;

	FHitResult Hit;
	OwnerActor->SetActorLocation(Next, true, &Hit); // Sweep = true

	if (Hit.bBlockingHit && Hit.ImpactNormal.Z >= GroundNormalZThreshold)
	{
		FallVelocity = 0.f;
		bLanded = true; // 다음 틱부터는 중력/이동 안 함
		OwnerActor->AddActorWorldOffset(FVector(0, 0, Hit.Location.Z + 30), false);

		if (UStateTreeComponent* STC = OwnerActor->FindComponentByClass<UStateTreeComponent>())
		{
			const FGameplayTag LandedTag =
				FGameplayTag::RequestGameplayTag(FName("Event.Leech.Landed"));

			STC->SendStateTreeEvent(LandedTag);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("[ST] STT_Fall Succeeded (Landed)"));
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Running;
}

