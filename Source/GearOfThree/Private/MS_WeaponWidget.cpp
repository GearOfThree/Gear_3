// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_WeaponWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UMS_WeaponWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D Center = ViewportSize * 0.5f;
	FVector2D Delta = MousePos - Center;

	if (FMath::Abs(Delta.X) < DeadZone && FMath::Abs(Delta.Y) < DeadZone)
	{
		SelectedDirection = EWeaponDirection::None;
		return;
	}

	if (FMath::Abs(Delta.X) > FMath::Abs(Delta.Y))
	{
		SelectedDirection = (Delta.X > 0) ?
			EWeaponDirection::Right :
			EWeaponDirection::Left;
	}
	else
	{
		SelectedDirection = (Delta.Y > 0) ?
			EWeaponDirection::Down :
			EWeaponDirection::Up;
	}
}
