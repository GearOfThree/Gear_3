// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "Components/StateTreeComponent.h"
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
	void OrbitToFall();
	
	
	// ST 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|StateTree", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTreeComponent> StateTreeComp;
	// ST 슬롯 생성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|StateTree")
	TObjectPtr<UStateTree> StateTreeSlot;
	
	UPROPERTY(EditDefaultsOnly, Category="AI|StateTree")
	FGameplayTag OrbitToFallEventTag;

	// 공전 가능 여부 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bOrbiting = true;
	// 낙하 상태 변환 여부 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bOrbitToFall = false;
	// 사망 상태 변환 여부 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bIsDead = false;
	
	// 스폰 매니저가 세팅해줄 값들
	UPROPERTY(BlueprintReadWrite, Category="Leech|Flock")
	TObjectPtr<AActor> LeechManager = nullptr;

	UPROPERTY(BlueprintReadWrite, Category="Leech|Flock")
	int32 OrbitIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	int32 OrbitCount = 35;
	
	// 공전 파라미터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	float OrbitRadius = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Flock")
	float OrbitSpeedDegPerSec = 240.f;
	
	UPROPERTY(VisibleAnywhere, Category="Leech|Flock")
	FQuat OrbitPlaneQuat = FQuat::Identity;
	
	
private:
	float OrbitAngleDeg = 0.f;

};
