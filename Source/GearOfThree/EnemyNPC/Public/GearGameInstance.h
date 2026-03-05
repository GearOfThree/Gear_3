#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GearGameInstance.generated.h"

UCLASS()
class GEAROFTHREE_API UGearGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 게임 클리어 여부를 저장할 전역 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game State")
	bool bIsGameCleared = false;
};