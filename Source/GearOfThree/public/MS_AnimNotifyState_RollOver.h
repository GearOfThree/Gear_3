// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MS_AnimNotifyState_RollOver.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_AnimNotifyState_RollOver : public UAnimNotifyState
{
	GENERATED_BODY()
	
	
public:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation) override;
};
