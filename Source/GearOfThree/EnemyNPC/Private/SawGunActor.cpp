// Fill out your copyright notice in the Description page of Project Settings.


#include "SawGunActor.h"

#include "BuzzKillProjectile.h"
#include "NPCCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASawGunActor::ASawGunActor()
{
	PrimaryActorTick.bCanEverTick = false; // 무기는 틱이 필요 없음.
}

void ASawGunActor::Fire()
{
	// 1. 기본 유효성 검사 (부모 클래스의 컴포넌트 활용)
	if (!WeaponMesh || !ProjectileClass) return;

	// 2. 무기를 들고 있는 주인(NPC) 가져오기
	ANPCCharacter* WeaponOwner = Cast<ANPCCharacter>(GetOwner());
	if (!WeaponOwner) return;

	// 3. 총구 위치 가져오기 (소켓 이름 "Muzzle")
	FVector MuzzlePos = WeaponMesh->GetSocketLocation(FName("Muzzle"));
    
	// 4. 발사 방향 결정
	FRotator SpawnRotation;

	// [핵심 변경] State Tree에서 결정된 타겟 정보를 NPC로부터 가져옵니다.
	// (지난번에 만든 Evaluator가 NPC의 특정 변수에 타겟을 저장하고 있다고 가정)
	AActor* CurrentTarget = WeaponOwner->GetCurrentTarget(); // NPC에 이 함수가 있어야 합니다.

	if (CurrentTarget)
	{
		// 타겟의 몸통 중앙을 조준
		FVector TargetLocation = CurrentTarget->GetActorLocation();
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(MuzzlePos, TargetLocation);
	}
	else
	{
		// 타겟이 없다면 주인이 바라보는 정면으로 발사
		SpawnRotation = WeaponOwner->GetActorRotation();
	}

	// 5. 스폰 파라미터 및 발사
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = WeaponOwner;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzlePos, SpawnRotation, SpawnParams);
}

