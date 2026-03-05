// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GearCharacter.h"
#include "MS_DamageableCharacter.h"
#include "NPCCharacter.generated.h"

// 클래스 전방 선언 (컴파일 속도 최적화)
class UStateTreeComponent;

UCLASS()
class GEAROFTHREE_API ANPCCharacter : public AMS_DamageableCharacter
{
	GENERATED_BODY()

public:
	ANPCCharacter();
	
	// 무기나 태스크가 타겟을 물어볼 때 사용
	AActor* GetCurrentTarget() const { return CurrentTargetActor; }
	
	// Evaluator가 타겟을 찾아내면 호출할 함수
	void SetCurrentTargetActor(AActor* NewTarget) { CurrentTargetActor = NewTarget; }

public:
	// --- AI 컴포넌트 ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC | AI")
	UStateTreeComponent* StateTreeComponent;

	// 이 변수는 State Tree의 Task나 Service에서 업데이트해줘야 합니다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "NPC | AI")
	AActor* CurrentTargetActor;
	
	// 주변에서 가장 가까운 적을 찾아 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "NPC | AI")
	AActor* FindClosestEnemy();
	
	
	//여기부터 사망로직
public:
	// 🔹 사망 여부 체크 (타겟팅 제외용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsDead = false;

	// 🔹 사망 애니메이션 몽타주 (블루프린트에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* DeathMontage;

	// 🔹 사망 처리 함수
	UFUNCTION(BlueprintCallable, Category = "Status")
	virtual void Die();

protected:
	// 🔹 애니메이션이 끝난 후 실제로 파괴할 함수
	void DestroyAfterDeath();
    
	FTimerHandle DeathTimerHandle;
};
