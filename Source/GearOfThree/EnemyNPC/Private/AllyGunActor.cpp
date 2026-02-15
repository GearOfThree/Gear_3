// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNPC/Public/AllyGunActor.h"

// Sets default values
AAllyGunActor::AAllyGunActor()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh -> SetRelativeScale3D(FVector(0.01f));
	MuzzleLocation -> SetRelativeLocation(FVector(6000.0f,0.0f,-1100.0f));
}

// Called when the game starts or when spawned
void AAllyGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAllyGunActor::Fire()
{

}

