// Fill out your copyright notice in the Description page of Project Settings.

#include "Leech/STT_Rush.h"

#include "Kismet/GameplayStatics.h"
#include "Leech.h"
#include "AIDebugComponent.h"
#include "StateTreeExecutionContext.h"

USTT_Rush::USTT_Rush(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}

EStateTreeRunStatus USTT_Rush::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);

	ElapsedTime = 0.f;
	bStartedRush = false;
	RushVelocity = FVector::ZeroVector;

	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}

	ALeech* Leech = Cast<ALeech>(OwnerActor);
	if (!Leech || !Leech->GetWorld())
	{
		return EStateTreeRunStatus::Failed;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Leech, 0);
	if (!PlayerPawn)
	{
		return EStateTreeRunStatus::Failed;
	}

	FVector Dir = PlayerPawn->GetActorLocation() - Leech->GetActorLocation();
	Dir.Z = 0.f;
	Dir = Dir.GetSafeNormal();

	if (Dir.IsNearlyZero())
	{
		return EStateTreeRunStatus::Failed;
	}

	Leech->bIsRush = true;
	Leech->bRushShouldEnd = false;
	Leech->bRushHitPlayer = false;
	
	if (Leech->DebugComp)
	{
		Leech->DebugComp->SetCurrentState(TEXT("Rush"), TEXT("Player Targeted"));
	}

	Leech->SetActorRotation(Dir.Rotation());

	// 초기 속도 = 수평 방향 속도 + 위쪽 속도
	RushVelocity = Dir * RushHorizontalSpeed;
	RushVelocity.Z = RushUpSpeed;

	bStartedRush = true;

	UE_LOG(LogTemp, Warning, TEXT("[Rush Enter] RushVelocity=%s"), *RushVelocity.ToString());

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTT_Rush::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime)
{
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}

	ALeech* Leech = Cast<ALeech>(OwnerActor);
	if (!Leech || !bStartedRush)
	{
		if (Leech)
		{
			Leech->bIsRush = false;
		}
		return EStateTreeRunStatus::Failed;
	}
	
	if (Leech->bRushShouldEnd)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Rush Tick] RushShouldEnd=true | HitPlayer=%s"),
			Leech->bRushHitPlayer ? TEXT("true") : TEXT("false"));

		Leech->bIsRush = false;
		Leech->bRushShouldEnd = false;

		if (Leech->bRushHitPlayer)
		{
			Leech->bRushHitPlayer = false;
			Leech->Destroy();
			return EStateTreeRunStatus::Succeeded;
		}

		Leech->bRushHitPlayer = false;
		return EStateTreeRunStatus::Succeeded;
	}

	ElapsedTime += DeltaTime;

	// 중력 적용
	RushVelocity.Z += Gravity * DeltaTime;

	const FVector CurrentLocation = Leech->GetActorLocation();
	const FVector NextLocation = CurrentLocation + RushVelocity * DeltaTime;

	FHitResult MoveHit;
	const bool bMoved = Leech->SetActorLocation(NextLocation, true, &MoveHit);

	if (MoveHit.bBlockingHit)
	{
		AActor* HitActor = MoveHit.GetActor();

		if (HitActor)
		{
			APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(Leech, 0);

			if (HitActor == PlayerPawn)
			{
				IMS_Damageable::Execute_ReceiveDamage(PlayerPawn, 50.f, Leech);
				if (Leech->DeathBombEffect)
				{
					const FVector SpawnLoc = Leech->GetActorLocation() + FVector(0.f, 0.f, 30.f);

					UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						Leech->DeathBombEffect,
						SpawnLoc,
						Leech->GetActorRotation(),
						true
					);
				}
				Leech->Destroy();
			}
		}

		return EStateTreeRunStatus::Succeeded;
	}

	// 시간 다 되면 종료
	if (ElapsedTime >= RushDuration)
	{
		Leech->bIsRush = false;
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void USTT_Rush::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}

	if (ALeech* Leech = Cast<ALeech>(OwnerActor))
	{
		Leech->bIsRush = false;
		Leech->bRushShouldEnd = false;
		Leech->bRushHitPlayer = false;
	}

	ElapsedTime = 0.f;
	RushVelocity = FVector::ZeroVector;
	bStartedRush = false;
}