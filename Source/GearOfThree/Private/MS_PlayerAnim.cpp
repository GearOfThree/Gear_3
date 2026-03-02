// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_PlayerAnim.h"

#include "MS_Player.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMS_PlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// 플레이어의 이동 속도를 가져와 speed 에 할당하고 싶다
	// 1. 소유 폰 얻어오기
	auto ownerPawn = TryGetPawnOwner();
	// 2. 플레이어로 캐스팅하기
	auto player = Cast<AMS_Player>(ownerPawn);
	
	// 캐스팅에 성공했다면 
	if (player)
	{
		// 3. 이동 속도 필요
		FVector velocity = player->GetVelocity();
		// 4. 플레이어의 전방 벡터가 필요
		FVector forwardVector = player->GetActorForwardVector();
		// 5. Speed 에 값 할당하기
		speed = FVector::DotProduct(forwardVector, velocity);
	
		auto movement = player->GetCharacterMovement();
		bIsInAir = movement->IsFalling();
	}
	
}
