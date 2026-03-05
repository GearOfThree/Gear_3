// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_DamageableCharacter.h"

#include "NPCCharacter.h"

// Sets default values
AMS_DamageableCharacter::AMS_DamageableCharacter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	HPComponent = CreateDefaultSubobject<UMS_HPComponent>(TEXT("HP"));
}

// Called when the game starts or when spawned
void AMS_DamageableCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMS_DamageableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMS_DamageableCharacter::ReceiveDamage_Implementation(float Damage, AActor* DamageCauser)
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
		HPComponent->IsDead = true;
		
		HPComponent->OnDead.Broadcast();
		
		
		
		GEngine->AddOnScreenDebugMessage(
			-1,  // 각 객체마다 고유 Key
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("Target Dead"))
		);
		
		if (ANPCCharacter* NPC = Cast<ANPCCharacter>(this))
		{
			NPC->Die(); // 애니메이션 재생, 충돌 끄기, 3초 뒤 파괴 실행!
		}
	}
	
}

// void AMS_TestGearCharacter::ReceiveDamage_Implementation(float Damage, AActor* DamageCauser)
// {
// 	HP->ApplyDamage(Damage);
// 	
// 	if (GEngine)
// 	{
// 		GEngine->AddOnScreenDebugMessage(
// 			-1,
// 			2.0f,
// 			FColor::Green,
// 			FString::Printf(TEXT("Current HP: %.1f Damage: %.0f"), HP->CurrentHP, Damage)
// 		);
// 	}
// 	
// 	// 죽었어 
// 	if (HP->CurrentHP <= 0.f)
// 	{
// 		GEngine->AddOnScreenDebugMessage(
// 			-1,  // 각 객체마다 고유 Key
// 			2.0f,
// 			FColor::Green,
// 			FString::Printf(TEXT("Target Dead"))
// 		);
// 		HP->IsDead = true;
// 	}
// 	
// }