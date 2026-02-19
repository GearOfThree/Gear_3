// Fill out your copyright notice in the Description page of Project Settings.


#include "STT_EnemyFire.h"

#include "AIController.h"
#include "EnemyNPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "NPCCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


EStateTreeRunStatus FSTT_EnemyFire::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	AEnemyNPCCharacter* Owner = Cast<AEnemyNPCCharacter>(Context.GetOwner());

	// 상태 진입 시 타이머 초기화
	InstanceData.TimeSinceLastFire = InstanceData.FireInterval; 
	InstanceData.CurrentReloadTime = 0.0f;

	if (Owner)
	{
		// 🚨 [핵심 해결 부분: 물리적 브레이크와 AI 정지] 🚨
		if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
		{
			AIC->StopMovement(); // 1. 길찾기 이동 명령 취소
		}

		if (UCharacterMovementComponent* MoveComp = Owner->GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately(); // 2. 물리적 관성 즉시 정지 (문워크 방지)
			MoveComp->Velocity = FVector::ZeroVector; // 3. 남은 속도 0으로 완벽 초기화
			MoveComp->bOrientRotationToMovement = false; // 4. 이동 방향 쳐다보기 기능 끄기
		}

		Owner->bUseControllerRotationYaw = true; // 5. 조준을 위해 컨트롤러 방향 사용 켜기

		// [수정] 들어오자마자 장전 중이라면 해제 (총알이 1발이라도 있다면)
		if (Owner->CurrentAmmo > 0)
		{
			Owner->bIsReloading = false;
		}
        
		UE_LOG(LogTemp, Log, TEXT("=== [FIRE] Enter FireState (Ammo: %d) ==="), Owner->CurrentAmmo);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_EnemyFire::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	ANPCCharacter* Owner = Cast<ANPCCharacter>(Context.GetOwner());
	
	// Evaluator가 골라준 타겟을 사용합니다.
	AActor* Target = InstanceData.TargetActor; 

	if (!Owner || !Target) return EStateTreeRunStatus::Failed;

	// 1. 타겟과의 거리 체크 (멀어지면 추격 상태로 복귀)
	float Distance = Owner->GetDistanceTo(Target);
	
	// 사거리보다 200 더 멀어져야만 다시 쫓아갑니다. (여유 공간 부여)
	if (Distance > (InstanceData.AttackRange + 200.0f))
	{
		return EStateTreeRunStatus::Succeeded; // Chase 상태로 전환 유도
	}

	// 2. 타겟 바라보기 (부드러운 회전)
	FVector Start = Owner->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();
	TargetLoc.Z = Start.Z; 
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, TargetLoc);
	FRotator SmoothRotation = FMath::RInterpTo(Owner->GetActorRotation(), LookAtRotation, DeltaTime, 15.0f);
	Owner->SetActorRotation(SmoothRotation);

	// 3. 장전 및 사격 로직 (부모 클래스의 변수 활용)
	if (Owner->bIsReloading)
	{
		InstanceData.CurrentReloadTime += DeltaTime;
		if (InstanceData.CurrentReloadTime >= InstanceData.ReloadTime)
		{
			Owner->ReloadWeapon();
		}
		return EStateTreeRunStatus::Running;
	}

	if (Owner->CurrentAmmo <= 0)
	{
		Owner->bIsReloading = true;
		return EStateTreeRunStatus::Running;
	}

	InstanceData.TimeSinceLastFire += DeltaTime;
	if (InstanceData.TimeSinceLastFire >= InstanceData.FireInterval)
	{
		UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>();
		if (WeaponComp)
		{
			WeaponComp->Fire(); //
			Owner->DecreaseAmmo();
			InstanceData.TimeSinceLastFire = 0.0f;
		}
	}

	return EStateTreeRunStatus::Running;
}