// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyNPCCharacter.h"

#include "GearAIController.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
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

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	}
	
	AIControllerClass = AGearAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

void AEnemyNPCCharacter::ReceiveDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (HPComponent->IsDead) return;

	HPComponent->ApplyDamage(Damage);

	// 1. 체력 퍼센트 계산
	float HPPercent = (HPComponent->CurrentHP / HPComponent->MaxHP) * 100.f;

	// 2. 체력 구간별 Gibbing 로직 (75%, 50%, 25%)
	// if문을 독립적으로 배치하여 한 번에 큰 데미지를 입어도 순차적으로 터지게 할 수 있습니다.
	if (HPPercent <= 75.f && !bGibStage75)
	{
		bGibStage75 = true;
		ExecutePartialGib(BoneToHide75);
	}

	if (HPPercent <= 50.f && !bGibStage50)
	{
		bGibStage50 = true;
		ExecutePartialGib(BoneToHide50);
	}

	if (HPPercent <= 25.f && !bGibStage25)
	{
		bGibStage25 = true;
		ExecutePartialGib(BoneToHide25);
	}

	// 3. 사망 판정
	if (HPComponent->CurrentHP <= 0.f)
	{
		HPComponent->IsDead = true;
		// 2. 피보라 이펙트 재생
		if (BloodEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodEffect, GetActorLocation());
		}
		this->Die(); 
	}
	else
	{
		// 살아있을 때만 히트 리액션 재생
		if (HitReactMontage && GetMesh() && GetMesh()->GetAnimInstance())
		{
			GetMesh()->GetAnimInstance()->Montage_Play(HitReactMontage);
		}
	}
}

void AEnemyNPCCharacter::ExecutePartialGib(FName BoneName)
{
	if (SionMesh)
	{
		// 1. 본 숨기기 (해당 본과 그 자식들까지 모두 물리/시각적으로 제외)
		// PBO_None: 해당 본 아래의 모든 자식 본들도 숨깁니다.
		SionMesh->HideBoneByName(BoneName, PBO_None);

		// 2. 피보라 이펙트 재생
		if (BloodEffect)
		{
			FVector BoneLocation = SionMesh->GetSocketLocation(BoneName);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodEffect, BoneLocation);
		}

		// 3. 로그 출력
		UE_LOG(LogTemp, Warning, TEXT("💥 Gibbing Stage Triggered: %s"), *BoneName.ToString());
	}
}
