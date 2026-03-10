// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Leech/Leech.h"
#include "GameFramework/Actor.h"
#include "LeechManager.generated.h"

UCLASS()
class GEAROFTHREE_API ALeechManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALeechManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// 매니저 스폰 직후 Location 값 저장(이후 범위 설정)
	UPROPERTY(EditAnywhere, Category="Manager|Move")
	FVector SpawnLocation;
	// 매니저의 이동 방향
	UPROPERTY(EditAnywhere, Category="Manager|Move")
	FVector Direction = FVector::ForwardVector;
	// 매니저의 이동 속력
	UPROPERTY(EditAnywhere, Category="Manager|Move")
	float Speed = 1000.f;
	// 매니저의 이동 가능 여부
	UPROPERTY(EditAnywhere, Category="Manager|Move")
	bool Moveable = true;
	// 매니저 이동 함수 및 함수 재생 타이머
	void ChangeDirection();
	FTimerHandle TimerHandle_ChangeDir;
	
	// 매니저 감염 함수 및 함수 재생 타이머
	void InfectionDirection();
	FTimerHandle TimerHandle_Infect;
	
	
	// 리치 스폰 공장
	UPROPERTY(EditAnywhere, Category="Leech|Spawn")
	TSubclassOf<ALeech> LeechFactory;
	// 스폰 시킬 리치 개체 수
	UPROPERTY(EditAnywhere, Category="Leech|Spawn")
	int32 SpawnCount = 50;
	// 스폰된 리치 보관 (배열)
	UPROPERTY(VisibleAnywhere, Category="Leech|Spawn")
	TArray<TObjectPtr<ALeech>> SpawnedLeeches;

	// 공전 파라미터 (리치들에게 주입)
	// 공전 반지름
	UPROPERTY(EditAnywhere, Category="Leech|Flock")
	float OrbitRadius = 200.f;
	// 공전 각속도 (deg/초)
	UPROPERTY(EditAnywhere, Category="Leech|Flock")
	float OrbitSpeedDegPerSec = 240.f;
	// 공전 궤도 최대 기울기
	UPROPERTY(EditAnywhere, Category="Leech|Flock")
	float MaxTiltDeg = 90.f;
	
	

private:
	// 스폰 함수
	void SpawnLeeches();

};
