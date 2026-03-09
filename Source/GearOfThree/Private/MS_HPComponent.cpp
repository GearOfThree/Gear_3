// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_HPComponent.h"

// Sets default values for this component's properties
UMS_HPComponent::UMS_HPComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMS_HPComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UMS_HPComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UMS_HPComponent::ApplyDamage(const float Damage)
{
	const float OldHP = CurrentHP;
	
	CurrentHP = FMath::Clamp(CurrentHP - Damage, 0.f, MaxHP);
	
	UE_LOG(LogTemp, Warning, TEXT("ApplyDamage / %.1f -> %.1f"), OldHP, CurrentHP);
	
	if (!FMath::IsNearlyEqual(OldHP, CurrentHP))
	{
		
		UE_LOG(LogTemp, Warning, TEXT("Broadcast OnHpChanged / CurrentHP=%.1f MaxHP=%.1f"), CurrentHP, MaxHP);
		// 플레이어가 사용한다. 
		OnHpChanged.Broadcast(CurrentHP, MaxHP);
	}
}

