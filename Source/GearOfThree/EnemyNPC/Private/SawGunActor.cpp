// Fill out your copyright notice in the Description page of Project Settings.


#include "SawGunActor.h"
#include "BuzzkillProjectile.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASawGunActor::ASawGunActor()
{
	PrimaryActorTick.bCanEverTick = false; // 무기는 틱이 필요 없음.

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
    
	// 3. [수정] 조준 방향 계산 (플레이어를 향하도록)
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FRotator SpawnRotation;

	if (Player)
	{
		// 플레이어의 위치를 가져오되, 발밑이 아닌 몸통(중앙)을 조준하도록 보정
		FVector TargetLocation = Player->GetActorLocation();
        
		// [중요] 총구 위치에서 플레이어 위치를 바라보는 회전값 계산 (Pitch, Yaw 모두 포함)
		SpawnRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, TargetLocation);
	}
	else
	{
		// 플레이어를 못 찾을 경우를 대비한 기본값 (정면)
		SpawnRotation = GetOwner()->GetActorForwardVector().Rotation();
	}

	// 4. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = GetInstigator();

	// 5. 발사 (이제 위아래 각도가 적용된 SpawnRotation 사용)
	GetWorld()->SpawnActor<ABuzzKillProjectile>(ProjectileClass, MuzzleLocation, SpawnRotation, SpawnParams);
}