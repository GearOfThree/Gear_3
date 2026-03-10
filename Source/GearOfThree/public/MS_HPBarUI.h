// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "MS_HPBarUI.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_HPBarUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void UpdateHp(float CurrentHP, float MaxHP);

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* PB_HP;

	// UPROPERTY(meta = (BindWidgetOptional))
	// UTextBlock* Txt_HP;
};
