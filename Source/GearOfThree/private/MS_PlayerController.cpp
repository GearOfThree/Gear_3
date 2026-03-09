// Copyright Epic Games, Inc. All Rights Reserved.


#include "public/MS_PlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "GearOfThree.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AMS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
	
		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
	
		} else {
	
			UE_LOG(LogGearOfThree, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
	
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget<UUserWidget>(this, CrosshairWidgetClass);

		if (CrosshairWidget)
		{
			CrosshairWidget->AddToViewport();
			CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	if (GameOverUIClass)
	{
		GameOverUIWidget = CreateWidget<UUserWidget>(this, GameOverUIClass);
		
		if (GameOverUIWidget)
		{
			GameOverUIWidget->AddToViewport();
			GameOverUIWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	
	// Main UI 생성
	if (MainUIClass)
	{
		MainUIWidget = CreateWidget<UMS_MainUI01>(this, MainUIClass);

		if (MainUIWidget)
		{
			MainUIWidget->AddToViewport();
		}
	}

	// Pawn이 이미 저장돼 있으면 여기서 바인딩
	if (MainUIWidget && CachedPawn)
	{
		MainUIWidget->BindToPlayer(CachedPawn);
	}
}

void AMS_PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	CachedPawn = InPawn;

	if (MainUIWidget && InPawn)
	{
		MainUIWidget->BindToPlayer(InPawn);
	}
}

void AMS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AMS_PlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
