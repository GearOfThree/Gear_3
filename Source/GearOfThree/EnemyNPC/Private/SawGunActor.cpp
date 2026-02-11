// Fill out your copyright notice in the Description page of Project Settings.


#include "SawGunActor.h"

// Sets default values
ASawGunActor::ASawGunActor()
{
	PrimaryActorTick.bCanEverTick = false; // 무기는 보통 틱이 필요 없음.

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
}

// Called when the game starts or when spawned
void ASawGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

