// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/Leech.h"
#include "TimerManager.h"
#include "GearOfThreeTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/RandomStream.h"
#include "GameplayTagsManager.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// 기본 생성자
ALeech::ALeech()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// ST 컴포넌트 만들어서 붙이기
	StateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComp"));
	// 팀 사이드를 적으로 설정
	TeamSide = ETeamSide::Enemy;
}

void ALeech::BeginPlay()
{
	Super::BeginPlay();
	
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ALeech::OnRushHit);
	HPComponent->MaxHP = 110;
	HPComponent->CurrentHP = 110;
	
	// 델리게이트 호출
	if (HPComponent)
	{
		HPComponent->OnFall.AddUObject(this, &ALeech::OrbitToFall);
		HPComponent->OnDead.AddUObject(this, &ALeech::AnyToDead);
	}
	
	if (StateTreeComp && StateTreeSlot)
	{
		StateTreeComp->SetStateTree(StateTreeSlot);
		StateTreeComp->StartLogic();
	}
	
	if (!OrbitToFallEventTag.IsValid())
	{
		OrbitToFallEventTag = FGameplayTag::RequestGameplayTag(FName("Event.Leech.OrbitToFall"));
	}
	
	const int32 Count = FMath::Max(1, OrbitCount);
	// 인덱스 별로 시작 궤도 각도 분산 (360 / 전체수) * 인덱스
	OrbitAngleDeg = (360.f / (float)Count) * (float)OrbitIndex;
}

void ALeech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bOrbiting)
	{
		return; // Falling / GroundMove면 여기서 공전 위치 덮어쓰기 금지
	}
	
	if (!IsValid(LeechManager)) return; // 매니저 자체가 없으면 작동 X
	
	// 랜덤 스트림 설정 후 궤도값 랜덤으로 변하게끔 설정
	FRandomStream RandomStream;
	RandomStream.Initialize(time(NULL) + OrbitIndex);
	OrbitRadius = FMath::Clamp(OrbitRadius + RandomStream.FRandRange(-5, 5), 100.f, 250.f);
	
	// 설정한 궤도 각도에 공전 각도(각속도 * DeltaTime)를 더해 새로운 궤도 각도를 설정하고 싶다
	OrbitAngleDeg = FMath::Fmod(OrbitAngleDeg + OrbitSpeedDegPerSec * DeltaTime, 360.f);

	// 스폰 당시 설정된 리치 매니저의 위치를 공전 중심으로 설정
	const FVector Center = LeechManager->GetActorLocation();
	// 라디안으로 바꿔준다.
	const float Rad = FMath::DegreesToRadians(OrbitAngleDeg);

	// 중심으로부터 공전 각도를 계산한 최종 위치를 저장
	FVector Offset;
	Offset.X = FMath::Cos(Rad) * OrbitRadius;
	Offset.Y = FMath::Sin(Rad) * OrbitRadius;
	Offset.Z = 0;

	// 궤도 면 회전
	const FVector TiltedOffset = OrbitPlaneQuat.RotateVector(Offset);

	// 목표 방향을 바라보게 하는 것이 먼저 진행되어야 함
	SetActorRotation(UKismetMathLibrary::MakeRotFromX((Center + TiltedOffset - GetActorLocation()).GetSafeNormal()));
	// 중심에 오프셋 더해 위치 설정
	SetActorLocation(Center + TiltedOffset, false);	// 스윕 false (리치끼리 충돌/끼임 최소화)
}

void ALeech::OrbitToFall()
{
	if (StateTreeComp && OrbitToFallEventTag.IsValid())
	{
		StateTreeComp->SendStateTreeEvent(OrbitToFallEventTag);
	}
}

void ALeech::AnyToDead()
{
	bIsDeadLeech = true;
	bOrbiting = false;
}

void ALeech::OnRushHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsRush)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);

	bRushShouldEnd = true;

	if (OtherActor == PlayerPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[Leech Rush Hit] Hit Player"));
		bRushHitPlayer = true;
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("[Leech Rush Hit] Hit Non-Player: %s"), *OtherActor->GetName());
		bRushHitPlayer = false;
	}
}

void ALeech::Die()
{
	if (bDying)
	{
		return;
	}
	bDying = true;

	DeathStartScale = GetActorScale3D();
	DeathTargetScale = DeathStartScale * DeathScaleMultiplier;

	// 첫 번째 이펙트
	if (DeathAcidEffect)
	{
		const FVector SpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 15.f);

		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DeathAcidEffect,
			SpawnLoc,
			GetActorRotation(),
			true
		);
	}

	// 커지는 연출용 반복 타이머
	GetWorldTimerManager().SetTimer(
		DeathGrowTimerHandle,
		[this]()
		{
			if (!IsValid(this))
			{
				return;
			}

			const float CurrentTime =
				GetWorldTimerManager().GetTimerElapsed(DeathFinalTimerHandle);

			const float Alpha = FMath::Clamp(CurrentTime / DeathDelay, 0.f, 1.f);

			const FVector NewScale = FMath::Lerp(DeathStartScale, DeathTargetScale, Alpha);
			SetActorScale3D(NewScale);
		},
		0.02f,
		true
	);

	// 마지막 폭발 + 파괴
	GetWorldTimerManager().SetTimer(
		DeathFinalTimerHandle,
		[this]()
		{
			// 마지막 순간 목표 크기로 확정
			SetActorScale3D(DeathTargetScale);

			// 반복 타이머 정지
			GetWorldTimerManager().ClearTimer(DeathGrowTimerHandle);

			// 두 번째 이펙트
			if (DeathBombEffect)
			{
				const FVector SpawnLoc = GetActorLocation() + FVector(0.f, 0.f, 15.f);

				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					DeathBombEffect,
					SpawnLoc,
					GetActorRotation(),
					true
				);
			}

			Destroy();
		},
		DeathDelay,
		false
	);
}
