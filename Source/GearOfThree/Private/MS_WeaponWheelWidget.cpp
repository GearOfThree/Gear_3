// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_WeaponWheelWidget.h"
#include "MS_WeaponSlotWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Button.h"

void UMS_WeaponWheelWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	// // // 슬롯 클릭 이벤트 바인딩
	// if (Slot_Left)  Slot_Left->OnSlotClicked.AddDynamic(this, &UMS_WeaponWheelWidget::HandleSlotClicked);
	// if (Slot_Right) Slot_Right->OnSlotClicked.AddDynamic(this, &UMS_WeaponWheelWidget::HandleSlotClicked);
	
	// RefreshSlots();
	
	UE_LOG(LogTemp, Warning, TEXT("Slots: L=%s R=%s"),
	*GetNameSafe(Slot_Left), *GetNameSafe(Slot_Right));
}

void UMS_WeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// // 슬롯 클릭 이벤트 바인딩
	if (Slot_Left)
	{
		Slot_Left->OnSlotClicked.RemoveAll(this);
		Slot_Left->OnSlotClicked.AddDynamic(this, &UMS_WeaponWheelWidget::HandleSlotClicked);
		
	}
	if (Slot_Right)
	{
		Slot_Right->OnSlotClicked.RemoveAll(this);
		Slot_Right->OnSlotClicked.AddDynamic(this, &UMS_WeaponWheelWidget::HandleSlotClicked);
		
	}
	
	bReady = true;

	// Construct 이후에 한 번만 처리
	if (bPendingRefresh)
	{
		RefreshSlots();
		bPendingRefresh = false;
	}
}

void UMS_WeaponWheelWidget::NativeDestruct()
{
	if (Slot_Left)  Slot_Left->OnSlotClicked.RemoveAll(this);
	if (Slot_Right) Slot_Right->OnSlotClicked.RemoveAll(this);

	bReady = false;
	bPendingRefresh = false;

	Super::NativeDestruct();
}

void UMS_WeaponWheelWidget::SetSlotDataMap(const TMap<EWeaponDirection, FMS_WeaponSlotData>& InMap)
{
	SlotDataMap = InMap;
	if (!bReady)
	{
		bPendingRefresh = true;   // 아직 준비 안 됐으면 미뤄둠
		return;
	}

	RefreshSlots();
}

void UMS_WeaponWheelWidget::HandleSlotClicked(EWeaponDirection Dir)
{
	SelectedDirection = Dir;
	RefreshSlots(); // 하이라이트 갱신
	
	// "클릭 즉시 교체"를 원하면 아래도 켜면 됨
	// OnDirectionSelected.Broadcast(Dir);
	UE_LOG(LogTemp, Warning, TEXT("Slot clicked Dir=%d"), (int32)Dir);
}

void UMS_WeaponWheelWidget::RefreshSlots()
{
	auto Apply = [&](UMS_WeaponSlotWidget* SlotWidget, EWeaponDirection Dir)
	{
		if (!SlotWidget) return;

		if (FMS_WeaponSlotData* Data = SlotDataMap.Find(Dir))
		{
			SlotWidget->ApplySlotData(*Data);
		}
		// SlotWidget 내부에서 null 체크하더라도 여기서도 한 번 더 안전
		SlotWidget->SetSelected(SelectedDirection == Dir);
	};

	Apply(Slot_Left,  EWeaponDirection::Left);
	Apply(Slot_Right, EWeaponDirection::Right);
}