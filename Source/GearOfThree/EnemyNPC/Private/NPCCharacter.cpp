// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
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

	// 1. 월드에 있는 모든 NPC를 검색
	TArray<AActor*> AllNPCs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ANPCCharacter::StaticClass(), AllNPCs);

	for (AActor* Actor : AllNPCs)
	{
		ANPCCharacter* NPC = Cast<ANPCCharacter>(Actor);
        
		// 2. 나 자신은 무시 (유효성 검사 포함)
		if (!NPC || NPC == this) continue;

		// 🚨 3. [핵심 추가] 죽은 적은 철저하게 무시합니다!
		// (방금 헤더 파일에 선언한 bIsDead 변수를 체크합니다)
		if (NPC->bIsDead) continue; 

		// 4. 나와 팀이 다른 경우만 적군으로 간주
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

void ANPCCharacter::Die()
{
	// 이미 죽었다면 중복 실행 방지
	if (bIsDead) return;
	bIsDead = true;

	// 1. 타겟팅 제외 (충돌 끄기)
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 2. AI 정지 (StateTree 전용)
	// 비헤이비어 트리가 아니므로, 작성해두신 StateTreeComponent를 직접 멈춥니다!
	if (StateTreeComponent)
	{
		StateTreeComponent->StopLogic(TEXT("Character Died"));
	}
    
	// 이동하던 발걸음 멈추기
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		AIC->StopMovement(); 
	}

	// 3. 사망 애니메이션 재생 및 파괴 타이머 설정
	float DeathAnimDuration = 3.0f; // 몽타주가 없을 때의 기본 대기 시간

	if (DeathMontage)
	{
		DeathAnimDuration = PlayAnimMontage(DeathMontage);
	}

	// 4. 애니메이션이 끝날 때쯤 시체 증발(Destroy)
	GetWorld()->GetTimerManager().SetTimer(
		DeathTimerHandle, 
		this, 
		&ANPCCharacter::DestroyAfterDeath, 
		DeathAnimDuration, 
		false
	);
}

void ANPCCharacter::DestroyAfterDeath()
{
	Destroy();
}
