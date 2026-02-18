#include "MS_WeaponSlotWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"


void UMS_WeaponSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("SlotWidget Constructed: Img_icon=%s Txt_name=%s"),
		*GetNameSafe(Img_icon), *GetNameSafe(Txt_name));
}

void UMS_WeaponSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (Btn_Click)
	{
		Btn_Click->OnClicked.AddDynamic(this, &UMS_WeaponSlotWidget::HandleClicked);		
	}
	
	UE_LOG(LogTemp, Warning, TEXT("BTN_Click=%s IMG_Icon=%s TXT_Name=%s"),
	*GetNameSafe(Btn_Click), *GetNameSafe(Img_icon), *GetNameSafe(Txt_name));
}

void UMS_WeaponSlotWidget::ApplySlotData(FMS_WeaponSlotData& InData)
{
	UE_LOG(LogTemp, Warning, TEXT("ApplySlotData this=%s Img_icon=%p Txt_name=%p"),
	*GetNameSafe(this), Img_icon, Txt_name);
	
	SlotDirection = InData.Direction;
	if (IsValid(Img_icon) && IsValid(InData.Icon))
	{
		Img_icon->SetBrushFromTexture(InData.Icon);
	}
	if (IsValid(Txt_name))
	{
		Txt_name->SetText(InData.DisplayName);
	}
}

void UMS_WeaponSlotWidget::SetSelected(bool bSelected)
{
	
	// ✅ 바인딩 안 됐으면 그냥 무시
	if (!IsValid(Bd_BG))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetSelected: BD_BG is null (%s)"), *GetNameSafe(this));
		return;
	}
	
	// 간단 하이라이트(색만 바꿈)
	const FLinearColor Normal(0.08f, 0.08f, 0.08f, 0.85f);
	const FLinearColor Selected(0.30f, 0.30f, 0.30f, 1.00f);

	Bd_BG->SetBrushColor(bSelected ? Selected : Normal);
}

void UMS_WeaponSlotWidget::HandleClicked()
{
	OnSlotClicked.Broadcast(SlotDirection);
}
