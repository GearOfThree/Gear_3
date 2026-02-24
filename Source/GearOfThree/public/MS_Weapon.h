// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_Bullet.h"
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

	
public:
	UFUNCTION(BlueprintPure, Category="Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	// 총알 발사 처리함수
	// MS_Player 에서 호출한다. 
	void Fire();
	
protected:
	void FireTowards(const FVector& AimPoint);
	
	void PlayMuzzleFlash();
	
	void PlayFireSound();
	
	void EjectShell();
};
