// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MS_MainUI.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_MainUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta=(BindWidget))
	class UMS_HPUI* PlayerHPUI;
	
	void BindToHpComponent(APawn* Pawn);
	
	
};
