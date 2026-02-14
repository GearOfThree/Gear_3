// Fill out your copyright notice in the Description page of Project Settings.


#include "STT_EnemyFire.h"
#include "EnemyNPCCharacter.h"
#include "StateTreeExecutionContext.h"
#include "WeaponComponent.h"
#include "Kismet/GameplayStatics.h"
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
	AEnemyNPCCharacter* Owner = Cast<AEnemyNPCCharacter>(Context.GetOwner());
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(Context.GetWorld(), 0);

	if (!Owner || !Player) return EStateTreeRunStatus::Failed;

	// 1. [거리 체크] 멀어지면 추격(Chase)으로 복귀
	float Distance = Owner->GetDistanceTo(Player);
	if (Distance > InstanceData.AttackRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("[STATE] Target out of range. Switching to Chase."));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Target Lost -> Chase"));
		Owner->bIsReloading = false; // 장전 중이었다면 캔슬하고 쫓아감
		return EStateTreeRunStatus::Succeeded; // -> ChaseState로 전환!
	}

	// 2. [회전] 항상 플레이어 바라보기
	FVector Start = Owner->GetActorLocation();
	FVector Target = Player->GetActorLocation();
	Target.Z = Start.Z; // 높이는 무시 (고개 들지 않게)
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	FRotator SmoothRotation = FMath::RInterpTo(Owner->GetActorRotation(), LookAtRotation, DeltaTime, 15.0f);
	Owner->SetActorRotation(SmoothRotation);

	// 3. [장전]
	if (Owner->bIsReloading)
	{
		FString ReloadMsg = FString::Printf(TEXT("Reloading... %.1f / %.1f"), InstanceData.CurrentReloadTime, InstanceData.ReloadTime);
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Orange, ReloadMsg);
		InstanceData.CurrentReloadTime += DeltaTime;
		if (InstanceData.CurrentReloadTime >= InstanceData.ReloadTime)
		{
			Owner->ReloadWeapon(); // 장전 완료
			// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Reload Complete!"));
		}
		return EStateTreeRunStatus::Running; // 장전 중엔 발사 안 함
	}

	// 4. [발사]
	if (Owner->CurrentAmmo <= 0)
	{
		Owner->bIsReloading = true; // 총알 바닥남 -> 장전 시작
		UE_LOG(LogTemp, Warning, TEXT("[COMBAT] Out of Ammo! Starting Reload."));
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Empty! Reloading..."));
		return EStateTreeRunStatus::Running;
	}

	InstanceData.TimeSinceLastFire += DeltaTime;
	if (InstanceData.TimeSinceLastFire >= InstanceData.FireInterval)
	{
		UWeaponComponent* WeaponComp = Owner->FindComponentByClass<UWeaponComponent>();
		if (WeaponComp)
		{
			WeaponComp->Fire(); // 발사!
			Owner->DecreaseAmmo();
			InstanceData.TimeSinceLastFire = 0.0f;
		}
	}

	return EStateTreeRunStatus::Running;
}