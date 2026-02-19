// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"

#include "SawGunActor.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponComponent::EquipWeapon(TSubclassOf<AGearWeaponBase> WeaponClass, FName SocketName)
{
	if (!WeaponClass) return;

	// 1. 기존 무기 제거 (중복 장착 방지)
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}

	// 2. 주인 캐릭터 정보 가져오기
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !GetWorld()) return;

	// 3. 무기 액터 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	CurrentWeapon = GetWorld()->SpawnActor<AGearWeaponBase>(
		WeaponClass, 
		FVector::ZeroVector, 
		FRotator::ZeroRotator, 
		SpawnParams
	);

	// 소켓에 부착
	if (CurrentWeapon && OwnerCharacter->GetMesh())
	{
		// 소켓의 위치/회전값에 무기를 장착
		CurrentWeapon->AttachToComponent(
			OwnerCharacter->GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale, 
			SocketName
		);
        
		UE_LOG(LogTemp, Log, TEXT("Weapon equipped to socket: %s"), *SocketName.ToString());
	}
}

void UWeaponComponent::Fire()
{
	// 무기를 들고 있을 때만 발사
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}