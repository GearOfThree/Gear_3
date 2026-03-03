// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_Damageable.h"
#include "MS_DamageableCharacter.h"
#include "MS_HPComponent.h"
#include "EnemyNPC/Public/GearCharacter.h"
#include "MS_TestGearCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API AMS_TestGearCharacter : public AMS_DamageableCharacter
{
	GENERATED_BODY()
	
public:
	AMS_TestGearCharacter();
	
	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaTime ) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BodyMesh;
	
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
// 	UMS_HPComponent* HPComponent;
// 	
public:
	virtual void ReceiveDamage_Implementation(float Damage, AActor* DamageCauser) override;
};
