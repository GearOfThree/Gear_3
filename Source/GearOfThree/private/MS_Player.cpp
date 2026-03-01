// Copyright Epic Games, Inc. All Rights Reserved.

#include "public/MS_Player.h"

#include "CombatDialogueComponent.h"
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
#include "Blueprint/UserWidget.h"
#include "public/MS_PlayerController.h"
#include "public/MS_WeaponWheelWidget.h"
#include "public/MS_Weapon.h"



AMS_Player::AMS_Player()
{
	// 틱설정 활성화
	PrimaryActorTick.bCanEverTick = true;
	
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
	
	// 총 스케레탈 메시 컴포넌트 등록
	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMeshComp"));
	WeaponMeshComp->SetupAttachment(GetMesh(), TEXT("WeaponSocket")); // 손 소켓
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	TeamSide = ETeamSide::Player;
	
	CombatDialogueComponent = CreateDefaultSubobject<UCombatDialogueComponent>(TEXT("CombatDialogue"));
}

void AMS_Player::BeginPlay()
{
	// #################### Mapping Context 관련 ####################
	
	UE_LOG(LogTemp, Warning, TEXT("AMS_Player BeginPlay"));
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

	// #################### 초점 변경 관련 ####################
	
	// 초기의 초점 내용을 저장한다. 

	CameraBoom->TargetArmLength = DefaultArmLength;
	CameraBoom->SocketOffset = DefaultSocketOffset;
	FollowCamera->SetFieldOfView(DefaultFOV);

	TargetArmLength = DefaultArmLength;
	TargetSocketOffset = DefaultSocketOffset;
	TargetFOV = DefaultFOV;
	
	CachedDefaults_Base();
	
	// #################### 무기 관련 ####################
	
	ResolveWeaponMeshComponent();
	
	if (WeaponMeshComp)
	{
		WeaponBaseLoc = WeaponMeshComp->GetRelativeLocation();
		WeaponBaseRot = WeaponMeshComp->GetRelativeRotation();
	}
	
	TMap<EWeaponDirection, FMS_WeaponSlotData> DataMap;
	
	{
		UTexture2D* ImageIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Miso/Gun/image/Gun03-K9Rav.Gun03-K9Rav'"));
				
		FMS_WeaponSlotData data;
		data.Direction = EWeaponDirection::Left;
		data.DisplayName = FText::FromString(TEXT("K9_Rav"));
		data.Icon = ImageIcon;
		data.WeaponClass = K9RavClass;
		DataMap.Add(EWeaponDirection::Left, data);
	}
	
	WeaponSlotDataMap = DataMap;
	
	FirstWeaponSpawn(EWeaponDirection::Left);
	
	// #################### 플레이중 음성 자막 설정 ####################
	
	if (CombatDialogueComponent)
	{
		CombatDialogueComponent->StartCombatDialogue();
	}
}

void AMS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!CameraBoom || !FollowCamera)
		return;

	if (bWantsADS)
	{
		TargetFOV = ADSFOV;
		TargetArmLength = ADSArmLength;
		TargetSocketOffset = ADSSocketOffset;
	} else
	{
		TargetFOV = DefaultFOV;
		TargetArmLength = DefaultArmLength;
		TargetSocketOffset = DefaultSocketOffset;
	}
	
	// 1) FOV 보간 (초점 변화 느낌의 핵심)
	const float NewFOV = FMath::FInterpTo(FollowCamera->FieldOfView, TargetFOV, DeltaTime, ADSInterpSpeed);
	FollowCamera->SetFieldOfView(NewFOV);

	// 2) 스프링암 길이 보간 (카메라 거리)
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, TargetArmLength, DeltaTime, ADSInterpSpeed);

	// 3) 스프링암 소켓 오프셋 보간 (어깨 시점 이동)
	CameraBoom->SocketOffset = FMath::VInterpTo(CameraBoom->SocketOffset, TargetSocketOffset, DeltaTime, ADSInterpSpeed);

	DecaySustainTick(DeltaTime);
	ApplyCameraRecoilTick(DeltaTime);
	ApplyWeaponKickTick(DeltaTime);
	PlayPendingShake();
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
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Started,   this, &AMS_Player::StartADS);
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Completed, this, &AMS_Player::StopADS);
		EnhancedInputComponent->BindAction(ZoomInAction, ETriggerEvent::Canceled,  this, &AMS_Player::StopADS);
		
		// 무기 슬롯 보이기
		EnhancedInputComponent->BindAction(OpenWeaponSlotAction, ETriggerEvent::Started, this, &AMS_Player::ToggleWeaponSlot);
	
		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMS_Player::HandleFire);
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

