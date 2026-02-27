// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyNPC/Public/GearCharacter.h"
#include "MS_TestGearCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API AMS_TestGearCharacter : public ACharacter
{
	GENERATED_BODY()
	
protected:
	AMS_TestGearCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaTime ) override;
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* BodyMesh;
	
	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, Category=StatInfo)
	float Hp;
	
	UPROPERTY(VisibleAnywhere, Category=StatInfo)
	float MaxHp;
	
	UPROPERTY(VisibleAnywhere)
	bool IsDead;
};
