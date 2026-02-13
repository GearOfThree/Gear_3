// Fill out your copyright notice in the Description page of Project Settings.


#include "SawGunActor.h"
#include "BuzzkillProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

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

void ASawGunActor::Fire()
{
	// 1. 데이터 유효성 검사
	if (!WeaponMesh || !ProjectileClass) return;

	// 2. 총구(Muzzle) 위치 가져오기
	FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("Muzzle"));
	
	// 캐릭터가 바라보는 방향(Forward)을 그대로 회전값(Rotation)으로 변환합니다.
	FRotator SpawnRotation = GetOwner()->GetActorForwardVector().Rotation();

	// 4. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	// 5. 발사 (스폰)
	GetWorld()->SpawnActor<ABuzzKillProjectile>(ProjectileClass, MuzzleLocation, SpawnRotation, SpawnParams);
}