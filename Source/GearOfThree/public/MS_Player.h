// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GearCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputMappingContext.h"
#include "MS_Weapon.h"
#include "MS_WeaponWheelWidget.h"
#include "WeaponDirection.h"
#include "MS_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS()
class AMS_Player : public AGearCharacter // 초점 이동 불가능
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected: // 지역변수 초기화 구역

	// Aim Down Sight
	// bool bIsADS = false;
	//
	// float TargetArmLength;
	//
	// FVector TargetSocketOffset;
	// // 시야각
	// float TargetFOV;
	
	
protected:
	
	// InputContext 지정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	class UInputMappingContext* DefaultMappingContext;
	
	/** Gameplay initialization */
	virtual void BeginPlay() override;
	
	virtual void Tick( float DeltaTime ) override;
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	// Sprint
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SprintAction;
	
	// Crouch
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* CrouchAction;
	
	// ZoomIn
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ZoomInAction;
	
	// OpenSlot
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* OpenWeaponSlotAction;
	
	// Fire
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* FireAction;
	
protected:
	// 최대 속력 값 (default 값 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SprintSpeed = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool IsCrouched = false;
	
protected: // 초점 변경 내용
	
	// 보간 속도
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float ADSInterpSpeed = 12.0f;
	
	// 기본 시야각
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float DefaultFOV = 90.0f;
	
	// 확대 느낌
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float ADSFOV = 30.0f; // 

	// 클수록 캐릭터가 멀고 작게 된담
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float DefaultArmLength = 380.0f;
	
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float ADSArmLength = 220.0f;

	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	FVector DefaultSocketOffset = FVector(0.0f, 75.0f, 70.0f);
	
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	FVector ADSSocketOffset = FVector(0.0f, 60.0f, 65.0f);
	
public:
	// 런타임 캐시(현재 컴포넌트 값 저장용)
	float CachedArmLength = 0.f;
	FVector CachedSocketOffset = FVector::ZeroVector;
	float CachedFOV = 0.f;
	bool bCached = false;
	
public:
	// 초기 상태
	float TargetFOV;
	float TargetArmLength;
	FVector TargetSocketOffset;
	
	// 안전하게 컴포넌트/값 초기화 해두기
	void CachedDefaults_Base();
	
	bool bCachedDefaults = false;
	bool bWantsADS = false;
	
public:
	bool IsWantsADS() const;
	
protected: // 무기 관련 내용
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<AMS_Weapon> CurrentWeaponRef = nullptr;

	bool bSlotOpen = false;
	
	UPROPERTY(EditDefaultsOnly, Category="UI|WeaponWheel")
	TSubclassOf<class UUserWidget> WeaponWheelWidgetClass;
	
	// 생성된 위젯 인스턴스 캐시
	UPROPERTY(Transient)
	UMS_WeaponWheelWidget* WeaponWheelWidget = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="weapon")
	TMap<EWeaponDirection, TSubclassOf<AMS_Weapon>> WeaponClassMap;
	
	UPROPERTY(Transient)
	TMap<EWeaponDirection, TObjectPtr<AMS_Weapon>> WeaponMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMeshComp;
	
	UPROPERTY()
	TMap<EWeaponDirection, FMS_WeaponSlotData> WeaponSlotDataMap;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> AR15WeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> K9RavClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> AK105WeaponClass;

public:

	// 생성자
	AMS_Player();

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	
	// 빠르게 달림
	void Sprint(const FInputActionValue& Value);
	
	// 빠르게 달리는것을 멈춤
	void StopSprint();

	// 몸 구부리기
	void StartCrouch(const FInputActionValue& Value);
	
	// 몸 일어서기
	void StopCrouch();
	
	void StartADS();
	
	void StopADS();
	
	void ToggleWeaponSlot();
	
	void ShowWeaponWheelUI(bool bShow);
	
	void EquipWeapon(EWeaponDirection direction);
	
	void FirstWeaponSpawn(EWeaponDirection direction);

	void HandleFire();
public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC | AI")
	class UCombatDialogueComponent* CombatDialogueComponent;
};

