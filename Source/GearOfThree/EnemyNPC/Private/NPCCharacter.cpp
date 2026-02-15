// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"
#include "Components/StateTreeComponent.h"
#include "Kismet/GameplayStatics.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// StateTree 컴포넌트 생성
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	
	// 기본값 초기화
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
	
	// 기본 팀 설정 (나중에 블루프린트에서 변경 가능)
	TeamSide = ETeamSide::Ally; 
}

void ANPCCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 에디터(블루프린트)에서 수정한 MaxAmmo 값을 게임 시작 시 CurrentAmmo에 반영
	CurrentAmmo = MaxAmmo;
}

void ANPCCharacter::DecreaseAmmo()
{
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
	}
}

void ANPCCharacter::ReloadWeapon()
{
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
}

AActor* ANPCCharacter::FindClosestEnemy()
{
	AActor* ClosestEnemy = nullptr;
	float MinDistance = MAX_FLT;

	// 월드 내 모든 ANPCCharacter를 찾음 (나중에 팀원이 만든 DamageComponent 기반으로 확장 가능)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCCharacter::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		ANPCCharacter* OtherNPC = Cast<ANPCCharacter>(Actor);
		if (!OtherNPC || OtherNPC == this) continue;

		// 팀이 다를 때만 적으로 간주
		if (OtherNPC->TeamSide != this->TeamSide)
		{
			float Distance = GetDistanceTo(OtherNPC);
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestEnemy = OtherNPC;
			}
		}
	}
	return ClosestEnemy;
}
