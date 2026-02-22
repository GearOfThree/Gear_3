// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyGunActor.h"

#include "AllyProjectile.h"
#include "NPCCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AAllyGunActor::AAllyGunActor()
{
	PrimaryActorTick.bCanEverTick = false;
	WeaponMesh -> SetRelativeScale3D(FVector(0.01f));
	MuzzleLocation -> SetRelativeLocation(FVector(6000.0f,0.0f,-1100.0f));
}

void AAllyGunActor::Fire()
{
    //총알 클래스가 블루프린트에서 설정되었는지 확인
    if (!ProjectileClass)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("❌ [Error] ProjectileClass is None!"));
        return;
    }

    //무기 메쉬 확인
    if (!WeaponMesh)
    {
         GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("❌ [Error] WeaponMesh is Missing!"));
         return;
    }

    ANPCCharacter* OwnerNPC = Cast<ANPCCharacter>(GetOwner());
    if (!OwnerNPC) return;

    // 총구 위치 가져오기 (소켓 이름: Muzzle)
    // 만약 Muzzle 소켓이 없으면 총의 원점(Root)에서 나갑니다.
    FVector SpawnLocation = WeaponMesh->DoesSocketExist(FName("Muzzle")) ? 
                            WeaponMesh->GetSocketLocation(FName("Muzzle")) : 
                            GetActorLocation();

    // 2. 발사 방향 계산
    FRotator SpawnRotation;
    AActor* Target = OwnerNPC->GetCurrentTarget();

    if (Target)
    {
        // 타겟이 있으면 타겟을 향해 발사
        FVector TargetLocation = Target->GetActorLocation();
        SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
    }
    else
    {
        // 타겟이 없으면 주인이 보는 방향으로 발사
        SpawnRotation = OwnerNPC->GetActorRotation();
    }

    // 총알 스폰 파라미터
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerNPC;
    SpawnParams.Instigator = OwnerNPC;
    //총알이 벽이나 총기 모델에 겹쳐도 강제로 스폰시킴
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 스폰
    AAllyProjectile* SpawningBullet = GetWorld()->SpawnActor<AAllyProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
    
    if (MuzzleFlashEffect && WeaponMesh)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlashEffect,
            WeaponMesh,             // 어디에 붙일 것인가?
            FName("Muzzle"),  // 총구 소켓 이름
            FVector::ZeroVector, 
            FRotator::ZeroRotator, 
            EAttachLocation::SnapToTarget, 
            true                    // 붙어서 따라다닐 것인가?
        );
    }
}
