#include "WeaponComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h" // 타이머 사용을 위한 헤더

UWeaponComponent::UWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UWeaponComponent::EquipWeapon(TSubclassOf<AGearWeaponBase> WeaponClass, FName SocketName)
{
    if (!WeaponClass) return;

    if (CurrentWeapon)
    {
       CurrentWeapon->Destroy();
    }

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter || !GetWorld()) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerCharacter;
    SpawnParams.Instigator = OwnerCharacter;

    CurrentWeapon = GetWorld()->SpawnActor<AGearWeaponBase>(
       WeaponClass, 
       FVector::ZeroVector, 
       FRotator::ZeroRotator, 
       SpawnParams
    );

    if (CurrentWeapon && OwnerCharacter->GetMesh())
    {
       CurrentWeapon->AttachToComponent(
          OwnerCharacter->GetMesh(), 
          FAttachmentTransformRules::SnapToTargetIncludingScale, 
          SocketName
       );
        
       //새 무기를 들었으니 탄창을 꽉 채우고 장전 상태를 초기화합니다
       CurrentAmmo = MaxAmmo;
       bIsReloading = false;
    }
}

void UWeaponComponent::Fire()
{
    if (!CurrentWeapon) return;

    // 🚨 [추가] 총의 물리적인 연사 쿨타임 검사 (AI가 아무리 광클해도 여기서 막힘!)
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastFireTime < FireRate) return;

    if (bIsReloading) return;

    if (CurrentAmmo <= 0)
    {
        StartReload();
        return;
    }

    CurrentWeapon->Fire();
    CurrentAmmo--;
    
    // 🚨 [추가] 방금 쐈으니까 시간 기록!
    LastFireTime = CurrentTime; 

}


// [추가] 장전 로직 구현부

void UWeaponComponent::StartReload()
{
    // 이미 장전 중이면 중복 실행 방지
    if (bIsReloading) return;

    bIsReloading = true;

    // ReloadTime(예: 2초)이 지나면 EndReload 함수를 1번만 실행합니다.
    GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UWeaponComponent::EndReload, ReloadTime, false);
}

void UWeaponComponent::EndReload()
{
    // 장전 완료! 총알 채우고 상태 해제
    CurrentAmmo = MaxAmmo;
    bIsReloading = false;
    
}