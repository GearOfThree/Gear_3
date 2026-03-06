// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_MainUI.h"
#include "MS_HPUI.h"
#include "MS_Player.h"

void UMS_MainUI::BindToHpComponent(APawn* Pawn)
{
	if (!Pawn || !PlayerHPUI) return;
	
	AActor* Player = Pawn;
	
	UMS_HPComponent* HPComponent = Pawn->FindComponentByClass<UMS_HPComponent>();
	
	if (!HPComponent) return;
	
	PlayerHPUI->UpdateHp(Player, HPComponent->CurrentHP, HPComponent->MaxHP);
	
	HPComponent->OnHpChanged.AddUObject(PlayerHPUI, &UMS_HPUI::UpdateHp);
}
