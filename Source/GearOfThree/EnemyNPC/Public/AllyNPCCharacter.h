// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AllyGunActor.h"
#include "EnemyNPC/Public/NPCCharacter.h"
#include "AllyNPCCharacter.generated.h"

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
	TSubclassOf<AAllyGunActor> StartingWeaponClass;
	
	// 블루프린트에서도 편집할 수 있도록 VisibleAnywhere를 줍니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Appearance")
	USkeletalMeshComponent* AllyMesh;
	
	void Fire();
	
protected:
	// 🚩 기존 단일 포인터에서 배열(TArray)로 변경
	// 모든 머티리얼 슬롯의 다이내믹 인스턴스를 저장합니다.
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> DynamicMaterials;

public:
	// 팀을 변경하고 외형을 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "Teams")
	void SetTeamSide(bool bIsEnemy);
};
