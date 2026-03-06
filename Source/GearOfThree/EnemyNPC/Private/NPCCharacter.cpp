// Fill out your copyright notice in the Description page of Project Settings.

#include "NPCCharacter.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StateTreeComponent.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f;
	// StateTree 컴포넌트 생성
	StateTreeComponent = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponent"));
	
	// 기본 팀 설정
	TeamSide = ETeamSide::Ally; 
}

AActor* ANPCCharacter::FindClosestEnemy()
{
	AActor* ClosestEnemy = nullptr;
    
	// 루트 연산을 없애기 위해 '제곱된 거리'의 최댓값으로 초기화합니다.
	float MinDistanceSq = FLT_MAX; 

	FVector MyLocation = GetActorLocation();
	float SearchRadius = 10000.0f; // 탐색 반경 100미터 (원하는 시야거리에 맞춰 조절하세요!)

	// 충돌 결과를 담을 배열
	TArray<FOverlapResult> OverlapResults;
    
	// NPC들이 속한 콜리전 채널을 지정 (보통 캐릭터는 Pawn 채널을 씁니다)
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	// 나 자신은 처음부터 검색에서 제외시켜 버립니다!
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); 

	// 맵 전체가 아니라, 내 주변 50미터 반경 안의 액터만 긁어옵니다
	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		MyLocation,
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(SearchRadius),
		QueryParams
	);

	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			ANPCCharacter* TargetNPC = Cast<ANPCCharacter>(Result.GetActor());

			// 유효성 검사 및 죽은 적 무시
			if (!TargetNPC || TargetNPC->bIsDead) continue; 

			// 적군인지 확인
			if (this->IsHostile(TargetNPC)) 
			{
				// GetDistanceTo 대신, 연산이 훨씬 가벼운 SizeSquared(제곱 거리)를 사용
				float DistSq = (MyLocation - TargetNPC->GetActorLocation()).SizeSquared();
                
				if (DistSq < MinDistanceSq)
				{
					MinDistanceSq = DistSq;
					ClosestEnemy = TargetNPC;
				}
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

	// AI 정지 (StateTree 전용)
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

	// 사망 애니메이션 재생 및 파괴 타이머 설정
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
	this->Destroy();
}
