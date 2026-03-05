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
	APlayerController* PC = GetOwningPlayer();
	
	if (PC)
	{
		// 1) UI 포커스/입력모드 되돌리기
		FInputModeGameOnly Mode;
		PC->SetInputMode(Mode);

		// 2) 커서 숨기기
		PC->SetShowMouseCursor(false);

		// 3) 일시정지 해제
		PC->SetPause(false);

		// (선택) 남아있는 입력/포커스 정리
		PC->FlushPressedKeys();
	}
	
	RemoveFromParent();
	
	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}

void UGameOverUI::OnQuit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(),GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
