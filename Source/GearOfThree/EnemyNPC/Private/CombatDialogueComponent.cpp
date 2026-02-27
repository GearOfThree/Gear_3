// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatDialogueComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCombatDialogueComponent::UCombatDialogueComponent()
{

	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatDialogueComponent::PlayDialogue(const FDialogueData& DialogueData)
{
	if (DialogueData.VoiceSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), DialogueData.VoiceSound);
	}
	
	if (OnSubtitleRequested.IsBound())
	{
		OnSubtitleRequested.Broadcast(DialogueData.SubtitleText, DialogueData.Duration);
	}
}

void UCombatDialogueComponent::StartCombatDialogue()
{
	if (CombatDialogueSequence.Num() == 0) return;
	
	CurrentDialogueIndex = 0;
	
	GetWorld()->GetTimerManager().SetTimer(DialogueTimerHandle, this, &UCombatDialogueComponent::PlayNextDialogueLine, 3.0f, false);
}

void UCombatDialogueComponent::PlayNextDialogueLine()
{
	if (CurrentDialogueIndex < CombatDialogueSequence.Num())
	{
		FDialogueData CurrentData = CombatDialogueSequence[CurrentDialogueIndex];
		
		PlayDialogue(CurrentData);
		
		CurrentDialogueIndex++;
		
		float NextDelay = CurrentData.Duration + 1.0f;
		
		GetWorld()->GetTimerManager().SetTimer(DialogueTimerHandle, this, &UCombatDialogueComponent::PlayNextDialogueLine, NextDelay, false);
	}else
	{
		GetWorld()->GetTimerManager().ClearTimer(DialogueTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[Dialogue] 모든 전투 대사 재생 완료"));
	}
	
}

