// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_MainUI01.h"

#include "MS_HPComponent.h"

void UMS_MainUI01::BindToPlayer(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("BindToPlayer Called / Pawn=%s / PlayerHPBar=%s"),
		*GetNameSafe(InPawn),
		*GetNameSafe(PlayerHPBar));
	
	if (!InPawn || !PlayerHPBar) return;

	UMS_HPComponent* HPComponent = InPawn->FindComponentByClass<UMS_HPComponent>();
	if (!HPComponent) return;

	
	// UE_LOG(LogTemp, Warning, TEXT("Found HPComponent = %s"), *GetNameSafe(HPComponent));
	
	// 시작하자마자 현재값 1회 반영
	PlayerHPBar->UpdateHp(HPComponent->CurrentHP, HPComponent->MaxHP);

	// HP 바뀔 때마다 UI 갱신
	HPComponent->OnHpChanged.RemoveAll(PlayerHPBar);
	HPComponent->OnHpChanged.AddUObject(PlayerHPBar, &UMS_HPBarUI::UpdateHp);
	
	// UE_LOG(LogTemp, Warning, TEXT("AddUObject Complete"));
}
