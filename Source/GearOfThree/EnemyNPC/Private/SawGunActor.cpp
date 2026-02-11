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
	// 투사체 클래스와 무기 메시가 있는지 확인
	if (WeaponMesh && ProjectileClass)
	{
		// 총구 소켓 위치 가져오기 ("Muzzle"이라는 이름의 소켓이 스켈레톤에 있어야 함)
		FVector MuzzleLocation = WeaponMesh->GetSocketLocation(FName("muzzle"));
		FRotator MuzzleRotation = WeaponMesh->GetSocketRotation(FName("muzzle"));

		// 3. 스폰 파라미터 설정
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner(); // 총의 주인 (사이언)
		SpawnParams.Instigator = GetInstigator();

		// 발사 (스폰)
		GetWorld()->SpawnActor<ABuzzKillProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	}
}
