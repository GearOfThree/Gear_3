// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "Blueprint/StateTreeConditionBlueprintBase.h"
#include "STC_MoveToShoot.generated.h"

UCLASS()
class GEAROFTHREE_API USTC_MoveToShoot : public UStateTreeConditionBlueprintBase
{
	GENERATED_BODY()
	
protected:
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
