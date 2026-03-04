// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_DamageableCharacter.h"
#include "Leech.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API ALeech : public AMS_DamageableCharacter
{
	GENERATED_BODY()
public:
	ALeech();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 스폰 매니저가 세팅해줄 값들
	UPROPERTY(BlueprintReadWrite, Category="Leech|Flock")
	TObjectPtr<AActor> LeechManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="Leech|Flock")
	int32 OrbitIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	int32 OrbitCount = 50;
	
	// 공전 파라미터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	float OrbitRadius = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	float OrbitSpeedDegPerSec = 240.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	float HeightOffset = 80.f;
	
	UPROPERTY(VisibleAnywhere, Category="Leech|Flock")
	FQuat OrbitPlaneQuat = FQuat::Identity;
	
private:
	float OrbitAngleDeg = 0.f;

};
