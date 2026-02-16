// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "public/WeaponDirection.h"
#include "MS_WeaponWidget.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_WeaponWidget : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	EWeaponDirection GetSelectedDirection() const { return SelectedDirection; }
	
private:
	EWeaponDirection SelectedDirection = EWeaponDirection::None;

	float DeadZone = 60.f;  // ???
};
