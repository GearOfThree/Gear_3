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
	
	float PrevHP = HPComponent->CurrentHP;
	
	HPComponent->ApplyDamage(Damage);
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(
	// 		-1,
	// 		2.0f,
	// 		FColor::Green,
	// 		FString::Printf(TEXT("Current HP: %.1f Damage: %.0f"), HPComponent->CurrentHP, Damage)
	// 	);
	// }
	
	
	// 현재 체력이 90이 되었어(리치 분기점) 
	// 앞의 두 조건만으로 충분하지만 혹시 모르니 bool 변수로 중복 호출 가능성 제거
	if (HPComponent->CurrentHP <= 90.f && PrevHP > 90.f && !bBroadcasted)
	{
		bBroadcasted = true;
		HPComponent->OnFall.Broadcast();
	}
	
	// 죽었어 
	// 반복해서 실행하면 안돼
	if (HPComponent->CurrentHP <= 0.f && !HPComponent->IsDead)
	{
		HPComponent->IsDead = true;
		
		HPComponent->OnDead.Broadcast();
		
		// GEngine->AddOnScreenDebugMessage(
		// 	-1,  // 각 객체마다 고유 Key
		// 	2.0f,
		// 	FColor::Green,
		// 	FString::Printf(TEXT("Target Dead"))
		// );
		
		if (ANPCCharacter* NPC = Cast<ANPCCharacter>(this))
		{
			NPC->Die(); // 애니메이션 재생, 충돌 끄기, 3초 뒤 파괴 실행!
		}
	}else
	{
		if (HitReactMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
		}
	}
	
}

bool AMS_DamageableCharacter::GetIsDead() const
{
	return HPComponent->IsDead;
}
