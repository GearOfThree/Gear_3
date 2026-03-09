// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_HPBarUI.h"



void UMS_HPBarUI::UpdateHp(float CurrentHP, float MaxHP)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdateHp Called / CurrentHP=%.1f MaxHP=%.1f PB_HP=%s"),
		CurrentHP,
		MaxHP,
		*GetNameSafe(PB_HP));
	
	if (!PB_HP || MaxHP <= 0.f) return;

	const float Percent = FMath::Clamp(CurrentHP / MaxHP, 0.f, 1.f);
	
	UE_LOG(LogTemp, Warning, TEXT("SetPercent = %.2f"), Percent);
	PB_HP->SetPercent(Percent);

	if (Txt_HP)
	{
		Txt_HP->SetText(
			FText::FromString(
				FString::Printf(TEXT("%.0f / %.0f"), CurrentHP, MaxHP)
			)
		);
	}
}
