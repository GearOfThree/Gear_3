// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MS_HPUI.generated.h"

class UMS_HPComponent;
/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_HPUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> HPBar;
	
	void UpdateHp(AActor* Actor, float CurrentHP, float MaxHP);
};
