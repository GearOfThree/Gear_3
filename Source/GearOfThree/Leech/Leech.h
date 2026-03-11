// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Particles/ParticleSystem.h"
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
	
	UFUNCTION()
	void OrbitToFall();
	UFUNCTION()
	void AnyToDead();
	
	// ST 컴포넌트 생성
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI|StateTree", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTreeComponent> StateTreeComp;
	// ST 슬롯 생성
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AI|StateTree")
	TObjectPtr<UStateTree> StateTreeSlot;
	// 낙하 상태 전환용 태그
	UPROPERTY(EditDefaultsOnly, Category="AI|StateTree")
	FGameplayTag OrbitToFallEventTag;

	// 공전 가능 여부 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bOrbiting = true;
	// 돌진 상태 확인 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bIsRush = false;
	// 사망 상태 변환 여부 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	bool bIsDeadLeech = false;
	// 원거리 공격 쿨타임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	float LastShootTime = -99999.f; // 처음엔 쿨타임 끝난 상태처럼
	// 돌진 공격 쿨타임
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Leech|State")
	float LastRushTime = -99999.f; // 처음엔 쿨타임 끝난 상태처럼
	
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Rush")
	bool bRushShouldEnd = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Leech|Rush")
	bool bRushHitPlayer = false;
	
	// 돌진 상태에서 플레이어 충돌 시 함수
	UFUNCTION()
	void OnRushHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);
	
	// 죽을 때 피 분수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* DeathAcidEffect = nullptr;
	// 죽을 때 폭발
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* DeathBombEffect = nullptr;
	
	UPROPERTY()
	bool bDying = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Death")
	float DeathDelay = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Death")
	float DeathScaleMultiplier = 1.55f;

	FVector DeathStartScale;
	FVector DeathTargetScale;

	FTimerHandle DeathGrowTimerHandle;
	FTimerHandle DeathFinalTimerHandle;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	class USoundBase* DeathSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
	class USoundBase* ExplosionSound;
	
	UFUNCTION()
	void Die();
	
private:
	float OrbitAngleDeg = 0.f;

};
