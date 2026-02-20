// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_Bullet.h"
#include "GameFramework/Actor.h"
#include "MS_Weapon.generated.h"

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
	
	// UPROPERTY(EditDefaultsOnly, Category="Weapon")
	// TSubclassOf<AMS_Bullet> BulletClass;
	
public:
	UFUNCTION(BlueprintPure, Category="Weapon")
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	
	// 총알 발사 처리함수
	void Fire();
};
