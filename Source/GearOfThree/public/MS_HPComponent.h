// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MS_HPComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDead)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHpChanged, AActor*, float, float)
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GEAROFTHREE_API UMS_HPComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMS_HPComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float MaxHP = 100.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HP")
	float CurrentHP = 100.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsDead;
	
	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float Damage);
	
	// 델리게이트 설정
	FOnDead OnDead;
	FOnDead OnFall;
	
	FOnHpChanged OnHpChanged;
	
};