void AMS_Player::StartADS()
{
	CachedDefaults_Base();

	bWantsADS = true;
	UE_LOG(LogTemp, Warning, TEXT("ADS=%d"), bWantsADS);
	
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	if (AMS_PlayerController* PlayerController = Cast<AMS_PlayerController>(GetController()))
	{
		if (PlayerController->CrosshairWidget)
		{
			PlayerController->CrosshairWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	
}

void AMS_Player::StopADS()
{
	CachedDefaults_Base();
	
	// 플레그 값
	bWantsADS = false;
	UE_LOG(LogTemp, Warning, TEXT("ADS=%d"), bWantsADS);
	
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	if (AMS_PlayerController* PlayerController = Cast<AMS_PlayerController>(GetController()))
	{
		if (PlayerController->CrosshairWidget)
		{
			PlayerController->CrosshairWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMS_Player::CachedDefaults_Base()
{
	if (bCached || !CameraBoom || !FollowCamera) return;

	CachedArmLength = CameraBoom->TargetArmLength;
	CachedSocketOffset = CameraBoom->SocketOffset;
	CachedFOV = FollowCamera->FieldOfView;

	bCached = true;
}

void AMS_Player::ResolveWeaponMeshComponent()
{
	UActorComponent* Component = GetComponentByClass(UStaticMeshComponent::StaticClass());
	if (Component->GetName().Contains(TEXT("WeaponMeshComp")))
	{
		WeaponMeshComp = Cast<UStaticMeshComponent>(Component);
		
	}
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
				UTexture2D* ImageIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Miso/Gun/image/Gun03-K9Rav.Gun03-K9Rav'"));
				// UStaticMesh* GunMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Script/Engine.StaticMesh'/Game/Fab/AR-15_style_rifle/ar_15_style_rifle/StaticMeshes/ar_15_style_rifle.ar_15_style_rifle'"));
				
				FMS_WeaponSlotData data;
				data.Direction = EWeaponDirection::Left;
				data.DisplayName = FText::FromString(TEXT("K9_Rav"));
				data.Icon = ImageIcon;
				data.WeaponClass = K9RavClass;
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
			
			WeaponSlotDataMap = DataMap; // 데이터 백업
			
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
	if (!WeaponMeshComp) return;
	
	FMS_WeaponSlotData* Data = WeaponSlotDataMap.Find(direction);
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon successed. Dir=%d Mesh=%s"),
			(int32)direction, *GetNameSafe(Data ? Data->WeaponClass : nullptr));
	
	if (!Data || !Data->WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon failed. Dir=%d Mesh=%s"),
			(int32)direction, *GetNameSafe(Data ? Data->WeaponClass : nullptr));
		return;
	}
	
	// 기존 무기 제거 
	if (IsValid(CurrentWeaponRef))
	{
		CurrentWeaponRef->Destroy();
		CurrentWeaponRef = nullptr;
	}
	
	// 새 무기 스폰 
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	
	CurrentWeaponRef = GetWorld()->SpawnActor<AMS_Weapon>(Data->WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon successed. Dir=%d CurrentWeaponRef=%s"),
			(int32)direction, *GetNameSafe(CurrentWeaponRef));
	if (!IsValid(CurrentWeaponRef))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: Spawn failed"));
		return;
	}
	
	// 손 소켓에 부착
	USkeletalMeshComponent* MeshComp = GetMesh();
	static const FName WeaponSocketName(TEXT("WeaponSocket")); 
	CurrentWeaponRef->AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponSocketName
	);
	
	UE_LOG(LogTemp, Warning, TEXT("Equipped weapon: %s"), *Data->DisplayName.ToString());
}

// 첫 무기 스폰
void AMS_Player::FirstWeaponSpawn(EWeaponDirection direction)
{
	if (!WeaponMeshComp) return;
	
	FMS_WeaponSlotData* Data = WeaponSlotDataMap.Find(direction);
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon successed. Dir=%d Mesh=%s"),
			(int32)direction, *GetNameSafe(Data ? Data->WeaponClass : nullptr));
	
	if (!Data || !Data->WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon failed. Dir=%d Mesh=%s"),
			(int32)direction, *GetNameSafe(Data ? Data->WeaponClass : nullptr));
		return;
	}
	
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	
	CurrentWeaponRef = GetWorld()->SpawnActor<AMS_Weapon>(Data->WeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, Params);
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon successed. Dir=%d CurrentWeaponRef=%s"),
			(int32)direction, *GetNameSafe(CurrentWeaponRef));
	if (!IsValid(CurrentWeaponRef))
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipWeapon: Spawn failed"));
		return;
	}
	
	// 손 소켓에 부착
	USkeletalMeshComponent* MeshComp = GetMesh();
	static const FName WeaponSocketName(TEXT("WeaponSocket")); 
	CurrentWeaponRef->AttachToComponent(
		MeshComp,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponSocketName
	);
}

