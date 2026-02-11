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

void UWeaponComponent::EquipWeapon(TSubclassOf<AActor> WeaponClass, FName SocketName)
{
	if (!WeaponClass) return;

	// 1. 이 컴포넌트를 가진 주인(사이언)을 가져옵니다.
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	// 2. 무기 스폰 (SpawnActorFromClass 노드 역할)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter->GetInstigator();

	CurrentWeapon = GetWorld()->SpawnActor<ASawGunActor>(WeaponClass, OwnerCharacter->GetActorTransform(), SpawnParams);

	// 3. 무기 부착 (AttachToComponent 노드 역할)
	if (CurrentWeapon)
	{
		// 주인 캐릭터의 메시(Mesh)를 찾습니다.
		// 일반적인 GetMesh() 대신 사이언의 AdditionalMesh를 사용한다면 그 컴포넌트를 찾아야 합니다.
		USkeletalMeshComponent* CharacterMesh = OwnerCharacter->FindComponentByClass<USkeletalMeshComponent>();

		if (CharacterMesh)
		{
			CurrentWeapon->AttachToComponent(CharacterMesh, 
				FAttachmentTransformRules::SnapToTargetIncludingScale, 
				SocketName);
		}
	}
}

// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UWeaponComponent::Fire()
{
	// 무기를 들고 있을 때만 발사
	if (CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}