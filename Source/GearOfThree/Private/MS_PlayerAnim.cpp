// Fill out your copyright notice in the Description page of Project Settings.


#include "MS_PlayerAnim.h"

#include "MS_Player.h"

void UMS_PlayerAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// 플레이어의 이동 속도를 가져와 speed 에 할당하고 싶다
	auto ownerPawn = TryGetPawnOwner();
	// 플레이어로 캐스팅하기
	auto player = Cast<AMS_Player>(ownerPawn);
	
	// 캐스팅에 성공했다면 
	if (player)
	{
		FVector velocity = player->GetVelocity();
		
		FVector forwardVector = player->GetActorForwardVector();
		
		speed = FVector::DotProduct(forwardVector, velocity);
	}
	
}
