// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STT_Move.h"

#include "StateTreeExecutionContext.h"
#include "Leech/Leech.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"


USTT_Move::USTT_Move(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}

EStateTreeRunStatus USTT_Move::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	FallVelocity = 0.f;
	Velocity = FVector::ZeroVector;

	// Orbit 덮어쓰기 끊기
	if (AActor* OwnerActor = Cast<AActor>(Context.GetOwner()))
	{
		if (ALeech* Leech = Cast<ALeech>(OwnerActor))
		{
			Leech->bOrbiting = false;
		}
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus USTT_Move::Tick(FStateTreeExecutionContext& Context, float DeltaTime)
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

	ALeech* SelfLeech = Cast<ALeech>(OwnerActor);

	// Overlap된 리치들로부터 수평으로 밀어내기
	{
		TArray<AActor*> Overlaps;
		OwnerActor->GetOverlappingActors(Overlaps, ALeech::StaticClass());

		const FVector SelfLoc = OwnerActor->GetActorLocation();

		for (AActor* Other : Overlaps)
		{
			if (!Other || Other == OwnerActor) continue;

			FVector Delta = SelfLoc - Other->GetActorLocation();
			Delta.Z = 0.f;

			const float DistSq = Delta.SizeSquared();
			if (DistSq < KINDA_SMALL_NUMBER) continue;

			const FVector PushDir = Delta.GetSafeNormal();
			const FVector Push = PushDir * SeparationStrength * DeltaTime;

			OwnerActor->AddActorWorldOffset(Push, bSeparationSweep);
		}
	}

	// 플레이어 방향 수평 이동 + 회전
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(OwnerActor, 0);
		if (PlayerPawn)
		{
			FVector ToPlayer = PlayerPawn->GetActorLocation() - OwnerActor->GetActorLocation();
			ToPlayer.Z = 0.f;

			if (!ToPlayer.IsNearlyZero())
			{
				const FVector DesiredVel = ToPlayer.GetSafeNormal() * MoveSpeed;

				// VInterpTo
				Velocity = FMath::VInterpTo(Velocity, DesiredVel, DeltaTime, /*InterpSpeed*/ 5.f);
				
				FVector Next = OwnerActor->GetActorLocation() + Velocity * DeltaTime;

				FHitResult MoveHit;
				OwnerActor->SetActorLocation(Next, true, &MoveHit);

				// 회전 (Velocity 기준)
				if (!Velocity.IsNearlyZero())
				{
					const FRotator TargetRot = UKismetMathLibrary::MakeRotFromX(Velocity.GetSafeNormal());
					const FRotator NewRot = FMath::RInterpTo(OwnerActor->GetActorRotation(), TargetRot, DeltaTime, TurnRate);
					OwnerActor->SetActorRotation(NewRot);
				}
			}
		}
	}

	// LineTrace로 바닥이면 Z 스냅, 아니면 중력 낙하
	bool bOnGround = false;
	FHitResult GroundHit;

	{
		const FVector Start = OwnerActor->GetActorLocation();
		const FVector End   = Start - FVector(0, 0, GroundCheckDistance);

		FCollisionQueryParams Params(SCENE_QUERY_STAT(STT_Move_Ground), false);
		Params.AddIgnoredActor(OwnerActor);
		
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);

		bOnGround = OwnerActor->GetWorld()->LineTraceSingleByObjectType(
			GroundHit,
			Start,
			End,
			ObjParams,
			Params
		);

		if (bOnGround)
		{
			// 캡슐 높이의 절반만큼 위로 올려서 지면에 앉히기
			float HalfHeight = 0.f;
			if (ACharacter* Char = Cast<ACharacter>(OwnerActor))
			{
				if (UCapsuleComponent* Cap = Char->GetCapsuleComponent())
				{
					HalfHeight = Cap->GetScaledCapsuleHalfHeight();
				}
			}

			FVector Snapped = OwnerActor->GetActorLocation();
			Snapped.Z = GroundHit.ImpactPoint.Z + HalfHeight + 0.1f;
			OwnerActor->SetActorLocation(Snapped, false);

			FallVelocity = 0.f; // 땅이면 낙하 속도 리셋
			// UE_LOG(LogTemp, Warning, TEXT("[ST] Move Ground Snap"));
		}
	}

	if (!bOnGround)
	{
		// 공중이면 중력 적용 (Fall과 동일)
		FallVelocity += Gravity * DeltaTime;
		FallVelocity = FMath::Max(FallVelocity, TerminalVelocity);

		FVector Next = OwnerActor->GetActorLocation();
		Next.Z += FallVelocity * DeltaTime;

		FHitResult Hit;
		OwnerActor->SetActorLocation(Next, true, &Hit);

		if (Hit.bBlockingHit && Hit.ImpactNormal.Z >= GroundNormalZThreshold)
		{
			FallVelocity = 0.f;
			// 여기서는 "Move" 상태에서 그냥 계속 Running 유지 (착지했다고 상태를 끝낼 필요는 없음)
			// UE_LOG(LogTemp, Warning, TEXT("[ST] Move Falling -> Landed"));
		}
	}

	return EStateTreeRunStatus::Running;
}
