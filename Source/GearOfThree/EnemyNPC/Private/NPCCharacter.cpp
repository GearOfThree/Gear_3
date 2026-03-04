// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"
#include "Components/StateTreeComponent.h"
#include "Kismet/GameplayStatics.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	// StateTree 컴포넌트 생성
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	
	// 기본값 초기화
	MaxAmmo = 10;
	CurrentAmmo = MaxAmmo;
	bIsReloading = false;
	
	// 기본 팀 설정
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
	float MinDistance = FLT_MAX;

	// 1. 월드에 있는 모든 NPC를 검색 (플레이어 제외)
	TArray<AActor*> AllNPCs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCCharacter::StaticClass(), AllNPCs);

	for (AActor* Actor : AllNPCs)
	{
		ANPCCharacter* NPC = Cast<ANPCCharacter>(Actor);
        
		// 2. 나 자신은 무시
		if (!NPC || NPC == this) continue;

		// 3. [중요] 죽은 적은 무시 (나중에 체력 0되면 IsDead=true로 설정 필요)
		// if (NPC->IsDead()) continue; 

		// 나와 팀이 다른 경우만 적군으로 간주
		// (TeamSide가 다르면 적으로 인식)
		if (this->IsHostile(NPC)) 
		{
			float Dist = GetDistanceTo(NPC);
			if (Dist < MinDistance)
			{
				MinDistance = Dist;
				ClosestEnemy = NPC;
			}
		}
	}

	return ClosestEnemy;
}
