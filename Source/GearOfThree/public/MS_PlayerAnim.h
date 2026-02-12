// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MS_PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_PlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float Speed; // default 값을 지정을 해줘야지 움직일 수 있다. 
	
};
