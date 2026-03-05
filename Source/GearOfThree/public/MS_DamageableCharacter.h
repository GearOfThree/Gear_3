// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GearCharacter.h"
#include "MS_Damageable.h"
#include "MS_HPComponent.h"
#include "GameFramework/Actor.h"
#include "MS_DamageableCharacter.generated.h"



UCLASS()
class GEAROFTHREE_API AMS_DamageableCharacter : public AGearCharacter, public IMS_Damageable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMS_DamageableCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMS_HPComponent* HPComponent;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBroadcasted = false;
	
	virtual void ReceiveDamage_Implementation(float Damage, AActor* DamageCauser) override;
};
