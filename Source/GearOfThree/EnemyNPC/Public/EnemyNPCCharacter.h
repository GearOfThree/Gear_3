// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "NPCCharacter.h"
#include "EnemyNPCCharacter.generated.h"

// 전방 선언 (헤더 파일 의존성 줄이기)

class UNiagaraSystem;
class UStateTreeComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UWeaponComponent;
class ASawGunActor;


USTRUCT(BlueprintType)
struct FGibData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> GibMeshes; // 터져나갈 고기 덩어리들

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* BloodEffect;    // 피보라 효과
};

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

	virtual void ReceiveDamage_Implementation(float Damage, AActor* DamageCauser) override;
public:

	/** Constructor */
	AEnemyNPCCharacter();	
	
protected:
	// 단계별 실행 여부 체크
	bool bGibStage75 = false;
	bool bGibStage50 = false;
	bool bGibStage25 = false;

	// 단계별 파손될 본(Bone) 이름 설정 (에디터에서 수정 가능하도록)
	UPROPERTY(EditAnywhere, Category = "Gore")
	FName BoneToHide75 = TEXT("arm_l");

	UPROPERTY(EditAnywhere, Category = "Gore")
	FName BoneToHide50 = TEXT("arm_r");

	UPROPERTY(EditAnywhere, Category = "Gore")
	FName BoneToHide25 = TEXT("leg_l");

	// 니아가라 피보라 효과
	UPROPERTY(EditAnywhere, Category = "Gore")
	class UNiagaraSystem* BloodEffect;

	// 실제 파손 처리를 담당할 함수
	void ExecutePartialGib(FName BoneName);
};