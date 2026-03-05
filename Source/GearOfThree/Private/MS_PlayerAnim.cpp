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
	if (!player) return;
	
	// 3. 이동 속도 필요
	FVector velocity = player->GetVelocity();
	// 4. 플레이어의 전방 벡터가 필요
	FVector forwardVector = player->GetActorForwardVector();
	// 5. Speed 에 값 할당하기
	Speed = FVector::DotProduct(forwardVector, velocity);
	// 6. 좌우 속도 할당하기
	FVector rightVector = player->GetActorRightVector();
	direction = FVector::DotProduct(rightVector, velocity);
	
	bIsCrouched = player->bIsCrouched;
	
	// 플레이어가 현재 공중에 있는지 여부를 기억하고 싶다. 
	UCharacterMovementComponent* movement = player->GetCharacterMovement();
		
	
	
	
	
}

void UMS_PlayerAnim::PlayDiveRoll()
{
	// 등록됐는지 확인하고 
	if (!DiveRollMontage) return;
	
	bool bAlready = Montage_IsPlaying(DiveRollMontage);
	UE_LOG(LogTemp, Warning, TEXT("Before Play: IsPlaying=%d"), bAlready);

	const float Len = Montage_Play(DiveRollMontage, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Montage_Play Len=%.3f NowPlaying=%d"),
		Len, Montage_IsPlaying(DiveRollMontage));
	
	// 다른 몽타주 재생중일때 애니메이션 재생을 막는다. 
	if (Montage_IsPlaying(DiveRollMontage)) return;
	
	Montage_Play(DiveRollMontage, 1.0f);
}
