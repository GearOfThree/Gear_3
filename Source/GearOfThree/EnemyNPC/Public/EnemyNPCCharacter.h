// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "NPCCharacter.h"
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
class GEAROFTHREE_API AEnemyNPCCharacter : public ANPCCharacter
{
	GENERATED_BODY()

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

	// [함수] 발사 키를 눌렀을 때 호출됨
	void FireSawBlade(const FInputActionValue& Value);

public:

	/** Constructor */
	AEnemyNPCCharacter();	
};