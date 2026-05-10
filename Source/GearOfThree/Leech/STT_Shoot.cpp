// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/STT_Shoot.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "StateTreeExecutionContext.h"
#include "Leech/Leech.h"
#include "AIDebugComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"

USTT_Shoot::USTT_Shoot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// EnterState에서 1회 처리하고 끝낼 거라 Tick 불필요
	bShouldCallTick = false;
	bShouldCallTickOnlyOnEvents = true;
}

EStateTreeRunStatus USTT_Shoot::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{	
	// OwnerActor 구하기
	AActor* OwnerActor = Cast<AActor>(Context.GetOwner());
	if (!OwnerActor)
	{
		UObject* OwnerObj = Context.GetOwner();
		OwnerActor = Cast<AActor>(OwnerObj ? OwnerObj->GetOuter() : nullptr);
	}
	ALeech* Leech = Cast<ALeech>(OwnerActor);
	if (!Leech || !OwnerActor->GetWorld())
	{
		return EStateTreeRunStatus::Failed;
	}

	if (Leech->DebugComp)
	{
		Leech->DebugComp->SetCurrentState(TEXT("Shoot"), TEXT("Firing Projectile"));
	}

	// 투사체 클래스 없으면 실패(또는 그냥 Succeeded로 넘겨도 됨)
	if (!ProjectileFactory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[STT_Shoot] ProjectileClass is null"));
		return EStateTreeRunStatus::Failed;
	}

	APawn* Target = UGameplayStatics::GetPlayerPawn(OwnerActor, 0);
	if (!Target)
	{
		return EStateTreeRunStatus::Failed;
	}

	UWorld* World = OwnerActor->GetWorld();

	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Shoot] World is null"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Shoot] World=%s  WorldType=%d  NetMode=%d"),
		*GetNameSafe(World),
		(int32)World->WorldType,
		(int32)World->GetNetMode());
	
	// 발사 위치: 리치의 로컬 오프셋을 월드로 변환
	const FVector SpawnLoc = OwnerActor->GetActorTransform().TransformPosition(MuzzleLocalOffset);

	// 발사 회전: 플레이어를 향하게
	FVector AimPoint = Target->GetActorLocation();

	const FVector Dir = (AimPoint - SpawnLoc);
	if (Dir.IsNearlyZero())
	{
		return EStateTreeRunStatus::Failed;
	}
	const FRotator SpawnRot = Dir.Rotation();

	FActorSpawnParameters Params;
	Params.Owner = OwnerActor;
	Params.Instigator = Cast<APawn>(OwnerActor);
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Projectile = World->SpawnActor<AActor>(ProjectileFactory, SpawnLoc, SpawnRot, Params);
	
	
	
	if (!Projectile)
	{
		UE_LOG(LogTemp, Warning, TEXT("[STT_Shoot] SpawnActor FAILED. Class=%s"), *GetNameSafe(ProjectileFactory));
		return EStateTreeRunStatus::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("[STT_Shoot] SpawnActor OK: %s  Owner=%s  Instigator=%s"),
	*GetNameSafe(Projectile),
	*GetNameSafe(Projectile->GetOwner()),
	*GetNameSafe(Projectile->GetInstigator()));

	// 쿨타임 갱신: "발사 성공 순간"에 찍어야 함
	Leech->LastShootTime = World->GetTimeSeconds();

	UE_LOG(LogTemp, Warning, TEXT("[STT_Shoot] Fired. LastShootTime=%.2f"), Leech->LastShootTime);

	// 1회 발사 후 바로 상태 종료 → Move로 돌아가게
	return EStateTreeRunStatus::Succeeded;
}

