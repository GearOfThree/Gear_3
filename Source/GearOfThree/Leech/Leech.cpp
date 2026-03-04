// Fill out your copyright notice in the Description page of Project Settings.


#include "Leech/Leech.h"
#include "GearOfThreeTypes.h"

// 기본 생성자
ALeech::ALeech()
{
	PrimaryActorTick.bCanEverTick = true;
	// 팀 사이드를 적으로 설정
	TeamSide = ETeamSide::Enemy;

}

void ALeech::BeginPlay()
{
	Super::BeginPlay();
	
	const int32 Count = FMath::Max(1, OrbitCount);
	// 인덱스 별로 시작 궤도 각도 분산 (360 / 전체수) * 인덱스
	OrbitAngleDeg = (360.f / (float)Count) * (float)OrbitIndex;
}

void ALeech::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!IsValid(LeechManager)) return; // 매니저 자체가 없으면 작동 X
	
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

	// 중심에 오프셋 더해 위치 설정
	SetActorLocation(Center + TiltedOffset, false);	// 스윕 false (리치끼리 충돌/끼임 최소화)
}
