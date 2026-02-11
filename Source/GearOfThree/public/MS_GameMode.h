// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MS_GameMode.generated.h"

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AMS_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AMS_GameMode();
};



