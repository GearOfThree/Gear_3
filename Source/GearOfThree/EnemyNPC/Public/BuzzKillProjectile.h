// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuzzKillProjectile.generated.h"

UCLASS()
class GEAROFTHREE_API ABuzzKillProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuzzKillProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* SawMesh;
	
	// 발사 속도 (Impulse 강도)
	UPROPERTY(EditAnywhere, Category="Physics")
	float LaunchPower = 3000.0f;
	
	// 회전 속도 (토크)
	UPROPERTY(EditAnywhere, Category="Physics")
	float SpinPower = 50000.0f;
	
	// 반발 계수 (0.0 ~ 1.0): 1.0이면 에너지 손실 없이 튕김
	UPROPERTY(EditAnywhere, Category = "Physics")
	float Bounciness = 0.9f;

	UFUNCTION() // 반드시 UFUNCTION 매크로 필요
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
