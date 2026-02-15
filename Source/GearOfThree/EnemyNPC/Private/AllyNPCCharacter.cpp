// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyNPCCharacter.h"
#include "WeaponComponent.h"
#include "Components/CapsuleComponent.h"

AAllyNPCCharacter::AAllyNPCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 75.0f);
	
	//팀을 아군으로 설정합니다. 
	TeamSide = ETeamSide::Ally;
	MaxAmmo = 20;
	CurrentAmmo = MaxAmmo;
	
	// 새로운 Skeletal Mesh 컴포넌트 생성 및 부착
	// 이름은 구분하기 쉽게 'SionMesh' 등으로 설정합니다.
	AllyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("AllyMesh"));
	AllyMesh->SetupAttachment(GetMesh()); // 부모 Mesh 밑에 부착
	
	GetMesh()->SetWorldLocationAndRotation(FVector(0,0,-90),FRotator(0,-90,0));
	
	// 1. 컴포넌트 생성 (CreateDefaultSubobject)
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
}

void AAllyNPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 여기에 나중에 DamageSystem 연결
}

void AAllyNPCCharacter::Fire()
{
	if (WeaponComponent)
	{
		// WeaponComponent -> SawGun -> Projectile Spawn 순으로 실행됨
		WeaponComponent->Fire();
	}
}
