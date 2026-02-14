// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Character.h"
#include "Components/StateTreeComponent.h"
#include "EnemyNPCCharacter.generated.h"

// 전방 선언 (헤더 파일 의존성 줄이기)

class UStateTreeComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UWeaponComponent;
class ASawGunActor;


/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class GEAROFTHREE_API AEnemyNPCCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	UStateTreeComponent* StateTreeComponent;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	
protected:
	virtual void BeginPlay() override;
	
	// 무기 관리 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent;

	// 에디터에서 할당할 무기 블루프린트 클래스 (BP_SawGun)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ASawGunActor> StartingWeaponClass;
	
	// 블루프린트에서도 편집할 수 있도록 VisibleAnywhere를 줍니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Appearance")
	class USkeletalMeshComponent* SionMesh;

	// Input Mapping Context
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* IMC_EnemyNPC;
	
	// Input Mapping Context
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* IMC_EnemyNPCLook;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* FireAction;

	// [함수] 발사 키를 눌렀을 때 호출됨
	void FireSawBlade(const FInputActionValue& Value);

public:

	/** Constructor */
	AEnemyNPCCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	// [전투 변수] 에디터에서 수정 가능
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxAmmo = 10;

	// [실시간 상태]
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	int32 CurrentAmmo = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsReloading = false;

	// [전투 함수]
	void DecreaseAmmo();
	void ReloadWeapon();
};