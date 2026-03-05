// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_AnimNotifyState_RollOver.h"

#include "MS_Player.h"

void UMS_AnimNotifyState_RollOver::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	UE_LOG(LogTemp, Warning, TEXT("[MS_AnimNotifyState] Begin Owner=%s Anim=%s"),
		*GetNameSafe(MeshComp ? MeshComp->GetOwner() : nullptr),
		*GetNameSafe(Animation));
	
	if (!MeshComp) return;
	
	if (AMS_Player* Player = Cast<AMS_Player>(MeshComp->GetOwner()))
	{
		Player->OnRollStart();
	}
}

void UMS_AnimNotifyState_RollOver::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	UE_LOG(LogTemp, Warning, TEXT("[MS_AnimNotifyState] End Owner=%s Anim=%s"),
		*GetNameSafe(MeshComp ? MeshComp->GetOwner() : nullptr),
		*GetNameSafe(Animation));
	
	if (!MeshComp) return;
	
	if (AMS_Player* Player = Cast<AMS_Player>(MeshComp->GetOwner()))
	{
		Player->OnRollEnd();
	}
}
