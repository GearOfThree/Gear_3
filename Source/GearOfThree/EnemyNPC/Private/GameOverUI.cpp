// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverUI.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOverUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Btn_Restart->OnClicked.AddUniqueDynamic(this, &UGameOverUI::OnRestart);
	Btn_Quit->OnClicked.AddUniqueDynamic(this, &UGameOverUI::OnQuit);
}


void UGameOverUI::OnRestart()
{
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void UGameOverUI::OnQuit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
