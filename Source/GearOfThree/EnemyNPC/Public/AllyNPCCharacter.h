// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyNPC/Public/NPCCharacter.h"
#include "AllyNPCCharacter.generated.h"

class ASawGunActor;
class UWeaponComponent;
/**
 * 
 */
UCLASS()
class GEAROFTHREE_API AAllyNPCCharacter : public ANPCCharacter
{
	GENERATED_BODY()

public:
	AAllyNPCCharacter();

protected:
	virtual void BeginPlay() override;
	
	// 무기 관리 컴포넌트 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent;

	// 에디터에서 할당할 무기 블루프린트 클래스 (BP_AllyGun)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	TSubclassOf<ASawGunActor> StartingWeaponClass;
	
	// 블루프린트에서도 편집할 수 있도록 VisibleAnywhere를 줍니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Appearance")
	USkeletalMeshComponent* AllyMesh;
	
	void Fire();
};
