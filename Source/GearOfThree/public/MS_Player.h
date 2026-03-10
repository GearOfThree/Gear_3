// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameOverUI.h"
#include "GearCharacter.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputMappingContext.h"
#include "MS_DamageableCharacter.h"
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
class AMS_Player : public AMS_DamageableCharacter
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
protected: // 모션

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RollAction;

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
	
public:
	void OnRollStart();
	
	void OnRollEnd();
	
	bool bIsRolling = false;
	
	bool bCrouched = false;
	
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
	
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float NormalLookSensitivity = 1.0f;
	
	UPROPERTY(EditAnywhere, Category="ADS|Settings")
	float ADSLookSensitivity = 0.3f;
	
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
	// bool bIsFiring = false;
	
protected: // 게임 오버 위젯
	UPROPERTY(Transient)
	UGameOverUI* GameOverWidget = nullptr;
	
	FTimerHandle DeadTimerHandle;
	
	void AfterDead();
	
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

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// USkeletalMeshComponent* WeaponMeshComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponMeshComp;
	
	UPROPERTY()
	TMap<EWeaponDirection, FMS_WeaponSlotData> WeaponSlotDataMap;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> AR15WeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> K9RavClass;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AMS_Weapon> AK105WeaponClass;

protected: // 카메라 반동 설정
	
	// 누적 목표(튀어야 하는 총량)
	FVector2D RecoilTarget = FVector2D::ZeroVector;     // (Pitch, Yaw)
	// 실제 적용 중(보간으로 따라감)
	FVector2D RecoilCurrent = FVector2D::ZeroVector;

	// Tick에서 delta 적용을 위해 이전 값 저장
	FVector2D RecoilPrev = FVector2D::ZeroVector;

	// 연사 누적 배수
	float SustainMul = 1.0f;

	// 최근 발사 시각 (Sustain 복구에 사용)
	float LastRecoilTime = -9999.f;
	
	// 마지막으로 받은 스펙 일부(복구 속도 등 Tick에서 필요)
	float CameraReturnSpeed = 18.0f;
	float SustainDecaySpeed = 8.0f;
	
	// 쉐이크
	TSubclassOf<class UCameraShakeBase> FireShakeClass = nullptr;
	float PendingShakeScale = 1.0f;
	
protected: // 총기 반동 설정
	FVector WeaponKickTargetLoc = FVector::ZeroVector;
	FRotator WeaponKickTargetRot = FRotator::ZeroRotator;

	FVector WeaponKickCurrentLoc = FVector::ZeroVector;
	FRotator WeaponKickCurrentRot = FRotator::ZeroRotator;
	
	// 총기 위치 복구 기준점
	FVector WeaponBaseLoc = FVector::ZeroVector;
	FRotator WeaponBaseRot = FRotator::ZeroRotator;
	
	// 마지막으로 받은 스펙 일부(복구 속도 등 Tick에서 필요)
	float WeaponKickInSpeed = 35.0f;
	float WeaponReturnSpeed = 22.0f;
	
private: // 반동 설정
	
	void ResolveWeaponMeshComponent();
	void ApplyCameraRecoilTick(float DeltaTime);
	void ApplyWeaponKickTick(float DeltaTime);
	void DecaySustainTick(float DeltaTime);
	void PlayPendingShake();
	
public:

	// 생성자
	AMS_Player();

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void DiveRoll(const FInputActionValue& Value);
	// void OnJumpCompleted();
	
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
	
	void HandleDead();
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
	
	// MS_Weapon 에서 호출한다. 
	void ApplyRecoilFromWeapon(const FMS_RecoilSpec& Spec, int32 ShotIndex, bool bIsADS);
};

