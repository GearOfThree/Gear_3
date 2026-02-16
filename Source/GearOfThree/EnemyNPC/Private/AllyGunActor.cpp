// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyNPC/Public/AllyGunActor.h"

#include "AllyProjectile.h"
#include "NPCCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAllyGunActor::AAllyGunActor()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh -> SetRelativeScale3D(FVector(0.01f));
	MuzzleLocation -> SetRelativeLocation(FVector(6000.0f,0.0f,-1100.0f));
}

void AAllyGunActor::Fire()
{
	// 1. 유효성 검사
	if (!ProjectileClass || !WeaponMesh) return;

	ANPCCharacter* OwnerNPC = Cast<ANPCCharacter>(GetOwner());
	if (!OwnerNPC) return;

	// 2. 총구 위치와 방향 계산
	// 소켓 이름이 "Muzzle"이라고 가정합니다.
	FVector SpawnLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
	FRotator SpawnRotation;

	// 3. 타겟(사이언) 조준 로직
	AActor* Target = OwnerNPC->GetCurrentTarget();
	if (Target)
	{
		// 타겟의 몸통 방향 계산
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, Target->GetActorLocation());
	}
	else
	{
		// 타겟이 없으면 주인의 정면 방향
		SpawnRotation = OwnerNPC->GetActorRotation();
	}

	// 4. 총알 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerNPC;
	SpawnParams.Instigator = OwnerNPC;

	GetWorld()->SpawnActor<AAllyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
}
