// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "InputMappingContext.h"
#include "MS_Weapon.h"
#include "MS_Player.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMS_Player : public ACharacter
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
	bool bIsADS = false;
	
	float TargetArmLength;
	
	FVector TargetSocketOffset;
	// 시야각
	float TargetFOV;
	
	
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
	
protected:
	// 최대 속력 값 (default 값 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	float SprintSpeed = 800.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement")
	bool IsCrouched = false;
	
protected:
	// 기본값
	UPROPERTY(EditAnywhere, Category="ADS")
	float HipArmLength = 300.f;

	UPROPERTY(EditAnywhere, Category="ADS")
	FVector HipSocketOffset = FVector(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category="ADS")
	float HipFOV = 120.f;

	// ADS 값 (오른쪽 어깨)
	UPROPERTY(EditAnywhere, Category="ADS")
	float ADSArmLength = 150.f;

	UPROPERTY(EditAnywhere, Category="ADS")
	FVector ADSSocketOffset = FVector(0.f, 70.f, 60.f);

	UPROPERTY(EditAnywhere, Category="ADS")
	float ADSFOV = 65.f;

	UPROPERTY(EditAnywhere, Category="ADS")
	float InterpSpeed = 12.f;
	
	void SetADS(bool bNewADS);
	
protected:
	UPROPERTY()
	AMS_Weapon* CurrentWeapon;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AMS_Weapon> StarterWeaponClass;
	
public:

	// 생성자
	AMS_Player();

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	void ChangeWeapon(TSubclassOf<AMS_Weapon> NewWeaponClass);
	
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
};

