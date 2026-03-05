// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MS_PlayerAnim.generated.h"

/**
 * 
 */
UCLASS()
class GEAROFTHREE_API UMS_PlayerAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	// 플레이어의 이동 속도 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float Speed = 0.f;
	
	// 플레이어 좌우 이동 속도 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	float direction = 0;
	
	// 몽타주 등록하기
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	TObjectPtr<UAnimMontage> DiveRollMontage;

	// 몽타주 재생 함수
	UFUNCTION(BlueprintCallable)
	void PlayDiveRoll();
	
	// 매 프레임 갱신되는 함수 
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	// 픞레이어가 공중에 있는지 여부 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	bool bIsInAir = false;
	
	// 구부리고 있는지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerAnim")
	bool bIsCrouched = false;
	
	// 죽었는지 여부 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerAnim")
	bool bIsDead = false;

	// Crouch 상태로 죽었는지 확인
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="PlayerAnim")
	bool bWasCrouchedOnDeath = false;
};
