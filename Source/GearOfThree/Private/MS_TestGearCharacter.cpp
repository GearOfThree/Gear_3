// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_TestGearCharacter.h"

#include "Components/SphereComponent.h"

AMS_TestGearCharacter::AMS_TestGearCharacter()
{
	//
	// CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	// CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	// CollisionComponent->SetSphereRadius(20);
	// RootComponent = CollisionComponent;
	//
	//
	// BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	// BodyMesh->SetupAttachment(CollisionComponent);
	// BodyMesh->SetNotifyRigidBodyCollision(true); // 물리 기반의 충돌 이벤트를 받을지 확인
	//
}

void AMS_TestGearCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMS_TestGearCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
