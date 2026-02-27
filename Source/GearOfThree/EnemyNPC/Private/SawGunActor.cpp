// Fill out your copyright notice in the Description page of Project Settings.


#include "SawGunActor.h"

#include "BuzzKillProjectile.h"
#include "NPCCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASawGunActor::ASawGunActor()
{
	PrimaryActorTick.bCanEverTick = false; // 무기는 틱이 필요 없음.
}

void ASawGunActor::Fire()
{
	// 기본 유효성 검사 (부모 클래스의 컴포넌트 활용)
	if (!WeaponMesh || !ProjectileClass) return;

	// 무기를 들고 있는 주인(NPC) 가져오기
	ANPCCharacter* WeaponOwner = Cast<ANPCCharacter>(GetOwner());
	if (!WeaponOwner) return;

	// 총구 위치 가져오기 (소켓 이름 "muzzle")
	FVector MuzzlePos = WeaponMesh->GetSocketLocation(FName("muzzle"));
    
	// 발사 방향 결정
	FRotator SpawnRotation;

	// [핵심 변경] State Tree에서 결정된 타겟 정보를 NPC로부터 가져옵니다.
	// (지난번에 만든 Evaluator가 NPC의 특정 변수에 타겟을 저장하고 있다고 가정)
	AActor* CurrentTarget = WeaponOwner->GetCurrentTarget(); // NPC에 이 함수가 있어야 합니다.

	// VRandCone 함수는 절반의 각도(Half-angle)를 요구하므로 반으로 나눕니다.
	float HalfRad = FMath::DegreesToRadians(WeaponSpreadAngle / 2.0f);

	if (CurrentTarget)
	{
		// 타겟의 몸통 중앙을 조준
		FVector TargetLocation = CurrentTarget->GetActorLocation();
		FRotator OriginRotation = UKismetMathLibrary::FindLookAtRotation(MuzzlePos, TargetLocation);
		// 탄퍼짐 로직
		FVector RandomDirection = FMath::VRandCone(OriginRotation.Vector(), HalfRad);
		SpawnRotation = RandomDirection.Rotation();
	}
	else
	{
		// 타겟이 없다면 주인이 바라보는 정면으로 발사
		FRotator OriginRotation = WeaponOwner->GetActorRotation();
		// 탄퍼짐 로직
		FVector RandomDirection = FMath::VRandCone(OriginRotation.Vector(), HalfRad);
		SpawnRotation = RandomDirection.Rotation();
	}

	// 5. 스폰 파라미터 및 발사
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = WeaponOwner;
	SpawnParams.Instigator = WeaponOwner;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzlePos, SpawnRotation, SpawnParams);
	
	if (MuzzleFlashEffect && WeaponMesh)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleFlashEffect,
			WeaponMesh,             // 어디에 붙일 것인가?
			FName("muzzle"),  // 총구 소켓 이름
			FVector::ZeroVector, 
			FRotator::ZeroRotator, 
			EAttachLocation::SnapToTarget, 
			true                    // 붙어서 따라다닐 것인가?
		);
	}
	
	// 에디터에서 사운드 에셋을 제대로 넣었는지 확인.
	if (FireSound)
	{
		// 총구 위치(MuzzleLocation)에서 사운드를 재생하여, 실제 총구에서 소리가 퍼져나가게 만듭니다.
		// GetActorLocation() 대신 앞서 구하셨을 총구 소켓의 위치 변수를 넣는 것이 가장 정확합니다.
		FVector SoundLocation = WeaponMesh ? WeaponMesh->GetSocketLocation(FName("muzzle")) : GetActorLocation();

		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),    // 현재 월드
			FireSound,     // 재생할 사운드 파일
			SoundLocation, // 소리가 날 위치 (총구 위치)
			1.0f,          // 볼륨 배율 (1.0 = 100%)
			1.0f,          // 피치(음높이) 배율 (1.0 = 원본 음높이)
			0.0f           // 시작 지연 시간 (0초 후 바로 시작)
		);
	}
}

