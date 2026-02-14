// GearOfThreeTypes.h
#pragma once

#include "CoreMinimal.h"
#include "GearOfThreeTypes.generated.h" // UENUM 등을 쓴다면 필수

// 팀 구분용 Enum
UENUM(BlueprintType)
enum class ETeamSide : uint8
{
	Player  UMETA(DisplayName = "Player"),
	Enemy   UMETA(DisplayName = "Enemy"),
	Neutral UMETA(DisplayName = "Neutral")
};