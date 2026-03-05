// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverUI.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UGameOverUI : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeOnInitialized() override;
	
public:
	// WBP_GameOver에서 위젯 연결
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Restart;
	
	// WBP_GameOver에서 위젯 연결
	UPROPERTY(meta=(BindWidget))
	UButton* Btn_Quit;
	
	UFUNCTION()
	void OnRestart();
	
	UFUNCTION()
	void OnQuit();
};
