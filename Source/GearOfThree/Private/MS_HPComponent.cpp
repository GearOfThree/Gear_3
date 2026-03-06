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
	
	CurrentHP = FMath::Clamp(CurrentHP - Damage, 0.f, MaxHP);
	
	// 각 대상자의 HP 변경을 알린다. 
	OnHpChanged.Broadcast(GetOwner(), CurrentHP, MaxHP);
}

