#pragma once

#include "CoreMinimal.h"
#include "MS_WeaponSlotData.h"
#include "public/WeaponDirection.h"
#include "Blueprint/UserWidget.h"
#include "MS_WeaponSlotWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSlotClicked, EWeaponDirection, Direction);

UCLASS()
class GEAROFTHREE_API UMS_WeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void ApplySlotData(FMS_WeaponSlotData& InData);
	
	
	// 클릭 이벤트를 부모에게 알림
	UPROPERTY(BlueprintAssignable)
	FOnWeaponSlotClicked OnSlotClicked;
	
	UFUNCTION(BlueprintCallable)
	void SetSelected(bool bSelected);
	
	UFUNCTION(BlueprintPure)
	EWeaponDirection GetDirection() const { return SlotDirection; }
	
	// 이 슬롯이 담당하는 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Slot")
	EWeaponDirection SlotDirection = EWeaponDirection::None;
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnInitialized() override;
	
	UFUNCTION()
	void HandleClicked();
private:
	
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Click = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UImage* Img_icon = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_name = nullptr;

	UPROPERTY(meta=(BindWidget))
	class UBorder* Bd_BG = nullptr;
};
