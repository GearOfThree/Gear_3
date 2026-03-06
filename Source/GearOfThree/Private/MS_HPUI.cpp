// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_HPUI.h"

#include "MS_Player.h"
#include "Components/ProgressBar.h"

void UMS_HPUI::UpdateHp(AActor* Actor, float CurrentHP, float MaxHP)
{
	if (!HPBar || MaxHP <= 0.f) return;
	UE_LOG(LogTemp, Warning, TEXT("UpdateHp Actor=%s HP=%.1f / %.1f"),
	*GetNameSafe(Actor), CurrentHP, MaxHP);
	
	// Actor 가 플레이어인지 확인한다. 
	if (!Actor->IsA(AMS_Player::StaticClass())) return;
	
	const float HpPercentage = FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
	HPBar->SetPercent(HpPercentage);
}
