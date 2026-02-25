// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearWeaponBase.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
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

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AAllyProjectile> ProjectileClass;

	// 🔹 총구 화염 이펙트 (블루프린트에서 나이아가라 에셋 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* MuzzleFlashEffect;
};
