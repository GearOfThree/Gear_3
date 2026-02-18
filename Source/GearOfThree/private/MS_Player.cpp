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
#include "AnimNodes/AnimNode_RandomPlayer.h"
#include "Blueprint/UserWidget.h"
#include "public/MS_PlayerController.h"
#include "public/MS_WeaponWheelWidget.h"
#include "public/MS_Weapon.h"
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
	
	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComp"));
	WeaponMeshComp->SetupAttachment(GetMesh(), TEXT("WeaponSocket")); // 손 소켓
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
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
	
	// 무기 스폰 방식 1
	// if (StarterWeaponClass)
	// {
	// 	CurrentWeapon = GetWorld()->SpawnActor<AMS_Weapon>(StarterWeaponClass);
	//
	// 	CurrentWeapon->AttachToComponent(
	// 		GetMesh(),
	// 		FAttachmentTransformRules::SnapToTargetIncludingScale,
	// 		TEXT("WeaponSocket")
	// 	);
	// }
	
	// 무기 스폰 방식 2
	for (const auto& Pair : WeaponClassMap)
	{
		const EWeaponDirection direction = Pair.Key;
		TSubclassOf<AMS_Weapon> WeaponClass = Pair.Value;
		
		// 클래스 확인
		if (!WeaponClass) continue;
		
		AMS_Weapon* weapon = GetWorld()->SpawnActor<AMS_Weapon>(WeaponClass);

		// 호출 확인
		if (!weapon) continue;
		
		weapon->SetOwner(this);
		weapon->SetActorHiddenInGame(true);
		weapon->SetActorEnableCollision(false);
		
		// 소켓에 모두 붙여놓고 숨겨 놓는다.
		weapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			TEXT("WeaponSocket")
		);
		
		// 일단 다 넣어 놓기
		WeaponMap.Add(direction, weapon);
	}
	
	
	
	
	// 시작 무기 선택
	if (WeaponMap.Contains(EWeaponDirection::Left))
	{
		EquipWeapon(EWeaponDirection::Left);
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
		
		// 무기 슬롯 보이기
		EnhancedInputComponent->BindAction(OpenWeaponSlotAction, ETriggerEvent::Started, this, &AMS_Player::ToggleWeaponSlot);
	}
	else
	{
		UE_LOG(LogGearOfThree, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

// 무기 교체
// void AMS_Player::ChangeWeapon(TSubclassOf<AMS_Weapon> NewWeaponClass)
// {
// 	if (CurrentWeapon)
// 	{
// 		CurrentWeapon->Destroy();
// 	}
//
// 	CurrentWeapon = GetWorld()->SpawnActor<AMS_Weapon>(NewWeaponClass);
//
// 	CurrentWeapon->AttachToComponent(
// 		GetMesh(),
// 		FAttachmentTransformRules::SnapToTargetIncludingScale,TEXT("WeaponSocket")
// 	);
// }

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

// ############################ 무기 선택 ############################

void AMS_Player::ToggleWeaponSlot()
{
	
	UE_LOG(LogTemp, Warning, TEXT("ToggleWeaponSlot CALLED"));
	bSlotOpen = !bSlotOpen;
	if (bSlotOpen)
	{
		ShowWeaponWheelUI(true);
		
	}
	else
	{
		ShowWeaponWheelUI(false);
		
	}
}

void AMS_Player::ShowWeaponWheelUI(bool bShow)
{
	AMS_PlayerController* playerController = Cast<AMS_PlayerController>(GetController());
	UE_LOG(LogTemp, Warning, TEXT("ShowWeaponWheelUI(%d) PC=%s"), bShow, *GetNameSafe(playerController));
	if (!playerController || !playerController->IsLocalController()) return;
	
	if (bShow)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponWheelWidgetClass=%s"), *GetNameSafe(WeaponWheelWidgetClass));
		
		if (!IsValid(WeaponWheelWidget) && WeaponWheelWidgetClass)
		{
			WeaponWheelWidget = CreateWidget<UMS_WeaponWheelWidget>(playerController, WeaponWheelWidgetClass);
		}
		
		if (WeaponWheelWidget)
		{
			TMap<EWeaponDirection, FMS_WeaponSlotData> DataMap;
			
			// 왼쪽
			{
				UTexture2D* ImageIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Miso/Gun/image/Gun01-AR15.Gun01-AR15'"));
				// UStaticMesh* GunMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Fab/AR-15_style_rifle/ar_15_style_rifle/StaticMeshes/ar_15_style_rifle.ar_15_style_rifle'"));
				
				FMS_WeaponSlotData data;
				data.Direction = EWeaponDirection::Left;
				data.DisplayName = FText::FromString(TEXT("AR15"));
				data.Icon = ImageIcon;
				data.WeaponClass = AR15WeaponClass;
				DataMap.Add(EWeaponDirection::Left, data);
			}
			
			// 오른쪽
			{
				UTexture2D* ImageIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Miso/Gun/image/Gun02-AK105.Gun02-AK105'"));
				// UStaticMesh* GunMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Fab/AK-105/AK105fbx.AK105fbx'"));
				
				FMS_WeaponSlotData data;
				data.Direction = EWeaponDirection::Right;
				data.DisplayName = FText::FromString(TEXT("AK105"));
				data.Icon = ImageIcon;
				data.WeaponClass = AK105WeaponClass;
				DataMap.Add(EWeaponDirection::Right, data);
			}
			
			WeaponSlotDataMap = DataMap;
			if (!WeaponWheelWidget->IsInViewport())
			{
				WeaponWheelWidget->AddToViewport(10);
			}
			WeaponWheelWidget->SetSlotDataMap(DataMap);
			UE_LOG(LogTemp, Warning, TEXT("AddedToViewport"));
			
			AMS_PlayerController* PlayerController = Cast<AMS_PlayerController>(GetController());
			if (PlayerController)
			{
				PlayerController->bShowMouseCursor = true;
				FInputModeGameAndUI Mode;
				Mode.SetWidgetToFocus(WeaponWheelWidget->TakeWidget());
				PlayerController->SetInputMode(Mode);
				PlayerController->SetIgnoreLookInput(true);
			}
			
			
			
		}
	}
	else
	{
		if (!WeaponWheelWidget) return;
		
		EWeaponDirection Direction = WeaponWheelWidget->GetSelectedDirection();
		WeaponWheelWidget->RemoveFromParent();
		AMS_PlayerController* PlayerController = Cast<AMS_PlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetIgnoreLookInput(false);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Wheel Direction = %d"), (int32)Direction);
		UE_LOG(LogTemp, Warning, TEXT("SelectedDirection=%d"), (int32)Direction);
		EquipWeapon(Direction);
	}
}

void AMS_Player::EquipWeapon(EWeaponDirection direction)
{
	// if (!WeaponMap.Contains(direction)) return;

	// AMS_Weapon* NewWeapon = WeaponMap[direction];
	// if (!NewWeapon) return;
	//
	// // 이미 장착하고 있는걸 또 선택하는 경우
	// if (CurrentWeapon == NewWeapon)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: Already equipped %s"), *GetNameSafe(NewWeapon));
	// 	return;
	// }
	//
	// if(CurrentWeapon)
	// {
	// 	CurrentWeapon->SetActorHiddenInGame(true);
	// }
	//
	// NewWeapon->SetActorHiddenInGame(false);
	// NewWeapon->AttachToComponent(
	// 	GetMesh(),
	// 	FAttachmentTransformRules::SnapToTargetIncludingScale,
	// 	TEXT("WeaponSocket")	
	// );
	//
	// CurrentWeapon = NewWeapon;
	
	if (!WeaponMeshComp) return;
	
	FMS_WeaponSlotData* Data = WeaponSlotDataMap.Find(direction);
	if (!Data || !Data->WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon failed. Dir=%d Mesh=%s"),
			(int32)direction, *GetNameSafe(Data ? Data->WeaponClass : nullptr));
		return;
	}
	
	// WeaponMeshComp->SetStaticMesh(Data->WeaponMesh);
	// UE_LOG(LogTemp, Warning, TEXT("Equipped: %s"), *Data->DisplayName.ToString());
	
	// 기존 무기 제거 
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	
	// 새 무기 스폰 
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();

	CurrentWeapon = GetWorld()->SpawnActor<AActor>(Data->WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	if (!IsValid(CurrentWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: Spawn failed"));
		return;
	}
	
	// 손 소켓에 부착
	USkeletalMeshComponent* MeshComp = GetMesh();
	static const FName WeaponSocketName(TEXT("WeaponSocket")); // 네 소켓명으로
	CurrentWeapon->AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponSocketName
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Equipped weapon: %s"), *Data->DisplayName.ToString());
}
