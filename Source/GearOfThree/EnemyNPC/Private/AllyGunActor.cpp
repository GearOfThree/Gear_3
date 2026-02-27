// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyGunActor.h"

#include "AllyProjectile.h"
#include "NPCCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

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
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Error] ProjectileClass is None!"));
        return;
    }

    //무기 메쉬 확인
    if (!WeaponMesh)
    {
         GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Error] WeaponMesh is Missing!"));
         return;
    }

    ANPCCharacter* OwnerNPC = Cast<ANPCCharacter>(GetOwner());
    if (!OwnerNPC) return;

    // 총구 위치 가져오기 (소켓 이름: muzzle)
    // 만약 muzzle 소켓이 없으면 총의 원점(Root)에서 나갑니다.
    FVector SpawnLocation = WeaponMesh->DoesSocketExist(FName("muzzle")) ? 
                            WeaponMesh->GetSocketLocation(FName("muzzle")) : 
                            GetActorLocation();

    // 2. 발사 방향 계산
    FRotator SpawnRotation;
    AActor* Target = OwnerNPC->GetCurrentTarget();
    
    // VRandCone 함수는 절반의 각도(Half-angle)를 요구하므로 반으로 나눕니다.
    float HalfRad = FMath::DegreesToRadians(WeaponSpreadAngle / 2.0f);

    if (Target)
    {
        // 타겟이 있으면 타겟을 향해 발사
        FVector TargetLocation = Target->GetActorLocation();
        FRotator OriginRotation = UKismetMathLibrary::FindLookAtRotation(SpawnLocation, TargetLocation);
        
        // 탄퍼짐 로직
        FVector RandomDirection = FMath::VRandCone(OriginRotation.Vector(), HalfRad);
        SpawnRotation = RandomDirection.Rotation();
    }
    else
    {
        // 타겟이 없으면 주인이 보는 방향으로 발사
        FRotator OriginRotation = OwnerNPC->GetActorRotation();
        
        // 탄퍼짐 로직
        FVector RandomDirection = FMath::VRandCone(OriginRotation.Vector(), HalfRad);
        SpawnRotation = RandomDirection.Rotation();
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
            WeaponMesh,             // 어디에 붙일 것인가
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
