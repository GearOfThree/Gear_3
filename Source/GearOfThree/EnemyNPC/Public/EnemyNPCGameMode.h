// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnemyNPCGameMode.generated.h"

/**
 * 
 */
UCLASS(abstract)
class AEnemyNPCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	/** Constructor */
	AEnemyNPCGameMode();
};
