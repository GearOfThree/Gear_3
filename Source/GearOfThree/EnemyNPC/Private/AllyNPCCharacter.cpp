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
	// 🚩 루프를 돌며 모든 머티리얼 슬롯에 대해 다이내믹 인스턴스 생성
	if (AllyMesh)
	{
		int32 NumMaterials = AllyMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* BaseMat = AllyMesh->GetMaterial(i);
			if (BaseMat)
			{
				// AllyMesh를 대상으로 인스턴스 생성
				UMaterialInstanceDynamic* DMI = AllyMesh->CreateDynamicMaterialInstance(i, BaseMat);
				if (DMI)
				{
					DynamicMaterials.Add(DMI);
				}
			}
		}
	}
	
	if (WeaponComponent && StartingWeaponClass)
	{
		WeaponComponent->EquipWeapon(StartingWeaponClass, FName("ally_gun_socket"));
	}
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

void AAllyNPCCharacter::SetTeamSide(bool bIsEnemy)
{
	TeamSide = bIsEnemy ? ETeamSide::Enemy : ETeamSide::Ally;
	// 🚩 2. 팀에 따른 색상 결정
	// 적군이면 빨간색(Red), 아군이면 원래대로 하얀색(White)
	FLinearColor TargetColor = bIsEnemy ? FLinearColor::Red : FLinearColor::White;

	// 🚩 3. 저장해둔 모든 머티리얼 슬롯 순회하며 색상 변경
	for (UMaterialInstanceDynamic* DMI : DynamicMaterials)
	{
		if (DMI)
		{
			// 머티리얼 에디터의 파라미터 이름 "BodyColor"와 일치해야 합니다!
			DMI->SetVectorParameterValue(TEXT("BodyColor"), TargetColor);
		}
	}

	// [디버그용 로그] 색상이 바뀌는지 로그 창에서 확인하세요.
	UE_LOG(LogTemp, Warning, TEXT("Team Changed: %s, Color Applied to AllyMesh"), bIsEnemy ? TEXT("Enemy") : TEXT("Ally"));
}