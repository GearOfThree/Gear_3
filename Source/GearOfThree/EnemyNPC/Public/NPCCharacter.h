// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GearOfThreeTypes.h"
#include "NPCCharacter.generated.h"

// 클래스 전방 선언 (컴파일 속도 최적화)
class UStateTreeComponent;

UCLASS()
class GEAROFTHREE_API ANPCCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANPCCharacter();

protected:
	// 컴포넌트 초기화 완료 시점에 호출되는 함수
	virtual void PostInitializeComponents() override;

public:
	// --- AI 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC | AI")
	UStateTreeComponent* StateTreeComponent;

	// --- 전투 데이터 ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC | Combat")
	int32 MaxAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC | Combat")
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC | Combat")
	bool bIsReloading;

	// --- 팀 구분 (공용 헤더의 Enum 사용) ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC | Combat")
	ETeamSide TeamSide;
	
	// 주변에서 가장 가까운 적을 찾아 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "NPC | AI")
	AActor* FindClosestEnemy();

	// --- 전투 함수 ---
	virtual void DecreaseAmmo();
	virtual void ReloadWeapon();
};
