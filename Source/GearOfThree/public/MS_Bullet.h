// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MS_Bullet.generated.h"

UCLASS()
class GEAROFTHREE_API AMS_Bullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMS_Bullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
public:
	// 발사체의 이동을 담당할 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Movement)
	class UProjectileMovementComponent* MovementComponent;
	
	// 충돌 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* CollisionComponent;
	
	// 외관 컴포넌트
	UPROPERTY(VisibleAnywhere, Category=BodyMesh)
	class UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(EditDefaultsOnly, Category="FX|Cascade")
	TObjectPtr<UParticleSystem> ImpactFX;
	
	UFUNCTION()
	void OnBulletHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
					 UPrimitiveComponent* OtherComp, FVector NormalImpulse,
					 const FHitResult& Hit);
	
	UPROPERTY(EditDefaultsOnly, Category=Power)
	float Power = 10;
};
