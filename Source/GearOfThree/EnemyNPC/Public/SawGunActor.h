// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SawGunActor.generated.h"

class ABuzzKillProjectile;

UCLASS()
class GEAROFTHREE_API ASawGunActor : public AActor
{
	GENERATED_BODY()
	
protected:
	// 무기 외형을 담당할 스켈레탈 메시
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;
	
public:	
	// Sets default values for this actor's properties
	ASawGunActor();
	
	// 사격 함수
	void Fire();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// [중요] 블루프린트에서 할당할 톱날 투사체 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ABuzzKillProjectile> ProjectileClass;
};
