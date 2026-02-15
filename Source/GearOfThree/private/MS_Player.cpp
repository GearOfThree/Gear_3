// Copyright Epic Games, Inc. All Rights Reserved.

#include "public/MS_Player.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GearOfThree.h"
#include "public/MS_PlayerController.h"
#include "Tools/UEdMode.h"

AMS_Player::AMS_Player()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	TargetArmLength = HipArmLength;
	TargetSocketOffset = HipSocketOffset;
	TargetFOV = HipFOV;
	
}

void AMS_Player::BeginPlay()
{
	Super::BeginPlay();
	
	if (AMS_PlayerController* player = Cast<AMS_PlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem 
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(player->GetLocalPlayer()))
		{
			if(DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
		
	}
}

void AMS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// InterpTo 을 사용하기 때문에 초점 변경되는 속도록 조절할 수 있음
	// Arm Length 보간
	float NewArm = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		InterpSpeed
	);
	CameraBoom->TargetArmLength = NewArm;

	// SocketOffset 보간
	FVector NewOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		TargetSocketOffset,
		DeltaTime,
		InterpSpeed
	);
	CameraBoom->SocketOffset = NewOffset;

	// FOV 보간
	float NewFOV = FMath::FInterpTo(
		FollowCamera->FieldOfView,
		TargetFOV,
		DeltaTime,
		InterpSpeed
	);
	FollowCamera->SetFieldOfView(NewFOV);
}



void AMS_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMS_Player::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMS_Player::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMS_Player::Look);
	
		// Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMS_Player::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMS_Player::StopSprint);
		
		// Crouch(엄패 기능을 위해 필요함)
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMS_Player::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMS_Player::StopCrouch);
	
		// ZoomIn // 토글 방식이 아닌 홀드 방식으로 설정
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Started, this, &AMS_Player::StartADS);
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Completed, this, &AMS_Player::StopADS);
		
	}
	else
	{
		UE_LOG(LogGearOfThree, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMS_Player::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMS_Player::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMS_Player::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMS_Player::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMS_Player::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMS_Player::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

// 
void AMS_Player::Sprint(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();
	
	if (bIsPressed && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		
	}
}

// 원래 속도로 돌아옴
void AMS_Player::StopSprint()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}	
}



void AMS_Player::StartCrouch(const FInputActionValue& Value)
{
	Crouch(); 
}

void AMS_Player::StopCrouch()
{
	UnCrouch(); 
}

void AMS_Player::SetADS(bool bNewADS)
{
	bIsADS = bNewADS;

	if (bIsADS)
	{
		TargetArmLength = ADSArmLength;
		TargetSocketOffset = ADSSocketOffset;
		TargetFOV = ADSFOV;

		// 조준 중 캐릭터가 카메라 방향으로 회전
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		TargetArmLength = HipArmLength;
		TargetSocketOffset = HipSocketOffset;
		TargetFOV = HipFOV;

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AMS_Player::StartADS()
{
	SetADS(true);
}

void AMS_Player::StopADS()
{
	SetADS(false);
}