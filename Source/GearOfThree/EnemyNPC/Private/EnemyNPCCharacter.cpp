// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyNPCCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GearOfThree.h"
#include "SawGunActor.h"
#include "WeaponComponent.h"

// Sets default values
AEnemyNPCCharacter::AEnemyNPCCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(40.f, 140.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	//이 위까지는 ThirdPorson 기본 설정
	
	// 새로운 Skeletal Mesh 컴포넌트 생성 및 부착
	// 이름은 구분하기 쉽게 'SionMesh' 등으로 설정합니다.
	SionMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SionMesh"));
	SionMesh->SetupAttachment(GetMesh()); // 부모 Mesh 밑에 부착
	
	GetMesh()->SetWorldLocationAndRotation(FVector(0,0,-115.0),FRotator(0,-90,0));
	GetMesh()->SetRelativeScale3D(FVector(1.6f));
	
	// 1. 컴포넌트 생성 (CreateDefaultSubobject)
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	
	// 사이언 전용 설정
	TeamSide = ETeamSide::Enemy; // 팀을 Enemy로 설정

}

void AEnemyNPCCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponComponent && StartingWeaponClass)
	{
		WeaponComponent->SetFireRate(0.4f);
		WeaponComponent->SetAmmo(10);
		WeaponComponent->EquipWeapon(StartingWeaponClass, FName("weapon_socket"));
	}
}


void AEnemyNPCCharacter::FireSawBlade(const FInputActionValue& Value)
{
	if (WeaponComponent)
	{
		// WeaponComponent -> SawGun -> Projectile Spawn 순으로 실행됨
		WeaponComponent->Fire();
	}
}