// 중계 함수
void AMS_Player::HandleFire()
{
	if (CurrentWeaponRef)
	{
		CurrentWeaponRef->Fire();
	}
}

void AMS_Player::ApplyRecoilFromWeapon(const FMS_RecoilSpec& Spec, int32 ShotIndex, bool bIsADS)
{
	// === 저장(틱에서 복구 속도 등 필요) ===
	CameraReturnSpeed = Spec.ReturnSpeed;
	SustainDecaySpeed = Spec.SustainDecaySpeed;
	WeaponKickInSpeed = Spec.WeaponKickInSpeed;
	WeaponReturnSpeed = Spec.WeaponReturnSpeed;
	
	// === 연사 누적 배수 업데이트 ===
	SustainMul = FMath::Clamp(SustainMul + Spec.SustainPerShot, 1.0f, Spec.SustainMax);
	LastRecoilTime = GetWorld()->GetTimeSeconds();

	// === 이번 샷의 (Pitch, Yaw) 계산 ===
	float PitchKick = 0.f;
	float YawKick = 0.f;
	
	const bool bUsePattern = (Spec.Pattern.Num() > 0);
	if (bUsePattern)
	{
		const int32 Index = ShotIndex % Spec.Pattern.Num();
		PitchKick = Spec.Pattern[Index].X;
		YawKick = Spec.Pattern[Index].Y;
	}
	else
	{
		PitchKick = FMath::RandRange(Spec.PitchKickMin, Spec.PitchKickMax);
		YawKick   = FMath::RandRange(Spec.YawKickMin, Spec.YawKickMax);
	}
	
	// ADS 보정
	const float ADSMul = bIsADS ? Spec.ADS_Multiplier : 1.0f;

	PitchKick *= ADSMul * SustainMul;
	YawKick   *= ADSMul * SustainMul;

	// === 카메라 목표 누적 ===
	// Pitch는 위로 튀는 느낌을 위해 "Target에 +"
	RecoilTarget.X += PitchKick;
	RecoilTarget.Y += YawKick;

	// === 무기 메시 킥백 목표 ===
	// -X로 뒤로, +Z로 살짝 위로 (너 무기 축이 다르면 조정)
	const float KickDist = Spec.KickbackDistance * ADSMul;
	const float KickUp   = Spec.KickbackUp * ADSMul;
	const float KickRotP = Spec.KickRotPitch * ADSMul;

	WeaponKickTargetLoc += FVector(-KickDist, 0.f, KickUp);
	WeaponKickTargetRot += FRotator(-KickRotP, 0.f, 0.f);

	// === 카메라 쉐이크 예약 ===
	FireShakeClass = Spec.FireShake;
	PendingShakeScale = bIsADS ? Spec.ADS_ShakeScale : Spec.ShakeScale;
}

