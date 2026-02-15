// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearWeaponBase.h"
#include "GameFramework/Actor.h"
#include "AllyGunActor.generated.h"

UCLASS()
class GEAROFTHREE_API AAllyGunActor : public AGearWeaponBase
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAllyGunActor();

	virtual void Fire() override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
