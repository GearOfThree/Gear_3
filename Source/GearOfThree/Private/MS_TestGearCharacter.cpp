// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_TestGearCharacter.h"

#include "Components/SphereComponent.h"

AMS_TestGearCharacter::AMS_TestGearCharacter()
{
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(BodyMesh);
	
	// HPComponent = CreateDefaultSubobject<UMS_HPComponent>(TEXT("HP"));
}

void AMS_TestGearCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AMS_TestGearCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMS_TestGearCharacter::ReceiveDamage_Implementation(float Damage, AActor* DamageCauser)
{
	IMS_Damageable::ReceiveDamage_Implementation(Damage, DamageCauser);
	HPComponent->ApplyDamage(Damage);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Current HP: %.1f Damage: %.0f"), HPComponent->CurrentHP, Damage)
		);
	}
	
	// 죽었어 
	if (HPComponent->CurrentHP <= 0.f)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,  // 각 객체마다 고유 Key
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Target Dead"))
		);
		HPComponent->IsDead = true;
	}
	
}
