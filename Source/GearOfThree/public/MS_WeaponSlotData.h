#pragma once

#include "CoreMinimal.h"
#include "WeaponDirection.h"
#include "Engine/Texture2D.h"
#include "MS_WeaponSlotData.generated.h"

USTRUCT(BlueprintType)
struct FMS_WeaponSlotData
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponDirection Direction = EWeaponDirection::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon = nullptr;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TObjectPtr<AMS_Weapon> WeaponMesh = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> WeaponClass;
	
	// 손 위치도 저장!!
	
};
