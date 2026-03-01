// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_Bullet.h"
#include "MS_RecoilSpec.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "MS_Weapon.generated.h"

UENUM()
enum class EFireShape : uint8
{
	Line,
	Sphere,
};

UCLASS()
class GEAROFTHREE_API AMS_Weapon : public AActor
{
	GENERATED_BODY()
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	
	// Sets default values for this actor's properties
	AMS_Weapon();

protected:
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	// TObjectPtr<USceneComponent> Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
	// TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="BulletFactory")
	TSubclassOf<AMS_Bullet> BulletFactory;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> FirePosition;
	
	UPROPERTY(EditDefaultsOnly, Category="FX|Niagara")
	TObjectPtr<UNiagaraSystem> MuzzleFX = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="FX|Sound")
	TObjectPtr<USoundBase> FireSound = nullptr;
	
	// 탄피 // 이건 할지 안할지 모름
	UPROPERTY(EditDefaultsOnly, Category="FX|Shell")
	TSubclassOf<AActor> ShellFactory;
	
	UPROPERTY(EditDefaultsOnly, Category="FX|Shell")
	TObjectPtr<USceneComponent> EjectPosition;
	
	
	// 무기의 사거리 설정 
	UPROPERTY(EditDefaultsOnly)
	EFireShape FireShape = EFireShape::Line;
	
	// 직선 거리
	UPROPERTY(EditDefaultsOnly)
	float BulletMaxDistance = 100000.f;
	
	// 폭 범위
	UPROPERTY(EditDefaultsOnly)
	float BulletRadius = 50.f;

	// MS_RecoilSpec 클래스 호출변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon|Recoil")
	FMS_RecoilSpec RecoilSpec;
	
public:
	UFUNCTION(BlueprintPure, Category="Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	// 총알 발사 처리함수
	// MS_Player 에서 호출한다. 
	UFUNCTION(BlueprintCallable)
	void Fire();
	
protected:
	void FireTowards(const FVector& AimPoint);
	
	void PlayMuzzleFlash();
	
	void PlayFireSound();
	
	void EjectShell();
	
protected:
	// 연사 패턴용 샷 인덱스
	int32 ShotIndex = 0;

	// 발사 끊겼을 때 인덱스 리셋용
	float LastFireTime = -9999.f;

	UPROPERTY(EditAnywhere, Category="Weapon|Recoil")
	float ShotResetDelay = 0.25f; // 0.25초 이상 안 쏘면 패턴 초기화
};
