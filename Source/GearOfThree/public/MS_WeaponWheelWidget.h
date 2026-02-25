// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MS_WeaponSlotData.h"
#include "Blueprint/UserWidget.h"
#include "public/WeaponDirection.h"
#include "MS_WeaponWheelWidget.generated.h"

/**
 * 
 */
// DECLARE_MULTICAST_DELEGATE_OneParam(FOnDirectionSelected, EWeaponDirection);
UCLASS()
class GEAROFTHREE_API UMS_WeaponWheelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 플레이어나 외부에서 슬롯 데이터 세팅
	UFUNCTION(BlueprintCallable)
	void SetSlotDataMap(const TMap<EWeaponDirection, FMS_WeaponSlotData>& InMap);
	
	UFUNCTION(BlueprintPure)
	EWeaponDirection GetSelectedDirection() const { return SelectedDirection; }

protected:
	virtual void NativeOnInitialized() override;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	UPROPERTY()
	bool bPendingRefresh = false;
	
	UPROPERTY()
	bool bReady = false;
private:
	void RefreshSlots();

	UFUNCTION()
	void HandleSlotClicked(EWeaponDirection Dir);
	
private:
	// 에디터(BP)에서 넣거나, 플레이어가 SetSlotDataMap으로 넣을 수 있음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess="true"))
	TMap<EWeaponDirection, FMS_WeaponSlotData> SlotDataMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	EWeaponDirection SelectedDirection = EWeaponDirection::None;

	UPROPERTY(meta=(BindWidget))
	class UMS_WeaponSlotWidget* Slot_Left = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	class UMS_WeaponSlotWidget* Slot_Right = nullptr;
	
	
};
