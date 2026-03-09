// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_HPBarUI.h"
#include "Blueprint/UserWidget.h"
#include "MS_MainUI01.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_MainUI01 : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void BindToPlayer(APawn* InPawn);

protected:
	UPROPERTY(meta = (BindWidget))
	UMS_HPBarUI* PlayerHPBar;
	
};