void AMS_Player::DecaySustainTick(float DeltaTime)
{
	// 마지막 반동 이후 시간이 지나면 SustainMul이 1.0으로 복귀
	const float Now = GetWorld()->GetTimeSeconds();
	const float Since = Now - LastRecoilTime;

	// 0.12초 정도 지나면 복구 시작(취향값)
	if (Since > 0.12f)
	{
		SustainMul = FMath::FInterpTo(SustainMul, 1.0f, DeltaTime, SustainDecaySpeed);
	}
}

void AMS_Player::ApplyCameraRecoilTick(float DeltaTime)
{
	// (1) RecoilCurrent가 RecoilTarget을 따라가게 보간
	RecoilCurrent.X = FMath::FInterpTo(RecoilCurrent.X, RecoilTarget.X, DeltaTime, 30.0f); // 튀는 속도(고정/취향)
	RecoilCurrent.Y = FMath::FInterpTo(RecoilCurrent.Y, RecoilTarget.Y, DeltaTime, 30.0f);

	// (2) 이번 프레임에 증가한 만큼만 컨트롤 입력으로 적용 (중요!)
	const FVector2D Delta = RecoilCurrent - RecoilPrev;

	AController* C = GetController();
	if (C)
	{
		// Pitch: 화면 위로 튀려면 보통 Pitch를 "음수"로 넣음(게임마다 축이 다를 수 있음)
		AddControllerPitchInput(-Delta.X);
		AddControllerYawInput(Delta.Y);
	}

	RecoilPrev = RecoilCurrent;

	// (3) Target을 0으로 되돌리기(복구)
	RecoilTarget.X = FMath::FInterpTo(RecoilTarget.X, 0.0f, DeltaTime, CameraReturnSpeed);
	RecoilTarget.Y = FMath::FInterpTo(RecoilTarget.Y, 0.0f, DeltaTime, CameraReturnSpeed);

	// (4) Current도 0쪽으로 같이 정리(잔 떨림 방지)
	RecoilCurrent.X = FMath::FInterpTo(RecoilCurrent.X, 0.0f, DeltaTime, CameraReturnSpeed * 0.5f);
	RecoilCurrent.Y = FMath::FInterpTo(RecoilCurrent.Y, 0.0f, DeltaTime, CameraReturnSpeed * 0.5f);
	RecoilPrev = RecoilCurrent;
}
void AMS_Player::ApplyWeaponKickTick(float DeltaTime)
{
	if (!WeaponMeshComp)
		return;

	// Target은 0으로 복귀
	WeaponKickTargetLoc = FMath::VInterpTo(WeaponKickTargetLoc, FVector::ZeroVector, DeltaTime, WeaponReturnSpeed);
	WeaponKickTargetRot = FMath::RInterpTo(WeaponKickTargetRot, FRotator::ZeroRotator, DeltaTime, WeaponReturnSpeed);

	// Current가 Target을 따라가며 “튐”
	WeaponKickCurrentLoc = FMath::VInterpTo(WeaponKickCurrentLoc, WeaponKickTargetLoc, DeltaTime, WeaponKickInSpeed);
	WeaponKickCurrentRot = FMath::RInterpTo(WeaponKickCurrentRot, WeaponKickTargetRot, DeltaTime, WeaponKickInSpeed);

	// 베이스 + 킥 적용
	WeaponMeshComp->SetRelativeLocation(WeaponBaseLoc + WeaponKickCurrentLoc);
	WeaponMeshComp->SetRelativeRotation((WeaponBaseRot + WeaponKickCurrentRot).Quaternion());
}

void AMS_Player::PlayPendingShake()
{
	if (!FireShakeClass)
		return;

	AMS_PlayerController* PC = Cast<AMS_PlayerController>(GetController());
	if (!PC)
	{
		FireShakeClass = nullptr;
		return;
	}

	PC->ClientStartCameraShake(FireShakeClass, PendingShakeScale);

	// 1회만 실행
	FireShakeClass = nullptr;
}